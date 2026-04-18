#include "pythonprocessactornode.h"
#include <QSharedMemory>
#include <QJsonArray>
#include <QProcess>
#include <QUuid>
#include <QCoreApplication>
#include <constants.h>
#include <contexts/pythonprocessdatacontext.h>
#include <models/nodetabledialogmodel.h>
#include <models/nodeparamlistdialogmodel.h>
#include <data/contextmetadata.h>
#include <unordered_set>
namespace Pipeline
{
    namespace Runtime
    {
        PythonProcessActorNode::PythonProcessActorNode()
            : ActorNode()
            , m_nodeParamListModel(new NodeParamListModel())
            , m_inputDataTable(new NodeTableModel())
            , m_outputDataTable(new NodeTableModel())
            , m_useInputTable(false)
        {
            this->m_nodeParamListModel->addParameter("Name", (int)ParamType::String, "");
            this->m_nodeParamListModel->addParameter("Python File", (int)ParamType::Browse, "");
        }

        PythonProcessActorNode::~PythonProcessActorNode()
        {
            m_nodeParamListModel->deleteLater();
            m_inputDataTable->deleteLater();
            m_outputDataTable->deleteLater();
        }

        QVariant PythonProcessActorNode::behaviour(const Thread::BehaviourContext &behaviour)
        {
            //auto parameters = m_inputParameterDataTable->getRoot();
            auto data = m_inputDataTable->getRoot();
            auto serializedData = data->serialize();
            QByteArray buffer(
                reinterpret_cast<const char*>(serializedData.data()),
                static_cast<int>(serializedData.size())
            );
            m_pythonThrowError = false;
            QVariant result;
            QProcess process;
            QString filename = this->data(Constants::NodeRoles::PythonFileName).toString();

            if (filename.isEmpty())
            {
                return false;
            }

            process.start("C:\\Users\\yerli\\AppData\\Local\\Programs\\Python\\Python39\\python.exe", QStringList() << filename);

            if (!process.waitForStarted())
            {
                qDebug() << "Python is not started";
                return false;
            }

            process.write(buffer);
            process.closeWriteChannel();
            process.waitForFinished();
            auto error = process.readAllStandardError();
            QByteArray outputData = process.readAllStandardOutput();
            int exitCode = process.exitCode();
            auto status = process.exitStatus();

            if (status == QProcess::NormalExit && exitCode == 0)
            {
                m_pythonError += QString(!m_pythonError.isEmpty() ? "\n" : "") + "Process is finished successfully.";
            }
            else
            {
                m_pythonError = error;
                m_pythonThrowError = true;
            }

            try
            {
                if (!m_pythonThrowError)
                {
                    const uint8_t* outputDataU8 = reinterpret_cast<const uint8_t*>(outputData.constData());
                    size_t size = static_cast<size_t>(outputData.size());
                    auto* outputResult = HierarchicalTableData::deserialize(outputDataU8, size);
                    result = QVariant::fromValue<std::shared_ptr<HierarchicalTableData>>(std::shared_ptr<HierarchicalTableData>(outputResult));
                    return result;
                }
            }
            catch (std::runtime_error &error)
            {
                m_pythonError = error.what();
            }

            return result;
        }

        QHash<int, QByteArray> PythonProcessActorNode::roleNames() const
        {
            auto roles = ActorNode::roleNames();
            roles[Constants::NodeRoles::PythonFileName] = "pythonFilename";
            roles[Constants::NodeRoles::InputTableModel] = "inputTableModel";
            roles[Constants::NodeRoles::NodeParameterListModel] = "nodeParameterListModel";
            roles[Constants::NodeRoles::OutputTableModel] = "outputTableModel";
            roles[Constants::NodeRoles::PythonError] = "pythonError";
            roles[Constants::NodeRoles::NodeRunningState] = "runningState";
            return roles;
        }

        bool PythonProcessActorNode::setData(const QVariant &value, int role, bool emitSignal)
        {
            bool result = false;

            if (role == Constants::NodeRoles::PythonFileName)
            {
                this->m_nodeParamListModel->setData("Python File", value.toString(), Constants::ParameterRoles::ValueRole);
                result = true;
            }
            else if (role == UI::Roles::Name)
            {
                this->m_nodeParamListModel->setData("Name", value.toString(), Constants::ParameterRoles::ValueRole);
                result = true;
            }
            else if (role == Constants::NodeRoles::PythonError)
            {
                this->m_pythonError = value.toString();
                result = true;
            }
            else if (role == Constants::NodeRoles::InputTableModel)
            {
                NodeTableModel* model = value.value<NodeTableModel*>();
                m_inputDataTable = model;
                result = true;
            }
            else if (role == Constants::NodeRoles::NodeParameterListModel)
            {
                NodeParamListModel* model = value.value<NodeParamListModel*>();
                m_nodeParamListModel = model;
                result = true;
            }
            else if (role == Constants::NodeRoles::OutputTableModel)
            {
                NodeTableModel* model = value.value<NodeTableModel*>();
                m_outputDataTable = model;
                result = true;
            }
            else
            {
                result = ActorNode::setData(value, role, false);
            }

            if (result && emitSignal && notifyChanged)
            {
                notifyChanged({role});
            }

            return result;
        }

        QVariant PythonProcessActorNode::data(int role) const
        {
            if (role == Constants::NodeRoles::PythonFileName)
            {
                return this->m_nodeParamListModel->data("Python File", Constants::ParameterRoles::ValueRole).toString();
            }
            else if (role == UI::Roles::Name)
            {
                return this->m_nodeParamListModel->data("Name", Constants::ParameterRoles::ValueRole).toString();
            }
            else if (role == Constants::NodeRoles::PythonError)
            {
                return m_pythonError;
            }
            else if (role == Constants::NodeRoles::InputTableModel)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(m_inputDataTable));
                return v;
            }
            else if (role == Constants::NodeRoles::NodeParameterListModel)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(m_nodeParamListModel));
                return v;
            }
            else if (role == Constants::NodeRoles::OutputTableModel)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(m_outputDataTable));
                return v;
            }
            else if (role == Constants::NodeRoles::NodeRunningState)
            {
                return this->getState();
            }
            else
            {
                return ActorNode::data(role);
            }
        }

        bool PythonProcessActorNode::addPort(Core::Port *port, bool isIn)
        {
            auto res = UI::MNode::addPort(port, isIn);
            bool has;

            if (isIn)
            {
                auto index = this->findInPortIndex(port, has);

                if (has)
                {
                    m_inputDataTable->setData(QModelIndex(), static_cast<int>(this->getInPortCount()), Constants::NodeTableRoles::Rows);
                    m_inputDataTable->setData(QModelIndex(), 1, Constants::NodeTableRoles::Columns);
                    auto childIndex = m_inputDataTable->createCell(m_inputDataTable->index(static_cast<int>(index), 0));
                    m_inputDataTable->setData(childIndex, QString::fromStdString(port->getName()), Constants::NodeTableRoles::CellName);
                }
            }
            else
            {
                auto index = this->findOutPortIndex(port, has);

                if (has)
                {
                    m_outputDataTable->setData(QModelIndex(), static_cast<int>(this->getOutPortCount()), Constants::NodeTableRoles::Rows);
                    m_outputDataTable->setData(QModelIndex(), 1, Constants::NodeTableRoles::Columns);
                    auto childIndex = m_outputDataTable->createCell(m_outputDataTable->index(static_cast<int>(index), 0));
                    m_outputDataTable->setData(childIndex, QString::fromStdString(port->getName()), Constants::NodeTableRoles::CellName);
                }
            }

            return res;
        }

        NodeContextMetadata PythonProcessActorNode::createMetadata() const
        {
            NodeContextMetadata metadata;
            metadata.setName(this->data(UI::Roles::Name).toString());
            metadata.setNodeType(Constants::NodeTypes::PythonNode);
            QJsonArray array;
            {
                for (int i = 0; i < this->m_nodeParamListModel->rowCount(); i++)
                {
                    auto index = this->m_nodeParamListModel->index(i, 0);

                    if (index.data(Constants::ParameterRoles::NameRole).toString() != "Name")
                    {
                        QJsonObject o;
                        o["name"] = index.data(Constants::ParameterRoles::NameRole).toString();
                        o["value"] = index.data(Constants::ParameterRoles::ValueRole).toString();
                        o["type"] = index.data(Constants::ParameterRoles::TypeRole).toInt();
                        array.append(o);
                    }
                }
            }
            metadata.add("parameters", array);
            QJsonArray inputPorts;
            {
                auto root = m_inputDataTable->getRoot();

                for (int inPortIndex = 0; inPortIndex < m_inputDataTable->rowCount(); inPortIndex++)
                {
                    QJsonObject input;
                    auto inPort = root->getCell(inPortIndex, 0);
                    input["name"] = QString::fromStdString(inPort->getName());
                    input["row_count"] = static_cast<int>(inPort->getRowCount());
                    input["column_count"] = static_cast<int>(inPort->getColumnCount());
                    const auto & headerList = inPort->getHeaders();
                    QJsonArray headers;

                    for (auto& pair : headerList)
                    {
                        QJsonObject h;
                        h["key"] = pair.first;
                        h["value"] = QString::fromStdString(pair.second);
                        headers.append(h);
                    }

                    input["header_data"] = headers;
                    inputPorts.append(input);
                }
            }
            metadata.add("inputPorts", inputPorts);
            QJsonArray outputPorts;
            {
                auto root = m_outputDataTable->getRoot();

                for (int outPortIndex = 0; outPortIndex < m_outputDataTable->rowCount(); outPortIndex++)
                {
                    QJsonObject output;
                    auto outPort = root->getCell(outPortIndex, 0);
                    output["name"] = QString::fromStdString(outPort->getName());
                    output["row_count"] = static_cast<int>(outPort->getRowCount());
                    output["column_count"] = static_cast<int>(outPort->getColumnCount());
                    const auto & headerList = outPort->getHeaders();
                    QJsonArray headers;

                    for (auto& pair : headerList)
                    {
                        QJsonObject h;
                        h["key"] = pair.first;
                        h["value"] = QString::fromStdString(pair.second);
                        headers.append(h);
                    }

                    output["header_data"] = headers;
                    inputPorts.append(output);
                }
            }
            metadata.add("outputPorts", outputPorts);
            return metadata;
        }

        BaseDataContext* PythonProcessActorNode::createDataContext(QObject *parent)
        {
            auto *context = new PythonProcessDataContext(parent);
            context->setPythonError(m_pythonError);
            context->setInputDataTable(m_inputDataTable);
            context->setNodeParameterListModel(m_nodeParamListModel);
            context->setOutputDataTable(m_outputDataTable);
            return context;
        }

        void PythonProcessActorNode::applyNodeContextMetadata(const NodeContextMetadata &nodeContextMetadata)
        {
            m_nodeParamListModel->clear();
            auto parameters = nodeContextMetadata.getProperty("parameters").toJsonArray();
            auto inputPorts = nodeContextMetadata.getProperty("inputPorts").toJsonArray();
            auto outputPorts = nodeContextMetadata.getProperty("outputPorts").toJsonArray();
            // add name parameter
            m_nodeParamListModel->addParameter("Name", (int)ParamType::String, nodeContextMetadata.getName());

            // apply parameteres
            for (auto param : parameters)
            {
                if (param.isObject())
                {
                    auto paramObj = param.toObject();

                    if (paramObj.contains("name") && paramObj.contains("type") && paramObj.contains("value"))
                    {
                        m_nodeParamListModel->addParameter(paramObj["name"].toString(), paramObj["type"].toInt(), paramObj["value"].toVariant());
                    }
                }
            }

            auto inPortRoot = m_inputDataTable->getRoot();

            if (inPortRoot)
            {
                inPortRoot->setSize(inputPorts.size(), 1);

                for (int inPortIndex = 0; inPortIndex < inputPorts.size(); inPortIndex++)
                {


                    auto inputValue = inputPorts.at(inPortIndex);

                    if (!inputValue.isObject())
                    {
                        continue;
                    }

                    auto inPortChildCell = inPortRoot->getOrCreateCell(inPortIndex, 0);

                    auto cellVariant = QVariant::fromValue<std::shared_ptr<HierarchicalTableData>>(inPortChildCell);
                    auto index = m_inputDataTable->index(inPortIndex, 0);
                    auto input = inputValue.toObject();
                    // apply input
                    inPortChildCell->setName(input["name"].toString().toStdString());
                    auto rowCount = input["row_count"].toInt(0);
                    auto columnCount = input["column_count"].toInt(0);
                    m_inputDataTable->setData(index, cellVariant, Constants::NodeTableRoles::ChildCell);
                    m_inputDataTable->setData(index, rowCount, Constants::NodeTableRoles::Rows);
                    m_inputDataTable->setData(index, columnCount, Constants::NodeTableRoles::Columns);
                    auto headers = input["header_data"].toArray();

                    auto* inPort = this->getInPort(inPortIndex);
                    if(inPort)
                    {
                        inPort->setName(input["name"].toString().toStdString());
                    }

                    for (auto header : headers)
                    {
                        auto headerObj = header.toObject();

                        if (headerObj.contains("key") && headerObj.contains("value"))
                        {
                            m_inputDataTable->setData(index, headerObj["value"].toString(), Constants::NodeTableRoles::HeaderData + headerObj["key"].toInt());
                        }
                    }
                }
            }

            auto outPortRoot = m_outputDataTable->getRoot();

            if (outPortRoot)
            {
                outPortRoot->setSize(outputPorts.size(), 1);

                for(int outPortIndex = 0; outPortIndex < outputPorts.size(); outPortIndex++)
                {
                    auto outputValue = outputPorts.at(outPortIndex);
                    if(!outputValue.isObject())
                    {
                        continue;
                    }

                    auto outPortChildCell = outPortRoot->getOrCreateCell(outPortIndex, 0);
                    auto cellVariant = QVariant::fromValue<std::shared_ptr<HierarchicalTableData>>(outPortChildCell);
                    auto index = m_outputDataTable->index(outPortIndex, 0);
                    auto output = outputValue.toObject();
                    // apply output
                    auto rowCount = output["row_count"].toInt(0);
                    auto columnCount = output["column_count"].toInt(0);
                    outPortChildCell->setName(output["name"].toString().toStdString());
                    m_outputDataTable->setData(index, cellVariant, Constants::NodeTableRoles::ChildCell);
                    m_outputDataTable->setData(index, rowCount, Constants::NodeTableRoles::Rows);
                    m_outputDataTable->setData(index, columnCount, Constants::NodeTableRoles::Columns);
                    auto headers = output["header_data"].toArray();

                    auto* outPort = this->getOutPort(outPortIndex);
                    if(outPort)
                    {
                        outPort->setName(output["name"].toString().toStdString());
                    }


                    for (auto header : headers)
                    {
                        auto headerObj = header.toObject();

                        if (headerObj.contains("key") && headerObj.contains("value"))
                        {
                            m_outputDataTable->setData(index, headerObj["value"].toString(), Constants::NodeTableRoles::HeaderData + headerObj["key"].toInt());
                        }
                    }
                }
            }

        }

        void PythonProcessActorNode::saveContext(BaseDataContext *dataContext)
        {
            auto *pythonContext = dynamic_cast<PythonProcessDataContext*>(dataContext);

            if (!pythonContext)
            {
                return;
            }

            this->m_pythonError = pythonContext->getPythonError();
            std::unordered_set<int> roles = {UI::Roles::Name, Constants::NodeRoles::PythonFileName, Constants::NodeRoles::PythonFileName, Constants::NodeRoles:: InputTableModel,
                                             Constants::NodeRoles::NodeParameterListModel,
                                             Constants::NodeRoles::OutputTableModel
                                            };

            if (auto *inputDialogModel = dynamic_cast<NodeTableDialogModel*>(pythonContext->getInputDataTable()))
            {
                for (size_t portIndex = 0; portIndex < this->getInPortCount(); portIndex++)
                {
                    auto *port = this->getInPort(portIndex);
                    auto refIndex = inputDialogModel->referenceModel()->index(static_cast<int>(portIndex), 0);
                    auto dilogIndex = inputDialogModel->index(static_cast<int>(portIndex), 0);

                    if (refIndex.data(Constants::NodeTableRoles::CellName) != dilogIndex.data(Constants::NodeTableRoles::CellName))
                    {
                        port->setName(dilogIndex.data(Constants::NodeTableRoles::CellName).toString().toStdString());
                    }
                }

                inputDialogModel->saveData();
            }

            if (auto *nodeParamListModel = dynamic_cast<NodeParamListDialogModel*>(pythonContext->getNodeParameterListModel()))
            {
                nodeParamListModel->saveData();
            }

            if (auto *outputDialogModel = dynamic_cast<NodeTableDialogModel*>(pythonContext->getOutputDataTable()))
            {
                for (size_t portIndex = 0; portIndex < this->getInPortCount(); portIndex++)
                {
                    auto *port = this->getOutPort(portIndex);
                    auto refIndex = outputDialogModel->referenceModel()->index(static_cast<int>(portIndex), 0);
                    auto dilogIndex = outputDialogModel->index(static_cast<int>(portIndex), 0);

                    if (refIndex.data(Constants::NodeTableRoles::CellName) != dilogIndex.data(Constants::NodeTableRoles::CellName))
                    {
                        port->setName(dilogIndex.data(Constants::NodeTableRoles::CellName).toString().toStdString());
                    }
                }

                outputDialogModel->saveData();
            }

            notifyChanged({roles.begin(), roles.end()});
        }

        void PythonProcessActorNode::onStarted()
        {
            if (!m_useInputTable)
            {
                auto behaviourContext = this->getContext();
                auto inputData = this->createInputDataFromContext(behaviourContext.m_variants);

                if (inputData)
                {
                    this->m_inputDataTable->setRoot(inputData);
                }
            }
        }

        void PythonProcessActorNode::onFinished(const QVariant& result)
        {
            // this is main thread we need to set root here beacuse of UI update
            if (!m_pythonThrowError)
            {
                auto outputResult = result.value<std::shared_ptr<HierarchicalTableData>>();
                m_outputDataTable->setRoot(outputResult);
                notifyChanged({Constants::NodeRoles::NodeRunningState});
            }
        }

        void PythonProcessActorNode::onFailed(const QVariant& result)
        {
        }

        void PythonProcessActorNode::inConnectionChanged(UI::MPort *inPort, UI::MPort* outPort)
        {
            QList<QVariant> dependentDataList;

            if (!outPort)
            {
                return;
            }

            bool has;
            size_t outputIndex = outPort->getOwnerNode()->findOutPortIndex(outPort, has);

            if (!has)
            {
                return;
            }

            for (size_t i = 0; i < inPort->getConnectionCount(); i++)
            {
                auto connection = inPort->getConnection(i);
                auto *dependentNode = dynamic_cast<PythonProcessActorNode*>(connection->getOutPort()->getOwnerNode());

                if (dependentNode)
                {
                    auto outputTable = dependentNode->data(Constants::NodeRoles::OutputTableModel).value<NodeTableModel*>();

                    if (outputTable)
                    {
                        QVariant v = outputTable->data(outputTable->index(static_cast<int>(outputIndex), 0), Constants::NodeTableRoles::ChildCell);
                        dependentDataList.append(v);
                    }
                }
            }

            size_t portIndex = this->findInPortIndex(inPort, has);

            if (has)
            {
                auto inputData = this->createInputDataFromContext(dependentDataList);
                inputData->setName(inPort->getName());

                if (inputData)
                {
                    QVariant data = QVariant::fromValue<std::shared_ptr<HierarchicalTableData>>(inputData);
                    this->m_inputDataTable->setData(this->m_inputDataTable->index(static_cast<int>(portIndex), 0), data, Constants::NodeTableRoles::ChildCell);
                }
            }
        }

        void PythonProcessActorNode::outConnectionChanged(UI::MPort *outPort, UI::MPort *inPort)
        {
        }

    }
}


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
            this->m_nodeParamListModel->addParameter("CSV Data", (int)ParamType::Browse, "");
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
            QString filename = this->data(NodeRoles::PythonFileName).toString();

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
                m_pythonError += QString(!m_pythonError.isEmpty() ? "\n": "") + "Process is finished successfully.";
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
            roles[NodeRoles::PythonFileName] = "pythonFilename";
            roles[NodeRoles::InputTableModel] = "inputTableModel";
            roles[NodeRoles::NodeParameterListModel] = "nodeParameterListModel";
            roles[NodeRoles::OutputTableModel] = "outputTableModel";
            roles[NodeRoles::PythonError] = "pythonError";
            roles[NodeRoles::NodeRunningState] = "runningState";
            return roles;
        }

        bool PythonProcessActorNode::setData(const QVariant &value, int role, bool emitSignal)
        {
            bool result = false;

            if (role == NodeRoles::PythonFileName)
            {
                this->m_nodeParamListModel->setData("Python File", value.toString(), ParameterRoles::ValueRole);
                result = true;
            }
            else if (role == UI::Roles::Name)
            {
                this->m_nodeParamListModel->setData("Name", value.toString(), ParameterRoles::ValueRole);
                result = true;
            }
            else if (role == NodeRoles::PythonError)
            {
                this->m_pythonError = value.toString();
                result = true;
            }
            else if (role == NodeRoles::InputTableModel)
            {
                NodeTableModel* model = value.value<NodeTableModel*>();
                m_inputDataTable = model;
                result = true;
            }
            else if (role == NodeRoles::NodeParameterListModel)
            {
                NodeParamListModel* model = value.value<NodeParamListModel*>();
                m_nodeParamListModel = model;
                result = true;
            }
            else if (role == NodeRoles::OutputTableModel)
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
            if (role == NodeRoles::PythonFileName)
            {
                return this->m_nodeParamListModel->data("Python File", ParameterRoles::ValueRole).toString();
            }
            else if (role == UI::Roles::Name)
            {
                return this->m_nodeParamListModel->data("Name", ParameterRoles::ValueRole).toString();
            }
            else if (role == NodeRoles::PythonError)
            {
                return m_pythonError;
            }
            else if (role == NodeRoles::InputTableModel)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(m_inputDataTable));
                return v;
            }
            else if (role == NodeRoles::NodeParameterListModel)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(m_nodeParamListModel));
                return v;
            }
            else if (role == NodeRoles::OutputTableModel)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(m_outputDataTable));
                return v;
            }
            else if (role == NodeRoles::NodeRunningState)
            {
                return this->getState();
            }
            else
            {
                return ActorNode::data(role);
            }
        }

        NodeContextMetadata PythonProcessActorNode::createMetadata() const
        {
            NodeContextMetadata metadata;
            metadata.setName(this->data(UI::Roles::Name).toString());
            metadata.setNodeType(NodeTypes::PythonNode);
            QJsonArray array;
            {
                for(int i = 0; i < this->m_nodeParamListModel->rowCount(); i++)
                {
                    auto index = this->m_nodeParamListModel->index(i,0);
                    if(index.data(ParameterRoles::NameRole).toString() != "Name")
                    {
                        QJsonObject o;
                        o["name"] = index.data(ParameterRoles::NameRole).toString();
                        o["value"] = index.data(ParameterRoles::ValueRole).toString();
                        o["type"] = index.data(ParameterRoles::TypeRole).toInt();
                        array.append(o);
                    }

                }
            }
            metadata.add("parameters",array);
            QJsonObject input;
            {
                auto root = m_inputDataTable->getRoot();
                input["row_count"] = m_inputDataTable->rowCount();
                input["column_count"] = m_inputDataTable->columnCount();
                const auto & headerList = root->getHeaders();
                QJsonArray headers;
                for(auto& pair : headerList)
                {
                    QJsonObject h;
                    h["key"] = pair.first;
                    h["value"] = QString::fromStdString(pair.second);
                    headers.append(h);
                }
                input["header_data"] = headers;
            }
            metadata.add("input", input);


            QJsonObject output;
            {
                auto root = m_outputDataTable->getRoot();
                output["row_count"] = m_outputDataTable->rowCount();
                output["column_count"] = m_outputDataTable->columnCount();
                const auto & headerList = root->getHeaders();
                QJsonArray headers;
                for(auto& pair : headerList)
                {
                    QJsonObject h;
                    h["key"] = pair.first;
                    h["value"] = QString::fromStdString(pair.second);
                    headers.append(h);
                }
                output["header_data"] = headers;
            }
            metadata.add("output", output);
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

        void PythonProcessActorNode::saveContext(BaseDataContext *dataContext)
        {
            auto *pythonContext = dynamic_cast<PythonProcessDataContext*>(dataContext);

            if (!pythonContext)
            {
                return;
            }
            this->m_pythonError = pythonContext->getPythonError();

            if (auto *inputDialogModel = dynamic_cast<NodeTableDialogModel*>(pythonContext->getInputDataTable()))
            {
                inputDialogModel->saveData();
            }

            if (auto *nodeParamListModel = dynamic_cast<NodeParamListDialogModel*>(pythonContext->getNodeParameterListModel()))
            {
                nodeParamListModel->saveData();
            }

            if (auto *outputDialogModel = dynamic_cast<NodeTableDialogModel*>(pythonContext->getOutputDataTable()))
            {
                outputDialogModel->saveData();
            }

            notifyChanged({UI::Roles::Name, NodeRoles::PythonFileName, NodeRoles::PythonFileName, NodeRoles:: InputTableModel,
                           NodeRoles::NodeParameterListModel,
                           NodeRoles::OutputTableModel,});
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
                notifyChanged({NodeRoles::NodeRunningState});
            }
        }

        void PythonProcessActorNode::onFailed(const QVariant& result)
        {
        }

        void PythonProcessActorNode::inConnectionChanged(UI::MPort *inPort, UI::MPort* /*outPort*/)
        {
            QList<QVariant> dependentDataList;

            for (size_t i = 0; i < inPort->getConnectionCount(); i++)
            {
                auto connection = inPort->getConnection(i);
                auto *dependentNode = dynamic_cast<PythonProcessActorNode*>(connection->getOutPort()->getOwnerNode());

                if (dependentNode)
                {
                    auto outputTable = dependentNode->data(NodeRoles::OutputTableModel).value<NodeTableModel*>();

                    if (outputTable)
                    {
                        QVariant v = QVariant::fromValue<std::shared_ptr<HierarchicalTableData>>(outputTable->getRoot());
                        dependentDataList.append(v);
                    }
                }
            }

            auto inputData = this->createInputDataFromContext(dependentDataList);
            if (inputData)
            {
                this->m_inputDataTable->setRoot(inputData);
            }
        }

        void PythonProcessActorNode::outConnectionChanged(UI::MPort *outPort, UI::MPort *inPort)
        {
        }

    }
}


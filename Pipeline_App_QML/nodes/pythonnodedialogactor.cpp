#include "pythonnodedialogactor.h"
#include <QProcess>
#include <QJsonArray>
#include <models/nodetablemodel.h>
#include <models/nodeparamlistmodel.h>
#include <nodes/pythonprocessactornode.h>
#include <memory.h>
#include <vector>
Q_DECLARE_METATYPE(std::shared_ptr<Pipeline::Runtime::HierarchicalTableData>);

namespace Pipeline::Runtime
{
    void PythonNodeDialogActor::runStandalone()
    {
        ActorNode::runStandalone();
    }

    PythonNodeDialogActor::PythonNodeDialogActor()
    {
    }

    NodeContextMetadata PythonNodeDialogActor::createMetadata() const
    {
        NodeContextMetadata metadata;
        metadata.setName(this->m_inputParameterModel->data("Name", ParameterRoles::ValueRole).toString());
        metadata.setNodeType(NodeTypes::PythonNode);

        QJsonArray array;
        {
            for(int i = 0; i < this->m_inputParameterModel->rowCount(); i++)
            {
                auto index = this->m_inputParameterModel->index(i,0);
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

    QVariant PythonNodeDialogActor::behaviour(const Thread::BehaviourContext &behaviour)
    {
        m_pythonThrowError = false;
        m_pythonError = "";
        QVariant result;
        QProcess process;
        auto stringList =  QStringList() << m_inputParameterModel->data("Python File").toString();
        process.start("C:\\Users\\yerli\\AppData\\Local\\Programs\\Python\\Python39\\python.exe", stringList);

        if (!process.waitForStarted())
        {
            qDebug() << "Python is not started";
            return false;
        }

        std::vector<uint8_t> data;

        auto rootTableData = m_inputDataTable->getRoot();
        auto tableData = rootTableData->serialize();

        std::vector<uint8_t> parameter;
        m_inputParameterModel->serialize(parameter);


        // boyutları ekle
        uint32_t parameterSize = static_cast<uint32_t>(parameter.size());
        uint32_t tableSize = static_cast<uint32_t>(tableData.size());

        // data vector içine yaz
        data.insert(data.end(), reinterpret_cast<uint8_t*>(&parameterSize), reinterpret_cast<uint8_t*>(&parameterSize) + sizeof(parameterSize));
        data.insert(data.end(), parameter.begin(), parameter.end());

        data.insert(data.end(), reinterpret_cast<uint8_t*>(&tableSize), reinterpret_cast<uint8_t*>(&tableSize) + sizeof(tableSize));
        data.insert(data.end(), tableData.begin(), tableData.end());

        QByteArray buffer(
            reinterpret_cast<const char*>(data.data()),
            static_cast<int>(data.size())
        );

        process.write(buffer);
        process.closeWriteChannel();
        process.waitForFinished();
        QByteArray outputData = process.readAllStandardOutput();
        auto error = process.readAllStandardError();

        int exitCode = process.exitCode();
        auto status = process.exitStatus();

        if (status == QProcess::NormalExit && exitCode == 0)
        {
            m_pythonError += QString(!m_pythonError.isEmpty() ? "\n": "") + "Process is finished successfully.";
            emit this->pythonErrorChanged();
        }
        else
        {
            m_pythonError = error;
            m_pythonThrowError = true;
            emit this->pythonErrorChanged();
        }

        try
        {
            if(!m_pythonThrowError)
            {
                const uint8_t* outputDataU8 = reinterpret_cast<const uint8_t*>(outputData.constData());
                size_t size = static_cast<size_t>(outputData.size());
                auto* outputResult = HierarchicalTableData::deserialize(outputDataU8, size);
                result = QVariant::fromValue<std::shared_ptr<HierarchicalTableData>>(std::shared_ptr<HierarchicalTableData>(outputResult));
            }
        }
        catch (std::runtime_error &error)
        {
            m_pythonError = error.what();
            emit this->pythonErrorChanged();
        }

        return result;
    }

    void PythonNodeDialogActor::setPythonError(const QString &pythonError)
    {
        if (m_pythonError == pythonError)
        {
            return;
        }

        m_pythonError = pythonError;
        emit this->pythonErrorChanged();
    }

    NodeTableModel* PythonNodeDialogActor::getInputDataTable() const
    {
        return m_inputDataTable;
    }

    void PythonNodeDialogActor::setInputDataTable(NodeTableModel *inputData)
    {
        if (m_inputDataTable == inputData)
        {
            return;
        }

        m_inputDataTable = inputData;
        emit this->inputDataChanged();
    }

    NodeTableModel* PythonNodeDialogActor::getOutputDataTable() const
    {
        return m_outputDataTable;
    }

    void PythonNodeDialogActor::setOutputDataTable(NodeTableModel *outputData)
    {
        if (m_outputDataTable == outputData)
        {
            return;
        }

        m_outputDataTable = outputData;
        emit this->outputDataChanged();
    }

    void PythonNodeDialogActor::onStarted()
    {

    }

    void PythonNodeDialogActor::onFinished(const QVariant &result)
    {
        // this is main thread we need to set root here beacuse of UI update
        if(!m_pythonThrowError)
        {
            auto outputResult = result.value<std::shared_ptr<HierarchicalTableData>>();
            m_outputDataTable->setRoot(outputResult);
        }
    }

    void PythonNodeDialogActor::onFailed(const QVariant &result)
    {
    }

    NodeParamListModel *PythonNodeDialogActor::getNodeParameterListModel() const
    {
        return m_inputParameterModel;
    }

    void PythonNodeDialogActor::setNodeParameterListModel(NodeParamListModel *newInputParameterModel)
    {
        if (m_inputParameterModel == newInputParameterModel)
            return;
        m_inputParameterModel = newInputParameterModel;
        emit nodeParameterListModelChanged();
    }

    BaseActorNodeDispatcher *PythonNodeDialogActor::getActorAction() const
    {
        return getDispatcher();
    }

}

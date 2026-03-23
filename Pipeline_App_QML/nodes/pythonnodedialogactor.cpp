#include "pythonnodedialogactor.h"
#include <QProcess>
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

        if (!error.isEmpty() && m_pythonError != error)
        {
            m_pythonError = error;
            m_pythonThrowError = true;
            emit this->pythonErrorChanged();
        }

        if (m_pythonError.isEmpty())
        {
            m_pythonError = "Process is finished successfully";
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

}

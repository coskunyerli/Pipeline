#include "pythonnodedialogactor.h"
#include <QProcess>
#include <models/nodetablemodel.h>
#include <nodes/pythonprocessactornode.h>
#include <memory.h>
Q_DECLARE_METATYPE(std::shared_ptr<Pipeline::Runtime::HierarchicalTableData>);

namespace Pipeline::Runtime
{
    void PythonNodeDialogActor::runStandalone()
    {
        ActorNode::runStandalone();
    }

    PythonNodeDialogActor::PythonNodeDialogActor()
    {
        this->setDispatcher(new PythonDispatcher(this));
    }

    QVariant PythonNodeDialogActor::behaviour(const Thread::BehaviourContext &behaviour)
    {
        m_pythonThrowError = false;
        m_pythonError = "";
        QVariant result;
        QProcess process;
        process.start("C:\\Users\\yerli\\AppData\\Local\\Programs\\Python\\Python39\\python.exe", QStringList() << m_filename);

        if (!process.waitForStarted())
        {
            qDebug() << "Python is not started";
            return false;
        }

        auto data = m_inputDataTable->getRoot()->serialize();
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

    void PythonNodeDialogActor::setFilename(const QString &filename)
    {
        if (m_filename == filename)
        {
            return;
        }

        m_filename = filename;
        emit this->filenameChanged();
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
}

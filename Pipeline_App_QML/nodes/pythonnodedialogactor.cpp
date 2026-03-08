#include "pythonnodedialogactor.h"
#include <QProcess>
#include <models/nodetablemodel.h>
#include <nodes/pythonprocessactornode.h>

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
        result = process.readAllStandardOutput();
        m_pythonError = process.readAllStandardError();

        if (m_pythonError.isEmpty())
        {
            m_pythonError = "Process is finished successfully";
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

    void PythonNodeDialogActor::onFinished(const QVariant &result)
    {
        // this is main thread we need to set root here beacuse of UI update
        try
        {
            QByteArray output = result.toByteArray();
            const uint8_t* outputData = reinterpret_cast<const uint8_t*>(output.constData());
            size_t size = static_cast<size_t>(output.size());
            auto outputResult = PythonNodeResult::deserialize(outputData, size);
            m_outputDataTable->setRoot(QSharedPointer<PythonNodeResult>(outputResult));
        }
        catch (std::runtime_error &error)
        {
            m_pythonError = error.what();
        }
    }

    void PythonNodeDialogActor::onFailed(const QVariant &result)
    {
    }
}

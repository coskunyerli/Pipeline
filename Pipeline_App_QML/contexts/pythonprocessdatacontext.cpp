#include "pythonprocessdatacontext.h"
#include <models/nodetablemodel.h>

namespace Pipeline::Runtime
{

    PythonProcessDataContext::PythonProcessDataContext(QObject *parent)
        : BaseDataContext(parent)
    {
    }

    QString PythonProcessDataContext::getFilename() const
    {
        return m_filename;
    }

    void PythonProcessDataContext::setFilename(const QString &newFilename)
    {
        if (m_filename == newFilename)
            return;

        m_filename = newFilename;
        emit filenameChanged();
    }

    QString PythonProcessDataContext::getPythonError() const
    {
        return m_pythonError;
    }

    void PythonProcessDataContext::setPythonError(const QString &newPythonError)
    {
        if (m_pythonError == newPythonError)
            return;

        m_pythonError = newPythonError;
        emit pythonErrorChanged();
    }

    NodeTableModel* PythonProcessDataContext::getInputDataTable() const
    {
        return m_inputData;
    }

    void PythonProcessDataContext::setInputDataTable(NodeTableModel *newInputData)
    {
        if (m_inputData == newInputData)
            return;

        m_inputData = newInputData;
        emit inputDataChanged();
    }

    NodeTableModel* PythonProcessDataContext::getOutputDataTable() const
    {
        return m_outputData;
    }

    void PythonProcessDataContext::setOutputDataTable(NodeTableModel *newOutputData)
    {
        if (m_outputData == newOutputData)
            return;

        m_outputData = newOutputData;
        emit outputDataChanged();
    }

    QString PythonProcessDataContext::getName() const
    {
        return m_name;
    }

    void PythonProcessDataContext::setName(const QString &newName)
    {
        if (m_name == newName)
            return;

        m_name = newName;
        emit nameChanged();
    }

    NodeTableModel *PythonProcessDataContext::getInputParameterDataTable() const
    {
        return m_inputParameterModel;
    }

    void PythonProcessDataContext::setInputParemeterDataTable(NodeTableModel *newInputParameterModel)
    {
        if (m_inputParameterModel == newInputParameterModel)
            return;
        m_inputParameterModel = newInputParameterModel;
        emit inputParameterDataChanged();
    }

}

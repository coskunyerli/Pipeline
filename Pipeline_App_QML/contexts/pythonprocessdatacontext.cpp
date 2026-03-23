#include "pythonprocessdatacontext.h"
#include <models/nodetablemodel.h>

namespace Pipeline::Runtime
{

    PythonProcessDataContext::PythonProcessDataContext(QObject *parent)
        : BaseDataContext(parent)
    {
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

    NodeParamListModel *PythonProcessDataContext::getNodeParameterListModel() const
    {
        return m_inputParameterModel;
    }

    void PythonProcessDataContext::setNodeParameterListModel(NodeParamListModel *inputParameterModel)
    {
        if (m_inputParameterModel == inputParameterModel)
            return;
        m_inputParameterModel = inputParameterModel;
        emit nodeParameterListModelChanged();
    }

}

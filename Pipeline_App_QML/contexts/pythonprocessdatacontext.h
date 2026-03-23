#pragma once
#include <contexts/basedatacontext.h>
#include <models/nodetablemodel.h>
#include <models/nodeparamlistmodel.h>
namespace Pipeline::Runtime
{
    class PythonProcessDataContext : public BaseDataContext
    {
            Q_OBJECT
            Q_PROPERTY(QString pythonError READ getPythonError WRITE setPythonError NOTIFY pythonErrorChanged)
            Q_PROPERTY(NodeTableModel* inputModel READ getInputDataTable WRITE setInputDataTable NOTIFY inputDataChanged)
            Q_PROPERTY(NodeParamListModel* inputParameterModel READ getNodeParameterListModel WRITE setNodeParameterListModel NOTIFY nodeParameterListModelChanged)
            Q_PROPERTY(NodeTableModel* outputModel READ getOutputDataTable WRITE setOutputDataTable NOTIFY outputDataChanged)
        public:
            explicit PythonProcessDataContext(QObject *parent = nullptr);

            QString getPythonError() const;
            void setPythonError(const QString &newPythonError);

            NodeTableModel* getInputDataTable() const;
            void setInputDataTable(NodeTableModel *newInputData);

            NodeTableModel* getOutputDataTable() const;
            void setOutputDataTable(NodeTableModel *newOutputData);


            NodeParamListModel *getNodeParameterListModel() const;
            void setNodeParameterListModel(NodeParamListModel *inputParameterModel);

        signals:
            void pythonErrorChanged();
            void inputDataChanged();
            void outputDataChanged();
            void nodeParameterListModelChanged();

        private:
            QString m_pythonError;
            NodeTableModel* m_inputData = nullptr;
            NodeTableModel* m_outputData = nullptr;
            NodeParamListModel *m_inputParameterModel = nullptr;
    };
}

#pragma once
#include <contexts/basedatacontext.h>
#include <models/nodetablemodel.h>
namespace Pipeline::Runtime
{
    class PythonProcessDataContext : public BaseDataContext
    {
            Q_OBJECT
            Q_PROPERTY(QString filename READ getFilename WRITE setFilename NOTIFY filenameChanged)
            Q_PROPERTY(QString name READ getName WRITE setName NOTIFY nameChanged)
            Q_PROPERTY(QString pythonError READ getPythonError WRITE setPythonError NOTIFY pythonErrorChanged)
            Q_PROPERTY(NodeTableModel* inputModel READ getInputDataTable WRITE setInputDataTable NOTIFY inputDataChanged)
            Q_PROPERTY(NodeTableModel* outputModel READ getOutputDataTable WRITE setOutputDataTable NOTIFY outputDataChanged)
        public:
            explicit PythonProcessDataContext(QObject *parent = nullptr);
            QString getFilename() const;
            void setFilename(const QString &newFilename);
            QString getPythonError() const;
            void setPythonError(const QString &newPythonError);

            NodeTableModel* getInputDataTable() const;
            void setInputDataTable(NodeTableModel *newInputData);

            NodeTableModel* getOutputDataTable() const;
            void setOutputDataTable(NodeTableModel *newOutputData);

            QString getName() const;
            void setName(const QString &newName);

        signals:
            void filenameChanged();
            void pythonErrorChanged();
            void inputDataChanged();
            void outputDataChanged();

            void nameChanged();

        private:
            QString m_filename;
            QString m_pythonError;
            NodeTableModel* m_inputData = nullptr;
            NodeTableModel* m_outputData = nullptr;
            QString m_name;
    };
}

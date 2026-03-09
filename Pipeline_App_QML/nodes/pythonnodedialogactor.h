#pragma once
#include <nodes/actornode.h>
namespace Pipeline::Runtime
{
    class NodeTableModel;
    class PythonNodeDialogActor: public QObject, public ActorNode
    {
            Q_OBJECT
            Q_PROPERTY(QString filename READ getFilename WRITE setFilename NOTIFY filenameChanged)
            Q_PROPERTY(QString pythonError READ getPythonError WRITE setPythonError NOTIFY pythonErrorChanged)
            Q_PROPERTY(NodeTableModel* inputData READ getInputDataTable WRITE setInputDataTable NOTIFY inputDataChanged)
            Q_PROPERTY(NodeTableModel* outputData READ getOutputDataTable WRITE setOutputDataTable NOTIFY outputDataChanged)
        public:
            Q_INVOKABLE void runStandalone();

        public:
            explicit PythonNodeDialogActor();
            void setRefenceActor(ActorNode*referenceActor)
            {
                this->m_referenceActor = referenceActor;
            }
            QVariant behaviour(const Thread::BehaviourContext& behaviour) override;
            ActorNode* getReferenceActor()const
            {
                return m_referenceActor;
            }
            QString getFilename() const
            {
                return m_filename;
            }
            void setFilename(const QString&filename);
            QString getPythonError() const
            {
                return m_pythonError;
            }
            void setPythonError(const QString&pythonError);
            NodeTableModel* getInputDataTable() const;
            void setInputDataTable(NodeTableModel* inputData);
            NodeTableModel* getOutputDataTable() const;
            void setOutputDataTable(NodeTableModel* outputData);
            void onFinished(const QVariant& result) override;
            void onFailed(const QVariant& result) override;
        signals:
            void filenameChanged();
            void pythonErrorChanged();
            void inputDataChanged();
            void outputDataChanged();
        private:
            ActorNode* m_referenceActor;
            QString m_filename;
            NodeTableModel* m_inputDataTable;
            NodeTableModel* m_outputDataTable;
            QString m_pythonError;
            bool m_pythonThrowError;

    };
}

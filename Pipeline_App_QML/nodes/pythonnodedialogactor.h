#pragma once
#include <nodes/actornode.h>
#include <models/nodeparamlistmodel.h>
#include <data/contextmetadata.h>
namespace Pipeline::Runtime
{
    class NodeTableModel;
    class PythonNodeDialogActor: public QObject, public ActorNode
    {
            Q_OBJECT
            Q_PROPERTY(QString pythonError READ getPythonError WRITE setPythonError NOTIFY pythonErrorChanged)
            Q_PROPERTY(NodeTableModel* inputData READ getInputDataTable WRITE setInputDataTable NOTIFY inputDataChanged)
            Q_PROPERTY(NodeParamListModel* inputParameterModel READ getNodeParameterListModel WRITE setNodeParameterListModel NOTIFY nodeParameterListModelChanged)
            Q_PROPERTY(NodeTableModel* outputData READ getOutputDataTable WRITE setOutputDataTable NOTIFY outputDataChanged)
            Q_PROPERTY(BaseActorNodeDispatcher* actorAction READ getActorAction)
        public:
            Q_INVOKABLE void runStandalone() override;

        public:
            explicit PythonNodeDialogActor();
            NodeContextMetadata createMetadata() const override;
            void setRefenceActor(ActorNode*referenceActor)
            {
                this->m_referenceActor = referenceActor;
            }
            QVariant behaviour(const Thread::BehaviourContext& behaviour) override;
            ActorNode* getReferenceActor()const
            {
                return m_referenceActor;
            }
            QString getPythonError() const
            {
                return m_pythonError;
            }
            void setPythonError(const QString&pythonError);
            NodeTableModel* getInputDataTable() const;
            void setInputDataTable(NodeTableModel* inputData);
            NodeTableModel* getOutputDataTable() const;
            void setOutputDataTable(NodeTableModel* outputData);
            void onStarted() override;
            void onFinished(const QVariant& result) override;
            void onFailed(const QVariant& result) override;
            virtual BaseDataContext* createDataContext(QObject *parent = nullptr) override
            {
                return nullptr;
            }
            virtual void saveContext(BaseDataContext* dataContext) override {}

            NodeParamListModel* getNodeParameterListModel() const;
            void setNodeParameterListModel(NodeParamListModel *newInputParameterModel);

            BaseActorNodeDispatcher *getActorAction() const;

        signals:
            void pythonErrorChanged();
            void inputDataChanged();
            void outputDataChanged();
            void nodeParameterListModelChanged();

        private:
            ActorNode* m_referenceActor;
            NodeParamListModel* m_inputParameterModel = nullptr;
            NodeTableModel* m_inputDataTable = nullptr;
            NodeTableModel* m_outputDataTable = nullptr;
            QString m_pythonError;
            bool m_pythonThrowError;
    };
}

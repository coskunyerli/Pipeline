#pragma once
#include "actornode.h"
#include <models/nodetablemodel.h>
#include <QObject>

class QSharedMemory;
namespace Pipeline
{
    namespace Runtime
    {
        class BaseDataContext;
        class PythonProcessActorNode : public ActorNode
        {
            public:
                explicit PythonProcessActorNode();
                ~PythonProcessActorNode() override;
                void setFilename(const QString &filename);
                QVariant behaviour(const Thread::BehaviourContext& behaviour) override;
                QHash<int, QByteArray> roleNames() const override;
                bool setData(const QVariant &value, int role, bool emitSignal = false) override;
                QVariant data(int role) const override;
                BaseDataContext* createDataContext(QObject*parent=nullptr) override;
                void saveContext(BaseDataContext* dataContext) override;
                void onStarted() override;
                void onFinished(const QVariant& result) override;
                void onFailed(const QVariant& result) override;
                void inConnectionChanged(UI::MPort *inPort, UI::MPort* outPort) override;
                void outConnectionChanged(UI::MPort *outPort, UI::MPort* inPort) override;
            private:
                QString m_filename;
                NodeTableModel* m_inputDataTable;
                NodeTableModel* m_outputDataTable;
                QString m_pythonError;
                bool m_pythonThrowError;
                bool m_useInputTable;
        };
    }
}

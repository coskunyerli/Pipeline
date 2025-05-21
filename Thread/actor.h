#pragma once
#include <QObject>
#include <QSemaphore>
#include <QVector>
#include "behaviour.h"
#include <QThreadPool>
namespace Pipeline
{
    class Actor : public QObject
    {
            Q_OBJECT
        public:
            friend class ActorRunnable;
            enum ProgressState
            {
                READY,
                PROGRASSING,
                STOPPED,
                FINISHED,
                PAUSED,
                FAILED
            };
            explicit Actor(QThreadPool *pool, QObject *parent = nullptr);
            explicit Actor(QThreadPool *pool, int priority, QObject *parent = nullptr);
            explicit Actor(QObject *parent = nullptr);
            void setThreadPool(QThreadPool *pool)
            {
                m_threadPool = pool;
            }
            void addNextActor(Actor* other);
            void removeNextActor(Actor* other);
            Behaviour getBehaviour()
            {
                return m_behaviour;
            }
            QVariant getValue() const
            {
                return m_value;
            }
            void setPriority(int priority)
            {
                this->m_priority = priority;
            }
            int getPriority() const
            {
                return m_priority;
            }
            ProgressState getState() const
            {
                return this->m_state;
            }
            BehaviourContext getContext() const;
            void setBehaviour(const Behaviour& behaviour)
            {
                m_behaviour = behaviour;
            }
            void startRequest();
            void reset();
        signals:
            void finished(QVariant);
            void failed(QVariant);
            void started();
        private:
            void setDependentActor(Actor* other);
            void removeDependentActor(Actor* other);
            void onDependentFinished(QVariant value);
        private:
            QSemaphore m_semaphore;
            Behaviour m_behaviour;
            QVector<Actor*> m_dependentActors;
            QVector<Actor*> m_nextActors;
            QVariant m_value;
            QThreadPool* m_threadPool;
            ProgressState m_state;
            int m_priority;
    };
}

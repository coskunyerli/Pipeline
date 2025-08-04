#pragma once
#include <QVariant>
namespace Pipeline
{
    namespace Thread
    {
        class BehaviourContext
        {
            public:
                BehaviourContext();
            public:
                QVariantList m_variants;
                bool m_hasDependentActor;
        };

        class Behaviour
        {
            public:
                Behaviour();
                Behaviour(std::function<QVariant(BehaviourContext)> func);
                virtual QVariant run(const BehaviourContext& context);
            private:
                std::function<QVariant(BehaviourContext)> m_func;
        };
    }
}

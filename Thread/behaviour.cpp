#include "behaviour.h"

namespace Pipeline
{
    namespace Thread
    {
        Behaviour::Behaviour()
        {
        }

        Behaviour::Behaviour(std::function<QVariant(BehaviourContext)> func)
            : m_func(func)
        {
        }

        QVariant Behaviour::run(const BehaviourContext& context)
        {
            if (m_func)
            {
                auto value = m_func(context);
                return value;
            }

            return QVariant();
        }

        BehaviourContext::BehaviourContext()
            : m_variants()
            , m_hasDependentActor(false)
        {
        }

    }
}

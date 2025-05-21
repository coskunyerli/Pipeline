#pragma once
#include <QVariant>
namespace Pipeline
{

    class BehaviourContext
    {
        public:
            QVariantList m_variants;
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

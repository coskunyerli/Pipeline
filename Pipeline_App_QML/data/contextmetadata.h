#pragma once
#include <QHash>
#include <QVariantMap>
#include <QJsonObject>

namespace Pipeline::Runtime
{
    class NodeContextMetadata
    {
        public:
            explicit NodeContextMetadata() = default;
            void add(QString key, const QVariant &value)
            {
                m_properties[key] = value;
            }

            void setName(const QString &name)
            {
                m_name = name;
            }

            QString getName() const
            {
                return m_name;
            }

            void setNodeType(int type)
            {
                m_nodeType = type;
            }

            int getNodeType() const
            {
                return m_nodeType;
            }

            QStringList keys() const
            {
                return m_properties.keys();
            }

            QString serialize() const;
            void deserialize(const QString& jsonString);
        private:
            QVariantMap m_properties;
            QString m_name;
            int m_nodeType;
    };


}

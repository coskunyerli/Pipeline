#include "contextmetadata.h"
#include <QJsonDocument>

namespace Pipeline::Runtime
{


    QString NodeContextMetadata::serialize() const
    {
        QJsonObject obj;
        obj["name"] = m_name;
        obj["type"] = m_nodeType;
        auto jsonObject = QJsonObject::fromVariantMap(m_properties);
        obj["properties"] = jsonObject;
        QJsonDocument doc(obj);
        return doc.toJson();
    }

    void NodeContextMetadata::deserialize(const QString &jsonString)
    {
        QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8());
        auto obj = doc.object();
        auto name = obj.take("name");
        auto type = obj.take("type");
        this->setName(name.isString() ? name.toString() : "");
        this->setNodeType(type.isDouble() ? type.toInt() : -1);
        auto prop = obj.take("properties");
        m_properties = prop.isObject() ? prop.toObject().toVariantMap() : QVariantMap();
    }

}

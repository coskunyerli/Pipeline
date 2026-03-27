#include "nodemodelservice.h"
#include <services/appdataservice.h>
#include <models/nodemodel.h>
namespace Pipeline::Runtime
{
    // // TODOJ burada nodelar json tutulacak
    // NodeModelService* NodeModelService::instance(QQmlEngine*, QJSEngine*)
    // {
    //     static NodeModelService service;
    //     return &service;
    // }

    NodeModelService::NodeModelService(QObject* parent)
        : QObject(parent)
    {
        m_model = new NodeModel(this);
        loadAllNodes();
    }

    QAbstractItemModel* NodeModelService::model() const
    {
        return m_model;
    }

    bool NodeModelService::saveNode(const QString& description, const NodeContextMetadata& metadata)
    {
        QString dir = AppDataService::instance()->templatesDir();
        QString name = metadata.getName();
        QString path = dir + "/" + name + ".node";
        QFile file(path);

        if (!file.open(QIODevice::WriteOnly))
            return false;

        QString data = metadata.serialize();
        file.write(data.toUtf8());
        file.close();
        m_model->addNode(description, metadata);
        return true;
    }

    NodeContextMetadata NodeModelService::loadNode(const QString& name)
    {
        QString dir = AppDataService::instance()->templatesDir();
        QString path = dir + "/" + name + ".node";
        QFile file(path);

        NodeContextMetadata metadata;
        if (!file.open(QIODevice::ReadOnly))
            return metadata;

        metadata.deserialize(file.readAll());
        return metadata;
    }

    QStringList NodeModelService::listNodes()
    {
        QString dir = AppDataService::instance()->templatesDir();
        QDir directory(dir);
        QStringList files = directory.entryList(
                                QStringList() << "*.node",
                                QDir::Files
                            );
        QStringList result;

        for (const QString& f : files)
            result << QFileInfo(f).baseName();

        return result;
    }

    void NodeModelService::loadAllNodes()
    {
        QStringList nodes = listNodes();

        for (const QString& name : nodes)
        {
            NodeContextMetadata data = loadNode(name);
            m_model->addNode(name, data);
        }
    }
}

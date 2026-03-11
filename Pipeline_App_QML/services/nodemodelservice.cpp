#include "nodemodelservice.h"
#include <services/appdataservice.h>
#include <models/nodemodel.h>

namespace Pipeline::Runtime
{
    // TODOJ burada nodelar json tutulacak
    NodeModelService* NodeModelService::instance(QQmlEngine*, QJSEngine*)
    {
        static NodeModelService service;
        return &service;
    }

    NodeModelService::NodeModelService(QObject* parent)
        : QObject(parent)
    {
        m_model = new NodeModel(this);
        loadAllNodes();
    }

    QObject* NodeModelService::model() const
    {
        return m_model;
    }

    bool NodeModelService::saveNode(const QString& name, const QByteArray& data)
    {
        QString dir = AppDataService::instance()->templatesDir();
        QString path = dir + "/" + name + ".node";
        QFile file(path);

        if (!file.open(QIODevice::WriteOnly))
            return false;

        file.write(data);
        file.close();
        // TODOJ m_model->addNode(name, data);
        return true;
    }

    QByteArray NodeModelService::loadNode(const QString& name)
    {
        QString dir = AppDataService::instance()->templatesDir();
        QString path = dir + "/" + name + ".node";
        QFile file(path);

        if (!file.open(QIODevice::ReadOnly))
            return {};

        return file.readAll();
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
            QByteArray data = loadNode(name);
            // TODOJ m_model->addNode(name, data);
        }
    }
}

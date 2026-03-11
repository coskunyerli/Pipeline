#include "appdataservice.h"
namespace Pipeline::Runtime
{

    AppDataService* AppDataService::instance(QQmlEngine*, QJSEngine*)
    {
        static AppDataService inst;
        return &inst;
    }

    QString AppDataService::templatesDir()
    {
        if (_templatesDir.isEmpty())
        {
            QString baseDir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            _templatesDir = baseDir + "/templates";
            QDir().mkpath(_templatesDir);
        }

        return _templatesDir;
    }

    bool AppDataService::saveTemplate(const QString &name, const QByteArray &data)
    {
        QString filePath = templatesDir() + "/" + name;
        QFile file(filePath);

        if (!file.open(QIODevice::WriteOnly)) return false;

        file.write(data);
        return true;
    }

    QByteArray AppDataService::loadTemplate(const QString &name)
    {
        QString filePath = templatesDir() + "/" + name;
        QFile file(filePath);

        if (!file.open(QIODevice::ReadOnly)) return {};

        return file.readAll();
    }

    bool AppDataService::removeTemplate(const QString &name)
    {
        QString filePath = templatesDir() + "/" + name;
        return QFile::remove(filePath);
    }
}

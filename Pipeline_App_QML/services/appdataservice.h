#pragma once
#include <QObject>
#include <QString>
#include <QByteArray>
#include <QDir>
#include <QStandardPaths>
#include <QFile>
#include <QQmlEngine>

namespace Pipeline::Runtime
{

    class AppDataService : public QObject
    {
            Q_OBJECT
        public:
            static AppDataService* instance(QQmlEngine*engine = nullptr, QJSEngine* jsEngine=nullptr);
            Q_INVOKABLE QString templatesDir();
            Q_INVOKABLE bool saveTemplate(const QString& name, const QByteArray& data);
            Q_INVOKABLE QByteArray loadTemplate(const QString& name);
            Q_INVOKABLE bool removeTemplate(const QString& name);

        private:
            explicit AppDataService(QObject* parent = nullptr) : QObject(parent) {}
            QString _templatesDir;
    };
}


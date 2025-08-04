#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <Models/nodegraphtreemodel.h>
#include <Models/nodegraphmodel.h>
#include <Models/nodegraphviewmodel.h>
#include <Models/connectiongraphmodel.h>
#include <Models/connectiongraphviewmodel.h>
#include <Models/nodeuimanager.h>
#include "graphmodelservice.h"
#include "Models/portgraphviewmodel.h"

int main(int argc, char* argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    qmlRegisterType<Pipeline::UI::NodeGraphTreeModel>("Pipeline.Models", 1, 0, "NodeGraphTreeModel");
    qmlRegisterType<Pipeline::UI::NodeGraphModel>("Pipeline.Models", 1, 0, "NodeGraphModel");
    qmlRegisterType<Pipeline::UI::NodeGraphViewModel>("Pipeline.Models", 1, 0, "NodeGraphViewModel");
    qmlRegisterType<Pipeline::UI::ConnectionGraphModel>("Pipeline.Models", 1, 0, "ConnectionGraphModel");
    qmlRegisterType<Pipeline::UI::ConnectionGraphViewModel>("Pipeline.Models", 1, 0, "ConnectionGraphViewModel");
    qmlRegisterType<Pipeline::UI::PortGraphViewModel>("Pipeline.Models", 1, 0, "PortGraphViewModel");
    qmlRegisterType<Pipeline::Runtime::GraphModelService>("Pipeline.Services", 1, 0, "GraphModelService");
    qmlRegisterType<Pipeline::UI::NodeIUManager>("Pipeline.Managers", 1, 0, "NodeUIManager");
    qRegisterMetaType<Pipeline::UI::NodeGraphTreeModel*>("UI::NodeGraphTreeModel*");
    qRegisterMetaType<Pipeline::UI::ConnectionGraphModel*>("UI::ConnectionGraphModel*");
    //qmlRegisterSingletonType<Pipeline::Runtime::GraphModelService>("Pipeline.Services", 1, 0, "GraphModelService", Pipeline::Runtime::GraphModelService::createSingletonInstance);
    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject * obj, const QUrl & objUrl)
    {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    },
    Qt::QueuedConnection);
    engine.load(url);
    return app.exec();
}

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <Models/nodegraphtreemodel.h>
#include <Models/nodegraphmodel.h>
#include <Models/nodegraphviewmodel.h>
#include <Models/connectiongraphmodel.h>
#include <Models/connectiongraphviewmodel.h>
#include <Models/nodeuimanager.h>
#include <services/graphmodelservice.h>
#include <services/appdataservice.h>
#include <services/nodemodelservice.h>
#include <models/nodetabledialogmodel.h>
#include <models/nodemodel.h>
#include <models/nodetablesliceproxymodel.h>
#include <models/nodeparamlistdialogmodel.h>
#include <models/nodeparamuilistmodel.h>
#include <Models/portgraphviewmodel.h>
#include <nodes/pythonnodedialogactor.h>
#include <nodes/pythonprocessactornode.h>
#include <contexts/pythonprocessdatacontext.h>
#include <data/contextmetadata.h>
#include <QQuickStyle>

int main(int argc, char* argv[])
{
    QCoreApplication::setOrganizationName("Erythra Dynamics");
    QCoreApplication::setApplicationName("Pipeline");
    QQuickStyle::setStyle("Fusion");
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;
    qmlRegisterType<Pipeline::UI::NodeGraphTreeModel>("Pipeline.Models", 1, 0, "NodeGraphTreeModel");
    qmlRegisterType<Pipeline::Runtime::NodeModel>("Pipeline.Models", 1, 0, "NodeModel");
    qmlRegisterType<Pipeline::Runtime::NodeTableSliceProxyModel>("Pipeline.Models", 1, 0, "NodeTableSliceProxyModel");
    qmlRegisterType<Pipeline::UI::NodeGraphModel>("Pipeline.Models", 1, 0, "NodeGraphModel");
    qmlRegisterType<Pipeline::UI::NodeGraphViewModel>("Pipeline.Models", 1, 0, "NodeGraphViewModel");
    qmlRegisterType<Pipeline::UI::ConnectionGraphModel>("Pipeline.Models", 1, 0, "ConnectionGraphModel");
    qmlRegisterType<Pipeline::UI::ConnectionGraphViewModel>("Pipeline.Models", 1, 0, "ConnectionGraphViewModel");
    qmlRegisterType<Pipeline::UI::PortGraphViewModel>("Pipeline.Models", 1, 0, "PortGraphViewModel");
    qmlRegisterType<Pipeline::Runtime::GraphModelService>("Pipeline.Services", 1, 0, "GraphModelService");
    qmlRegisterType<Pipeline::Runtime::NodeModelService>("Pipeline.Services", 1, 0, "NodeModelService");
    qmlRegisterType<Pipeline::Runtime::NodeTableDialogModel>("Pipeline.Models", 1, 0, "NodeTableDialogModel");
    qmlRegisterType<Pipeline::Runtime::NodeParamListDialogModel>("Pipeline.Models", 1, 0, "NodeParamListDialogModel");
    qmlRegisterType<Pipeline::Runtime::NodeParamUIListModel>("Pipeline.Models", 1, 0, "NodeParamUIListModel");
    qmlRegisterType<Pipeline::Runtime::PythonNodeDialogActor>("Pipeline.Actors", 1, 0, "PythonNodeDialogActor");
    qmlRegisterType<Pipeline::Runtime::PythonProcessDataContext>("Pipeline.Contexts", 1, 0, "PythonProcessDataContext");
    qmlRegisterType<Pipeline::UI::NodeIUManager>("Pipeline.Managers", 1, 0, "NodeUIManager");
    qRegisterMetaType<Pipeline::UI::NodeGraphTreeModel*>("UI::NodeGraphTreeModel*");
    qRegisterMetaType<Pipeline::UI::ConnectionGraphModel*>("UI::ConnectionGraphModel*");
    qRegisterMetaType<Pipeline::Runtime::NodeContextMetadata>("ContextMetadata");
    // reach from qml but not created form qml
    qmlRegisterUncreatableType<Pipeline::Runtime::NodeTableModel>("Pipeline.Models", 1, 0, "NodeTableModel", "NodeTableModel cannot be created from QML");
    qmlRegisterUncreatableType<Pipeline::Runtime::PythonProcessActorNode>("Pipeline.Actors", 1, 0, "PythonProcessActorNode", "PythonProcessActorNode cannot be created from QML");
    // qmlRegisterSingletonType<Pipeline::Runtime::AppDataService>(
    //     "Pipeline.Services", 1, 0, "AppDataService",
    //     &Pipeline::Runtime::AppDataService::instance
    // );
    // qmlRegisterSingletonType<Pipeline::Runtime::NodeModelService>(
    //     "Pipeline.Services",
    //     1,
    //     0,
    //     "NodeModelService",
    //     &Pipeline::Runtime::NodeModelService::instance
    // );
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


Q_DECLARE_METATYPE(Pipeline::Runtime::NodeContextMetadata)
Q_DECLARE_METATYPE(QSharedPointer<Pipeline::Runtime::HierarchicalTableData>);

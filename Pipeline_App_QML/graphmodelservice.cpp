#include "graphmodelservice.h"
#include "nodes/pythonprocessactornode.h""
#include "nodes/pythonscriptactornode.h"
#include "Models/constants.h"
#include <QFile>

namespace Pipeline
{
    namespace Runtime
    {
        bool GraphModelService::addNode(const QPointF & pos)
        {
            if (!m_model)
            {
                return false;
            }

            auto *node = new PythonProcessActorNode();
            node->setFilename("C:\\Users\\yerli\\PycharmProjects\\opengl\\cppTest.py");
            auto *outPort = new UI::MPort(node);
            auto *inPort = new UI::MPort(node);
            //auto *inPort2 = new UI::MPort(node);
            //auto *outPort2 = new UI::MPort(node);
            // auto *outPort3 = new UI::MPort(node);
            node->addPort(inPort, true);
            //node->addPort(inPort2, true);
            //node->addPort(outPort2, false);
            // node->addPort(outPort3, false);
            node->addPort(outPort, false);
            node->setX(pos.x());
            node->setY(pos.y());
            return m_model->addNode(node);
        }

        void GraphModelService::start()
        {
            if (!m_model)
            {
                return;
            }

            auto* actorNode = m_model->getData<ActorNode>(m_model->index(0, 0));

            if (actorNode)
            {
                actorNode->run();
            }
        }

        UI::NodeGraphTreeModel* GraphModelService::model() const
        {
            return m_model;
        }

        void GraphModelService::setModel(UI::NodeGraphTreeModel * model)
        {
            if (model == m_model)
            {
                return;
            }

            if (m_model)
            {
                disconnect(m_model, &QAbstractItemModel::rowsInserted, this, &GraphModelService::onRowsInserted);
                // disconnect(oldModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ConnectionGraphViewModel::onRowsAboutToBeRemoved);
                // disconnect(oldModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &ConnectionGraphViewModel::onRowsAboutToBeInserted);
                // disconnect(oldModel, &QAbstractItemModel::rowsInserted, this, &ConnectionGraphViewModel::onRowsInserted);
                // disconnect(oldModel, &QAbstractItemModel::rowsRemoved, this, &ConnectionGraphViewModel::onRowsRemoved);
                // disconnect(oldModel, &QAbstractItemModel::modelAboutToBeReset, this, &ConnectionGraphViewModel::onModelAboutToBeReset);
                // disconnect(oldModel, &QAbstractItemModel::modelReset, this, &ConnectionGraphViewModel::onModelReset);
            }

            m_model = model;

            if (m_model)
            {
                m_model->registerNodeType<PythonScriptActorNode>();
                m_model->registerNodeType<PythonProcessActorNode>();
                connect(m_model, &QAbstractItemModel::rowsInserted, this, &GraphModelService::onRowsInserted);
                // connect(sourceModel, &QAbstractItemModel::rowsAboutToBeRemoved, this, &ConnectionGraphViewModel::onRowsAboutToBeRemoved);
                // connect(sourceModel, &QAbstractItemModel::rowsAboutToBeInserted, this, &ConnectionGraphViewModel::onRowsAboutToBeInserted);
                // connect(sourceModel, &QAbstractItemModel::rowsInserted, this, &ConnectionGraphViewModel::onRowsInserted);
                // connect(sourceModel, &QAbstractItemModel::rowsRemoved, this, &ConnectionGraphViewModel::onRowsRemoved);
                // connect(sourceModel, &QAbstractItemModel::modelAboutToBeReset, this, &ConnectionGraphViewModel::onModelAboutToBeReset);
                // connect(sourceModel, &QAbstractItemModel::modelReset, this, &ConnectionGraphViewModel::onModelReset);
            }

            emit modelChanged();
        }

        void GraphModelService::onRowsInserted(const QModelIndex & parent, int first, int last)
        {
            auto index = m_model->index(first, 0, parent);

            switch (index.data(UI::Roles::Type).toInt())
            {
                case UI::DataType::Connection:
                    {
                        auto* connection = m_model->getData<UI::MConnection>(index);

                        if (connection && connection->getOutPort() && connection->getInPort())
                        {
                            auto outNode = dynamic_cast<ActorNode*>(connection->getOutPort()->getOwnerNode());
                            auto inNode = dynamic_cast<ActorNode*>(connection->getInPort()->getOwnerNode());
                            outNode->addNextNode(inNode);
                        }

                        break;
                    }

                default:
                    break;
            }
        }

        GraphModelService::GraphModelService(QObject * parent)
            : QObject(parent)
            , m_model(nullptr)
        {
        }

        GraphModelService::~GraphModelService()
        {
        }
    }
}


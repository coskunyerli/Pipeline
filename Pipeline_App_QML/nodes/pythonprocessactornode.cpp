#include "pythonprocessactornode.h"
#include <QSharedMemory>
#include <QProcess>
#include <QUuid>
#include <QCoreApplication>
#include <constants.h>

Q_DECLARE_METATYPE(QSharedPointer<Pipeline::Runtime::HierarchicalTableData>);


namespace Pipeline
{
    namespace Runtime
    {
        PythonProcessActorNode::PythonProcessActorNode()
            : ActorNode()
            , m_inputDataTable(new NodeTableModel())
            , m_outputDataTable(new NodeTableModel())
            , m_useInputTable(false)
        {
            this->setDispatcher(new PythonDispatcher(this));
        }

        PythonProcessActorNode::~PythonProcessActorNode()
        {
        }

        void PythonProcessActorNode::setFilename(const QString &filename)
        {
            m_filename = filename;
        }

        QVariant PythonProcessActorNode::behaviour(const Thread::BehaviourContext &behaviour)
        {
            auto data = m_inputDataTable->getRoot()->serialize();
            QByteArray buffer(
                reinterpret_cast<const char*>(data.data()),
                static_cast<int>(data.size())
            );
            m_pythonThrowError = false;
            QVariant result;
            QProcess process;
            process.start("C:\\Users\\yerli\\AppData\\Local\\Programs\\Python\\Python39\\python.exe", QStringList() << m_filename);

            if (!process.waitForStarted())
            {
                qDebug() << "Python is not started";
                return false;
            }

            process.write(buffer);
            process.closeWriteChannel();
            process.waitForFinished();
            auto error = process.readAllStandardError();

            if (!error.isEmpty() && m_pythonError != error)
            {
                m_pythonError = error;
                m_pythonThrowError = true;
            }

            QByteArray outputData = process.readAllStandardOutput();

            if (m_pythonError.isEmpty())
            {
                m_pythonError = "Process is finished successfully";
            }

            try
            {
                if (!m_pythonThrowError)
                {
                    const uint8_t* outputDataU8 = reinterpret_cast<const uint8_t*>(outputData.constData());
                    size_t size = static_cast<size_t>(outputData.size());
                    auto* outputResult = HierarchicalTableData::deserialize(outputDataU8, size);
                    result = QVariant::fromValue<std::shared_ptr<HierarchicalTableData>>(std::shared_ptr<HierarchicalTableData>(outputResult));
                    return result;
                }
            }
            catch (std::runtime_error &error)
            {
                m_pythonError = error.what();
            }

            return result;
        }

        QHash<int, QByteArray> PythonProcessActorNode::roleNames() const
        {
            auto roles = ActorNode::roleNames();
            roles[NodeRoles::PythonFileName] = "pythonFilename";
            roles[NodeRoles::InputTableModel] = "inputTableModels";
            roles[NodeRoles::OutputTableModel] = "outputTableModel";
            roles[NodeRoles::PythonError] = "pythonError";
            roles[NodeRoles::ActorAction] = "actorAction";
            roles[NodeRoles::NodeRunningState] = "runningState";
            return roles;
        }

        bool PythonProcessActorNode::setData(const QVariant &value, int role)
        {
            if (role == NodeRoles::PythonFileName)
            {
                this->setFilename(value.toString());
                return true;
            }
            else if (role == NodeRoles::PythonError)
            {
                this->m_pythonError = value.toString();
                return true;
            }
            else if (role == NodeRoles::InputTableModel)
            {
                NodeTableModel* model = value.value<NodeTableModel*>();
                m_inputDataTable = model;
                return true;
            }
            else if (role == NodeRoles::OutputTableModel)
            {
                NodeTableModel* model = value.value<NodeTableModel*>();
                m_outputDataTable = model;
                return true;
            }
            else
            {
                return ActorNode::setData(value, role);
            }
        }

        QVariant PythonProcessActorNode::data(int role) const
        {
            if (role == NodeRoles::PythonFileName)
            {
                return m_filename;
            }
            else if (role == NodeRoles::PythonError)
            {
                return m_pythonError;
            }
            else if (role == NodeRoles::ActorAction)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(this->getDispatcher()));
                return v;
            }
            else if (role == NodeRoles::InputTableModel)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(m_inputDataTable));
                return v;
            }
            else if (role == NodeRoles::OutputTableModel)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(m_outputDataTable));
                return v;
            }
            else if(role == NodeRoles::NodeRunningState)
            {
                return this->getState();
            }
            else
            {
                return ActorNode::data(role);
            }
        }

        void PythonProcessActorNode::onStarted()
        {
            if(!m_useInputTable)
            {
                auto behaviourContext = this->getContext();
                if (!behaviourContext.m_variants.isEmpty())
                {
                    if (behaviourContext.m_variants[0].canConvert<std::shared_ptr<HierarchicalTableData>>())
                    {
                        auto inputData = behaviourContext.m_variants[0].value<std::shared_ptr<HierarchicalTableData>>();

                        if (inputData)
                        {
                            this->m_inputDataTable->setRoot(inputData);
                        }
                    }
                }
            }

        }

        void PythonProcessActorNode::onFinished(const QVariant& result)
        {
            // this is main thread we need to set root here beacuse of UI update
            if (!m_pythonThrowError)
            {
                auto outputResult = result.value<std::shared_ptr<HierarchicalTableData>>();
                m_outputDataTable->setRoot(outputResult);
            }
        }

        void PythonProcessActorNode::onFailed(const QVariant& result)
        {
        }

        PythonDispatcher::PythonDispatcher(ActorNode *node)
            : QObject()
            , m_pythonNode(node)
        {
        }

        void PythonDispatcher::runStandalone()
        {
            m_pythonNode->runStandalone();
        }

    }
}


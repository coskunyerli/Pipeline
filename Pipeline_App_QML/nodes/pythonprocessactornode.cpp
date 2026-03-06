#include "pythonprocessactornode.h"
#include <QSharedMemory>
#include <QProcess>
#include <QUuid>
#include <QCoreApplication>
#include <constants.h>

struct Row
{
    int id;
    float value;
    QString name;
};

namespace Pipeline
{
    namespace Runtime
    {
        PythonProcessActorNode::PythonProcessActorNode()
            : ActorNode()
            , m_inputDataTable(new NodeTableModel())
            , m_outputDataTable(new NodeTableModel())
        {
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
            QVariant result;
            QProcess process;
            process.start("C:\\Users\\yerli\\AppData\\Local\\Programs\\Python\\Python39\\python.exe", QStringList() << m_filename);

            if (!process.waitForStarted())
            {
                qDebug() << "Python başlatılamadı";
                return false;
            }

            auto data = m_inputDataTable->getRoot()->serialize();
            QByteArray buffer(
                reinterpret_cast<const char*>(data.data()),
                static_cast<int>(data.size())
            );
            process.write(buffer);
            process.closeWriteChannel();
            process.waitForFinished();

            result = process.readAllStandardOutput();
            m_pythonError = process.readAllStandardError();

            return result;
        }

        QHash<int, QByteArray> PythonProcessActorNode::roleNames() const
        {
            auto roles = ActorNode::roleNames();
            roles[NodeRoles::PythonFileName] = "pythonFilename";
            roles[NodeRoles::InputTableModel] = "inputTableModel";
            roles[NodeRoles::OutputTableModel] = "outputTableModel";
            roles[NodeRoles::PythonError] = "pythonError";
            return roles;
        }

        bool PythonProcessActorNode::setData(const QVariant &value, int role)
        {
            if (role == NodeRoles::PythonFileName)
            {
                this->setFilename(value.toString());
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
            else
            {
                return ActorNode::data(role);
            }
        }

        void PythonProcessActorNode::onFinished(const QVariant& result)
        {
            // this is main thread we need to set root here beacuse of UI update
            try
            {
                QByteArray output = result.toByteArray();
                const uint8_t* outputData = reinterpret_cast<const uint8_t*>(output.constData());
                size_t size = static_cast<size_t>(output.size());

                auto outputResult = PythonNodeResult::deserialize(outputData, size);
                m_outputDataTable->setRoot(outputResult);
            }
            catch (std::runtime_error &error)
            {
                m_pythonError = error.what();
            }

        }

        void PythonProcessActorNode::onFailed(const QVariant& result)
        {

        }
    }
}


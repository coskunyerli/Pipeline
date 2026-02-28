#include "pythonprocessactornode.h"
#include <QSharedMemory>
#include <QProcess>
#include <QUuid>
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
            process.closeWriteChannel(); // stdin kapatılmazsa Python bekler
            process.waitForFinished();
            qDebug() << "Python output:";
            qDebug().noquote() << process.readAllStandardOutput();
            qDebug().noquote() << process.readAllStandardError();
            return result;
        }

        QHash<int, QByteArray> PythonProcessActorNode::roleNames() const
        {
            auto roles = ActorNode::roleNames();
            roles[NodeRoles::PythonFileName] = "pythonFilename";
            roles[NodeRoles::TableModel] = "tableModel";
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
            else if (role == NodeRoles::TableModel)
            {
                QVariant v = QVariant::fromValue(static_cast<QObject*>(m_inputDataTable));
                return v;
            }
            else
            {
                return ActorNode::data(role);
            }
        }
    }
}


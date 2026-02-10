#include "pythonprocessactornode.h"
#include <QSharedMemory>
#include <QProcess>
#include <QUuid>
#include <constants.h>

struct Row {
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
            , m_memory(nullptr)
        {
        }

        PythonProcessActorNode::~PythonProcessActorNode()
        {
            if(m_memory)
            {
                m_memory->deleteLater();
            }

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

            if (!process.waitForStarted()) {
                qDebug() << "Python başlatılamadı";
                return false;
            }

            QList<Row> rows = {
                {1, 10.5f, "Apple"},
                {2, 20.25f, "Banana"},
                {3, 30.75f, "Cherry"}
            };

            QByteArray buffer;
            QDataStream stream(&buffer, QIODevice::WriteOnly);

            // Endianness sabitle (çok önemli)
            stream.setByteOrder(QDataStream::LittleEndian);
            stream.setFloatingPointPrecision(QDataStream::SinglePrecision); // 🔥 BU ŞART

            // row count
            stream << static_cast<qint32>(rows.size());

            for (const Row& r : rows) {
                stream << static_cast<qint32>(r.id);
                stream << r.value;

                QByteArray strBytes = r.name.toUtf8();
                stream << static_cast<qint32>(strBytes.size());
                stream.writeRawData(strBytes.constData(), strBytes.size());
            }


            process.write(buffer);

            process.closeWriteChannel(); // stdin kapatılmazsa Python bekler
            process.waitForFinished();

            qDebug() << "Python output:";
            qDebug().noquote() << process.readAllStandardOutput();
            qDebug().noquote() << process.readAllStandardError();
            delete m_memory;
            m_memory = nullptr;
            return result;
        }

        QHash<int, QByteArray> PythonProcessActorNode::roleNames() const
        {
            auto roles = ActorNode::roleNames();
            roles[NodeRoles::PythonFileName] = "pythonFilename";
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
            else
            {
                return ActorNode::data(role);
            }
        }
    }
}


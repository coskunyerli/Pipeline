#include <Python.h>
#include <QDebug>
#include <QFile>
#include <QSharedMemory>
#include "pythonfileactornode.h"
#include <constants.h>
//#include <pybind11/embed.h>
//#include <bindings/pythonnoderesult_python.h>

//namespace py = pybind11;
namespace Pipeline
{
    namespace Runtime
    {
        PythonFileActorNode::PythonFileActorNode()
            : ActorNode()
            , m_inputDataTable(new NodeTableModel())
            , m_outputDataTable(new NodeTableModel())
        {
        }

        PythonFileActorNode::~PythonFileActorNode()
        {
            delete m_inputDataTable;
        }

        void PythonFileActorNode::setFilename(const QString &filename)
        {
            m_filename = filename;
        }

        QVariant PythonFileActorNode::behaviour(const Thread::BehaviourContext &behaviour)
        {
            QFile file(m_filename);

            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return false;

            QString script = file.readAll();
            QVariant result;
            //QByteArray scriptUtf8 = script.toUtf8();
            // const char* scriptCStr = scriptUtf8.constData();
            // //py::scoped_interpreter guard{};
            // //py::module_::import("PythonNodeResult");
            // auto* inputResultData = this->m_inputDataTable->getRoot();
            // auto* outputResultData = this->m_outputDataTable->getRoot();
            // //py::dict locals;
            // //locals["inputTable"] = py::cast(inputResultData, py::return_value_policy::reference);
            // //locals["outputTable"] = py::cast(outputResultData, py::return_value_policy::reference);
            // //py::exec(scriptCStr, py::globals(), locals);
            return result;
        }

        QHash<int, QByteArray> PythonFileActorNode::roleNames() const
        {
            auto roles = ActorNode::roleNames();
            roles[NodeRoles::PythonFileName] = "pythonFilename";
            roles[NodeRoles::TableModel] = "tableModel";
            return roles;
        }

        bool PythonFileActorNode::setData(const QVariant &value, int role)
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

        QVariant PythonFileActorNode::data(int role) const
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


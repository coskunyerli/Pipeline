
#include <Python.h>


#include "pythonscriptactornode.h"
#include <QDebug>
#include "constants.h"

namespace Pipeline
{
    namespace Runtime
    {
        PythonScriptActorNode::PythonScriptActorNode()
            : ActorNode()
        {
        }

        void PythonScriptActorNode::setScript(const QString &script)
        {
            m_pythonScript = script;
        }

        QVariant PythonScriptActorNode::behaviour(const Thread::BehaviourContext &behaviour)
        {
            // 1. Python yorumlayıcıyı başlat (ilk kez başlatılmamışsa)
            if (!Py_IsInitialized())
                Py_Initialize();

            QVariant result;
            // 2. Python scripti QString'den al
            QByteArray scriptUtf8 = m_pythonScript.toUtf8();
            const char* scriptCStr = scriptUtf8.constData();
            // 3. Yeni bir Python dict oluştur: scriptin çalışacağı ortam
            PyObject* pGlobalDict = PyDict_New();
            PyDict_SetItemString(pGlobalDict, "__builtins__", PyEval_GetBuiltins());
            // 4. Scripti çalıştır (fonksiyon tanımı vs)
            PyObject* execResult = PyRun_String(scriptCStr, Py_file_input, pGlobalDict, pGlobalDict);

            if (execResult == nullptr)
            {
                PyErr_Print();
                Py_DECREF(pGlobalDict);
                return {};
            }

            // 5. run fonksiyonunu al
            PyObject* pFunc = PyDict_GetItemString(pGlobalDict, "run");

            if (pFunc && PyCallable_Check(pFunc))
            {
                // Basit bir örnek context: Python'a dict olarak gönderilecek
                PyObject* pyContext = PyDict_New();
                PyDict_SetItemString(pyContext, "input", PyUnicode_FromString("Hello from C++"));
                // 6. Python fonksiyonunu çağır
                PyObject* pReturnValue = PyObject_CallFunctionObjArgs(pFunc, pyContext, NULL);
                Py_DECREF(pyContext);

                if (pReturnValue)
                {
                    // 7. Sonucu QVariant'e çevir
                    if (PyUnicode_Check(pReturnValue))
                    {
                        result = QString::fromUtf8(PyUnicode_AsUTF8(pReturnValue));
                    }
                    else if (PyLong_Check(pReturnValue))
                    {
                        result = static_cast<qint64>(PyLong_AsLongLong(pReturnValue));
                    }
                    else if (PyFloat_Check(pReturnValue))
                    {
                        result = PyFloat_AsDouble(pReturnValue);
                    }
                    else if (PyBool_Check(pReturnValue))
                    {
                        result = (pReturnValue == Py_True);
                    }
                    else
                    {
                        qWarning() << "Bilinmeyen dönüş tipi";
                        result = QVariant();
                    }

                    Py_DECREF(pReturnValue);
                }
                else
                {
                    PyErr_Print();
                }
            }
            else
            {
                qWarning() << "Python fonksiyonu 'run' bulunamadı veya çağrılamaz.";
                PyErr_Print();
            }

            Py_DECREF(pGlobalDict);
            qDebug() << result;
            return result;
        }

        QHash<int, QByteArray> PythonScriptActorNode::roleNames() const
        {
            auto roles = ActorNode::roleNames();
            roles[NodeRoles::PythonScript] = "pythonScript";
            return roles;
        }

        bool PythonScriptActorNode::setData(const QVariant &value, int role)
        {
            if (role == NodeRoles::PythonScript)
            {
                this->setScript(value.toString());
                return true;
            }
            else
            {
                return ActorNode::setData(value, role);
            }
        }

        QVariant PythonScriptActorNode::data(int role) const
        {
            if (role == NodeRoles::PythonScript)
            {
                return m_pythonScript;
            }
            else
            {
                return ActorNode::data(role);
            }
        }

        PythonScriptActorNode::PythonScriptActorNode(Thread::Actor *actor)
            : ActorNode(actor)
        {
        }

        PythonScriptActorNode::PythonScriptActorNode(QString script)
            : ActorNode()
            , m_pythonScript(script)
        {
        }

    }
}

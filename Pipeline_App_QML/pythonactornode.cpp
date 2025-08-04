
#include <Python.h>


#include "pythonactornode.h"
#include <QDebug>
#include "constants.h"

namespace Pipeline
{
    namespace Runtime
    {
        PythonActorNode::PythonActorNode()
            : ActorNode()
        {
        }

        void PythonActorNode::setScript(const QString &script)
        {
            m_pythonScript = script;
        }

        QVariant PythonActorNode::behaviour(const Thread::BehaviourContext &behaviour)
        {
            // 1. Python yorumlayıcıyı başlat (ilk kez başlatılmamışsa)
            if (!Py_IsInitialized())
                Py_Initialize();

            qDebug() << 24;
            QVariant result;
            // 2. Python scripti QString'den al
            QByteArray scriptUtf8 = m_pythonScript.toUtf8();
            const char* scriptCStr = scriptUtf8.constData();
            // 3. Yeni bir Python dict oluştur: scriptin çalışacağı ortam
            PyObject* pGlobalDict = PyDict_New();
            PyDict_SetItemString(pGlobalDict, "__builtins__", PyEval_GetBuiltins());
            // 4. Scripti çalıştır (fonksiyon tanımı vs)
            PyObject* execResult = PyRun_String(scriptCStr, Py_file_input, pGlobalDict, pGlobalDict);
            qDebug() << 34;

            if (execResult == nullptr)
            {
                qDebug() << 37;
                PyErr_Print();
                Py_DECREF(pGlobalDict);
                return {};
            }

            // 5. run fonksiyonunu al
            PyObject* pFunc = PyDict_GetItemString(pGlobalDict, "run");

            if (pFunc && PyCallable_Check(pFunc))
            {
                qDebug() << 47;
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

        QHash<int, QByteArray> PythonActorNode::roleNames() const
        {
            auto roles = ActorNode::roleNames();
            roles[NodeRoles::PythonFileName] = "pythonFilename";
            return roles;
        }

        bool PythonActorNode::setData(const QVariant &value, int role)
        {
            if (role == NodeRoles::PythonFileName)
            {
                this->setScript(value.toString());
                return true;
            }
            else
            {
                return ActorNode::setData(value, role);
            }
        }

        PythonActorNode::PythonActorNode(Thread::Actor *actor)
            : ActorNode(actor)
        {
        }

        PythonActorNode::PythonActorNode(QString script)
            : ActorNode()
            , m_pythonScript(script)
        {
        }

    }
}

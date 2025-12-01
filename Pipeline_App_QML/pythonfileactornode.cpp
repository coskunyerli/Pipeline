#include <Python.h>
#include <QDebug>
#include <QFile>
#include "pythonfileactornode.h"
#include "constants.h"
namespace
{
    QStringList getFunctionParamNames(PyObject* pFunc)
    {
        QStringList paramNames;

        if (!pFunc || !PyCallable_Check(pFunc))
        {
            qWarning() << "Verilen nesne çağrılabilir değil.";
            return paramNames;
        }

        // Fonksiyonun __code__ nesnesini al
        PyObject* codeObj = PyObject_GetAttrString(pFunc, "__code__");

        if (!codeObj)
        {
            PyErr_Print();
            return paramNames;
        }

        // co_varnames bir tuple (parametre ve local değişken isimleri)
        PyObject* varnames = PyObject_GetAttrString(codeObj, "co_varnames");
        Py_DECREF(codeObj);

        if (!varnames || !PyTuple_Check(varnames))
        {
            PyErr_Print();
            Py_XDECREF(varnames);
            return paramNames;
        }

        // co_argcount parametre sayısını al
        PyObject* argCountObj = PyObject_GetAttrString(pFunc, "__code__");

        if (!argCountObj)
        {
            PyErr_Print();
            Py_DECREF(varnames);
            return paramNames;
        }

        PyObject* argCountPy = PyObject_GetAttrString(argCountObj, "co_argcount");
        Py_DECREF(argCountObj);

        if (!argCountPy || !PyLong_Check(argCountPy))
        {
            PyErr_Print();
            Py_XDECREF(argCountPy);
            Py_DECREF(varnames);
            return paramNames;
        }

        int argCount = (int)PyLong_AsLong(argCountPy);
        Py_DECREF(argCountPy);

        // İlk argCount kadar tuple elemanını al (bunlar fonksiyon parametre isimleri)
        for (int i = 0; i < argCount; ++i)
        {
            PyObject* item = PyTuple_GetItem(varnames, i); // Borrowed reference

            if (PyUnicode_Check(item))
            {
                const char* name = PyUnicode_AsUTF8(item);
                paramNames.append(QString::fromUtf8(name));
            }
        }

        Py_DECREF(varnames);
        return paramNames;
    }
}
namespace Pipeline
{
    namespace Runtime
    {
        PythonFileActorNode::PythonFileActorNode()
            : ActorNode()
        {
        }

        void PythonFileActorNode::setFilename(const QString &filename)
        {
            m_filename = filename;
        }

        QVariant PythonFileActorNode::behaviour(const Thread::BehaviourContext &behaviour)
        {
            // 1. Python yorumlayıcıyı başlat (ilk kez başlatılmamışsa)
            if (!Py_IsInitialized())
                Py_Initialize();

            QFile file(m_filename);

            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                return false;

            QString script = file.readAll();
            QVariant result;
            // 2. Python scripti QString'den al
            QByteArray scriptUtf8 = script.toUtf8();
            const char* scriptCStr = scriptUtf8.constData();
            // 3. Yeni bir Python dict oluştur: scriptin çalışacağı ortam
            PyObject* pGlobalDict = PyDict_New();
            PyDict_SetItemString(pGlobalDict, "__builtins__", PyEval_GetBuiltins());
            // 4. Scripti çalıştır (fonksiyon tanımı vs)
            PyObject* execResult = PyRun_String(scriptCStr, Py_file_input, pGlobalDict, pGlobalDict);
            qDebug() << 34;

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

        QHash<int, QByteArray> PythonFileActorNode::roleNames() const
        {
            auto roles = ActorNode::roleNames();
            roles[NodeRoles::PythonFileName] = "pythonFilename";
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
            else
            {
                return ActorNode::data(role);
            }
        }
    }
}


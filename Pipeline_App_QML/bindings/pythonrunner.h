#pragma once
#include <pybind11/embed.h>
#include <string>
#include <data/pythonnoderesult.h>

namespace py = pybind11;

namespace Pipeline::Runtime
{
class PythonRunner
{
public:
    PythonRunner() {
        // Interpreter'ı başlat (uygulama başında bir kez)
        if (!Py_IsInitialized()) {
            py::initialize_interpreter();
            m_ownsInterpreter = true;
        }
    }

    ~PythonRunner() {
        if (m_ownsInterpreter && Py_IsInitialized()) {
            py::finalize_interpreter();
        }
    }

    // Script çalıştır, result objesini Python'a "result" adıyla geçir
    bool execute(const std::string& script, PythonNodeResult* result) {
        try {
            py::dict locals;

            // C++ objesini Python'a geçir (ownership C++'ta kalıyor)
            locals["result"] = py::cast(result, py::return_value_policy::reference);

            // Script'i çalıştır
            py::exec(script, py::globals(), locals);

            return true;
        }
        catch (const py::error_already_set& e) {
            m_lastError = e.what();
            return false;
        }
    }

    // Dosyadan script çalıştır
    bool executeFile(const std::string& filepath, PythonNodeResult* result) {
        try {
            py::dict locals;
            locals["result"] = py::cast(result, py::return_value_policy::reference);

            py::eval_file(filepath, py::globals(), locals);
            return true;
        }
        catch (const py::error_already_set& e) {
            m_lastError = e.what();
            return false;
        }
    }

    std::string getLastError() const { return m_lastError; }

private:
    bool m_ownsInterpreter = false;
    std::string m_lastError;
};
}

// pythonnoderesult_python.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <data/pythonnoderesult.h>

namespace py = pybind11;
using namespace Pipeline::Runtime;

PYBIND11_MODULE(PythonNodeResult, m) {
    m.doc() = "PythonNodeResult - hierarchical table data structure";

    py::enum_<PythonNodeResult::ValueType>(m, "ValueType")
        .value("None", PythonNodeResult::ValueType::None)
        .value("Value", PythonNodeResult::ValueType::Value)
        .value("Matrix", PythonNodeResult::ValueType::Matrix)
        .value("All", PythonNodeResult::ValueType::All)
        .export_values();

    py::class_<PythonNodeResult>(m, "NodeResult")
        .def(py::init<>())
        .def("set_size", &PythonNodeResult::setSize)
        .def_property_readonly("column_count", &PythonNodeResult::getColumnCount)
        .def_property_readonly("row_count", &PythonNodeResult::getRowCount)
        .def_property("value",
                      &PythonNodeResult::getValue,
                      &PythonNodeResult::setValue)
        .def_property_readonly("value_type", &PythonNodeResult::getValueType)
        .def_property_readonly("child_count", &PythonNodeResult::getChildCount)
        .def("get_cell", &PythonNodeResult::getCell,
             py::return_value_policy::reference_internal)
        .def("get_or_create_cell", &PythonNodeResult::getOrCreateCell,
             py::return_value_policy::reference_internal)
        .def("delete_cell", &PythonNodeResult::deleteCell)
        .def("__getitem__", [](PythonNodeResult& self, std::pair<size_t, size_t> idx) {
                return self.getCell(idx.first, idx.second);
            }, py::return_value_policy::reference_internal)
        .def("__setitem__", [](PythonNodeResult& self, std::pair<size_t, size_t> idx, const std::string& val) {
            auto* cell = self.getOrCreateCell(idx.first, idx.second);
            if (cell) cell->setValue(val);
        });

    // --- Serialization ---
    m.def("serialize", [](PythonNodeResult& node) {
            auto buf = PythonNodeResult::serialize(&node);
            return py::bytes(reinterpret_cast<const char*>(buf.data()), buf.size());
        }, py::arg("node"),
        "Serialize a NodeResult tree to binary bytes");

    m.def("deserialize", [](py::bytes data) {
            std::string str = data;
            auto* node = PythonNodeResult::deserialize(
                reinterpret_cast<const uint8_t*>(str.data()), str.size());
            return node;
        }, py::arg("data"),
        py::return_value_policy::take_ownership,
        "Deserialize binary bytes into a NodeResult tree");
}

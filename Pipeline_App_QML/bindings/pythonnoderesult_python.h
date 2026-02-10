#pragma once
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <data/pythonnoderesult.h>

namespace py = pybind11;
using namespace Pipeline::Runtime;

PYBIND11_EMBEDDED_MODULE(PythonNodeResult, m) {
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
             py::return_value_policy::reference)
        .def("get_or_create_cell", &PythonNodeResult::getOrCreateCell,
             py::return_value_policy::reference)
        .def("delete_cell", &PythonNodeResult::deleteCell)
        .def("__getitem__", [](PythonNodeResult& self, std::pair<size_t, size_t> idx) {
                return self.getCell(idx.first, idx.second);
            }, py::return_value_policy::reference)
        .def("__setitem__", [](PythonNodeResult& self, std::pair<size_t, size_t> idx, const std::string& val) {
            auto* cell = self.getOrCreateCell(idx.first, idx.second);
            if (cell) cell->setValue(val);
        });
}

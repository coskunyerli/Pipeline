// HierarchicalTableData_python.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <data/hierarchicaltabledata.h>

namespace py = pybind11;
using namespace Pipeline::Runtime;

PYBIND11_MODULE(HierarchicalTableData, m) {
    m.doc() = "HierarchicalTableData - hierarchical table data structure";

    py::enum_<HierarchicalTableData::ValueType>(m, "ValueType")
        .value("None", HierarchicalTableData::ValueType::None)
        .value("Value", HierarchicalTableData::ValueType::Value)
        .value("Matrix", HierarchicalTableData::ValueType::Matrix)
        .value("All", HierarchicalTableData::ValueType::All)
        .export_values();

    py::class_<HierarchicalTableData>(m, "HierarchicalTableData")
        .def(py::init<>())
        .def("set_size", &HierarchicalTableData::setSize)
        .def_property_readonly("column_count", &HierarchicalTableData::getColumnCount)
        .def_property_readonly("row_count", &HierarchicalTableData::getRowCount)
        .def_property("value",
                      &HierarchicalTableData::getValue,
                      &HierarchicalTableData::setValue)
        .def_property_readonly("value_type", &HierarchicalTableData::getValueType)
        .def_property_readonly("get_cell_value_type", &HierarchicalTableData::getCellValueType)
        .def_property_readonly("child_count", &HierarchicalTableData::getChildCount)
        .def("get_cell", &HierarchicalTableData::getCell,
             py::return_value_policy::reference_internal)
        .def("get_or_create_cell", &HierarchicalTableData::getOrCreateCell,
             py::return_value_policy::reference_internal)
        .def("delete_cell", &HierarchicalTableData::deleteCell)
        .def("__getitem__", [](HierarchicalTableData& self, std::pair<size_t, size_t> idx) {
                return self.getCellValue(idx.first, idx.second);
            }, py::return_value_policy::reference_internal)
        .def("__setitem__", [](HierarchicalTableData& self, std::pair<size_t, size_t> idx, const std::string& val) {
            self.setCellValue(idx.first, idx.second, val);
        });

    // --- Serialization ---
    m.def("serialize", [](HierarchicalTableData& node) {
            auto buf = HierarchicalTableData::serialize(&node);
            return py::bytes(reinterpret_cast<const char*>(buf.data()), buf.size());
        }, py::arg("node"),
        "Serialize a NodeResult tree to binary bytes");

    m.def("deserialize", [](py::bytes data) {
            std::string str = data;
            auto* node = HierarchicalTableData::deserialize(
                reinterpret_cast<const uint8_t*>(str.data()), str.size());
            return node;
        }, py::arg("data"),
        py::return_value_policy::take_ownership,
        "Deserialize binary bytes into a HierarchicalTableData tree");
}

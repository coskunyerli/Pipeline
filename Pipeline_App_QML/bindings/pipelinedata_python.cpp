// HierarchicalTableData_python.cpp
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <data/hierarchicaltabledata.h>
#include <data/nodeparameterlist.h>

namespace py = pybind11;
using namespace Pipeline::Runtime;

PYBIND11_MODULE(PipelineData, m) {
    m.doc() = "PipelineData - Pipeline data structure";

    py::enum_<HierarchicalTableData::ValueType>(m, "ValueType")
        .value("None", HierarchicalTableData::ValueType::None)
        .value("Value", HierarchicalTableData::ValueType::Value)
        .value("Matrix", HierarchicalTableData::ValueType::Matrix)
        .value("All", HierarchicalTableData::ValueType::All)
        .export_values();

    py::class_<HierarchicalTableData>(m, "HierarchicalTableData")
        .def(py::init<>())
        .def("set_size", &HierarchicalTableData::setSize)
        .def("set_header_data", &HierarchicalTableData::setHeaderData)
        .def("get_header_data", &HierarchicalTableData::getHeaderData)
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
        .def("__getitem__", [](HierarchicalTableData& self, std::pair<size_t, size_t> idx) {
                return self.getCellValue(idx.first, idx.second);
            }, py::return_value_policy::reference_internal)
        .def("__setitem__", [](HierarchicalTableData& self, std::pair<size_t, size_t> idx, const std::string& val) {
            self.setCellValue(idx.first, idx.second, val);

        }) // --- Serialize as member ---
        .def("serialize", [](HierarchicalTableData& self) {
            auto buf = HierarchicalTableData::serialize(&self);
            return py::bytes(reinterpret_cast<const char*>(buf.data()), buf.size());
        })

        // --- Deserialize as static member ---
        .def_static("deserialize", [](py::bytes data) {
                std::string str = data;
                auto* node = HierarchicalTableData::deserialize(
                    reinterpret_cast<const uint8_t*>(str.data()), str.size());
                return node;
            }, py::return_value_policy::take_ownership);

    // NodeParameter binding
    py::class_<NodeParameter>(m, "NodeParameter")
        .def_readwrite("name", &NodeParameter::name)
        .def_readwrite("type", &NodeParameter::type)

        // value property
        .def_property("value",
            [](const NodeParameter& p) -> const PVariant::Value& {
                return p.value.data();  // const reference dön
            },
            [](NodeParameter& p, const PVariant::Value& v) {
                p.value.set(v);        // value set
            },
            py::return_value_policy::reference_internal
            );

    // NodeParameterList binding
    py::class_<NodeParameterList>(m, "NodeParameterList")
        .def(py::init<>())

        .def("add_parameter",
            [](NodeParameterList& self,
               const std::string& name,
               int type,
               const PVariant::Value& value)
            {
                self.addParameter(name, type, PVariant(value));
            },
            py::arg("name"),
            py::arg("type"),
            py::arg("value") = PVariant::Value{}
            )

        .def("get_parameter_index", &NodeParameterList::getParameterIndex)
        .def("get_parameter_name", &NodeParameterList::getParameterName)

        .def("remove_parameter",
             py::overload_cast<int>(&NodeParameterList::removeParameter))
        .def("remove_parameter",
             py::overload_cast<const std::string&>(&NodeParameterList::removeParameter))

        .def("param_size", &NodeParameterList::paramSize)

        .def("get_parameter",
            [](const NodeParameterList& self, size_t index)
            {
                bool has;
                const auto& p = self.getParameter(index, has);
                if (!has)
                    throw std::runtime_error("Parameter not found");
                return &p;
            },
            py::return_value_policy::reference_internal
            )

        .def("get_parameter_by_name",
            [](const NodeParameterList& self, const std::string& name)
            {
                bool has;
                const auto& p = self.getParameter(name, has);
                if (!has)
                    throw std::runtime_error("Parameter not found");
                return &p;
            },
            py::return_value_policy::reference_internal
            )

        .def("serialize",
             [](const NodeParameterList& self)
             {
                 std::vector<uint8_t> out;
                 self.serialize(out);
                 return out;
             })

        .def("deserialize",
             [](NodeParameterList& self, py::bytes data)
             {
                std::string str = data;
                self.deserialize(reinterpret_cast<const uint8_t*>(str.data()), str.size());
             });
}

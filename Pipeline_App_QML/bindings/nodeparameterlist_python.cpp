#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
//#include <pybind11/variant.h>  // <--- variant desteği
#include <data/nodeparameterlist.h>

namespace py = pybind11;
using namespace Pipeline::Runtime;

PYBIND11_MODULE(pipeline_runtime, m)
{
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
             [](NodeParameterList& self, const std::vector<uint8_t>& data)
             {
                 self.deserialize(data.data(), data.size());
             });
}

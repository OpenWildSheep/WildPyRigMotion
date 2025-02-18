
#include "riglogic/riglogic/RigInstance.h"
#include "riglogic/riglogic/RigLogic.h"

#include "dna/JSONStreamWriter.h"
#include "dna/JSONStreamReader.h"
#include "dna/BinaryStreamWriter.h"
#include "dna/BinaryStreamReader.h"

#pragma warning(push, 0)
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/functional.h"
#include "pybind11/numpy.h"
#pragma warning(pop)

using namespace pybind11::literals;

namespace py = pybind11;

namespace pybind11
{
    namespace detail
    {
        template <>
        struct type_caster<av::StringView> : string_caster<av::StringView, true>
        {
        };
    } // namespace detail
} // namespace pybind11

template <typename T>
struct Deleter
{
    void operator()(T* ptr) const
    {
        T::destroy(ptr);
    }
};

PYBIND11_MODULE(pyriglogic, rlpy)
{
    rlpy.doc() = "pybind11 for RigLogic";

    // ****************************************** RL4 ***************************************************

    using ConstArrayViewFloat = rl4::ConstArrayView<float>;
    py::class_<ConstArrayViewFloat>(rlpy, "ArrayViewConstFloat", py::buffer_protocol())
        .def(
            "__getitem__",
            [](const ConstArrayViewFloat& self, std::size_t i)
            {
                if (i >= self.size())
                    throw py::index_error();
                return self[i];
            })
        .def("__len__", &ConstArrayViewFloat::size)
        .def(
            "data",
            [](ConstArrayViewFloat& self) { return py::array_t<float const>({self.size()}, {sizeof(float)}, self.data()); },
            py::return_value_policy::reference_internal);

    using ArrayViewFloat = rl4::ArrayView<float>;
    py::class_<ArrayViewFloat>(rlpy, "ArrayViewFloat", py::buffer_protocol())
        .def(
            "__getitem__",
            [](const ArrayViewFloat& self, std::size_t i)
            {
                if (i >= self.size())
                    throw py::index_error();
                return self[i];
            })
        .def("__len__", &ArrayViewFloat::size)
        .def(
            "data",
            [](ArrayViewFloat& self) { return py::array_t<float const>({self.size()}, {sizeof(float)}, self.data()); },
            py::return_value_policy::reference_internal);

    py::enum_<rl4::CalculationType>(rlpy, "CalculationType")
        .value("Scalar", rl4::CalculationType::Scalar)
        .value("SSE", rl4::CalculationType::SSE)
        .value("AVX", rl4::CalculationType::AVX)
        .value("NEON", rl4::CalculationType::NEON)
        .value("AnyVector", rl4::CalculationType::AnyVector)
        .export_values();

    py::enum_<rl4::TranslationType>(rlpy, "TranslationType").value("Vector", rl4::TranslationType::Vector).export_values();

    py::enum_<rl4::RotationType>(rlpy, "RotationType")
        .value("EulerAngles", rl4::RotationType::EulerAngles)
        .value("Quaternions", rl4::RotationType::Quaternions)
        .export_values();

    py::enum_<rl4::RotationOrder>(rlpy, "RotationOrder")
        .value("XYZ", rl4::RotationOrder::XYZ)
        .value("XZY", rl4::RotationOrder::XZY)
        .value("YXZ", rl4::RotationOrder::YXZ)
        .value("YZX", rl4::RotationOrder::YZX)
        .value("ZXY", rl4::RotationOrder::ZXY)
        .value("ZYX", rl4::RotationOrder::ZYX)
        .export_values();

    py::enum_<rl4::ScaleType>(rlpy, "ScaleType").value("Vector", rl4::ScaleType::Vector).export_values();

    py::class_<rl4::Configuration>(rlpy, "Configuration")
        .def(py::init<>())
        .def_readwrite("calculationType", &rl4::Configuration::calculationType)
        .def_readwrite("loadJoints", &rl4::Configuration::loadJoints)
        .def_readwrite("loadBlendShapes", &rl4::Configuration::loadBlendShapes)
        .def_readwrite("loadAnimatedMaps", &rl4::Configuration::loadAnimatedMaps)
        .def_readwrite("loadMachineLearnedBehavior", &rl4::Configuration::loadMachineLearnedBehavior)
        .def_readwrite("loadRBFBehavior", &rl4::Configuration::loadRBFBehavior)
        .def_readwrite("loadTwistSwingBehavior", &rl4::Configuration::loadTwistSwingBehavior)
        .def_readwrite("translationType", &rl4::Configuration::translationType)
        .def_readwrite("rotationType", &rl4::Configuration::rotationType)
        .def_readwrite("rotationOrder", &rl4::Configuration::rotationOrder)
        .def_readwrite("scaleType", &rl4::Configuration::scaleType);

    py::class_<rl4::RigLogic, std::unique_ptr<rl4::RigLogic, Deleter<rl4::RigLogic>>>(rlpy, "RigLogic")
        .def(
            py::init(&rl4::RigLogic::create),
            py::keep_alive<1, 2>(),
            py::arg("reader"),
            py::arg("config") = rl4::Configuration{},
            py::arg("memRes") = (pma::MemoryResource*)nullptr)
        .def_static("restore", &rl4::RigLogic::restore)
        .def("dump", &rl4::RigLogic::dump)
        .def("getLODCount", &rl4::RigLogic::getLODCount)
        .def("getNeutralJointValues", &rl4::RigLogic::getNeutralJointValues)
        .def("getJointVariableAttributeIndices", &rl4::RigLogic::getJointVariableAttributeIndices)
        .def("getJointGroupCount", &rl4::RigLogic::getJointGroupCount)
        .def("getNeuralNetworkCount", &rl4::RigLogic::getNeuralNetworkCount)
        .def("getRBFSolverCount", &rl4::RigLogic::getRBFSolverCount)
        .def("getMeshCount", &rl4::RigLogic::getMeshCount)
        .def("getMeshRegionCount", &rl4::RigLogic::getMeshRegionCount)
        .def("getNeuralNetworkIndices", &rl4::RigLogic::getNeuralNetworkIndices)
        .def("mapGUIToRawControls", &rl4::RigLogic::mapGUIToRawControls)
        .def("mapRawToGUIControls", &rl4::RigLogic::mapRawToGUIControls)
        .def("calculateControls", &rl4::RigLogic::calculateControls)
        .def(
            "calculateMachineLearnedBehaviorControls",
            static_cast<void (rl4::RigLogic::*)(rl4::RigInstance*) const>(&rl4::RigLogic::calculateMachineLearnedBehaviorControls))
        .def(
            "calculateMachineLearnedBehaviorControls",
            static_cast<void (rl4::RigLogic::*)(rl4::RigInstance*, std::uint16_t) const>(
                &rl4::RigLogic::calculateMachineLearnedBehaviorControls))
        .def("calculateRBFControls", static_cast<void (rl4::RigLogic::*)(rl4::RigInstance*) const>(&rl4::RigLogic::calculateRBFControls))
        .def(
            "calculateRBFControls",
            static_cast<void (rl4::RigLogic::*)(rl4::RigInstance*, std::uint16_t) const>(&rl4::RigLogic::calculateRBFControls))
        .def("calculateJoints", static_cast<void (rl4::RigLogic::*)(rl4::RigInstance*) const>(&rl4::RigLogic::calculateJoints))
        .def("calculateJoints", static_cast<void (rl4::RigLogic::*)(rl4::RigInstance*, std::uint16_t) const>(&rl4::RigLogic::calculateJoints))
        .def("calculateBlendShapes", &rl4::RigLogic::calculateBlendShapes)
        .def("calculateAnimatedMaps", &rl4::RigLogic::calculateAnimatedMaps)
        .def("calculate", &rl4::RigLogic::calculate);

    py::class_<rl4::RigInstance, std::unique_ptr<rl4::RigInstance, Deleter<rl4::RigInstance>>>(rlpy, "RigInstance")
        .def(py::init(&rl4::RigInstance::create), py::keep_alive<1, 2>(), py::arg("rigLogic"), py::arg("memRes") = (pma::MemoryResource*)nullptr)

        .def("getGUIControlCount", &rl4::RigInstance::getGUIControlCount)
        .def("getGUIControl", &rl4::RigInstance::getGUIControl)
        .def("setGUIControl", &rl4::RigInstance::setGUIControl)
        .def("getGUIControlValues", &rl4::RigInstance::getGUIControlValues)
        .def(
            "setGUIControlValues",
            [](rl4::RigInstance* ri, std::vector<float> input_array)
            {
                if (input_array.size() != ri->getGUIControlCount())
                {
                    throw py::value_error("setGUIControlValues() expect an array of size equal to "
                                          "getGUIControlCount()");
                }
                ri->setGUIControlValues(input_array.data());
            })

        .def("getRawControlCount", &rl4::RigInstance::getRawControlCount)
        .def("getRawControl", &rl4::RigInstance::getRawControl)
        .def("setRawControl", &rl4::RigInstance::setRawControl)
        .def("getRawControlValues", &rl4::RigInstance::getRawControlValues)
        .def(
            "setRawControlValues",
            [](rl4::RigInstance* ri, std::vector<float> input_array)
            {
                if (input_array.size() != ri->getRawControlCount())
                {
                    throw py::value_error("setRawControlValues() expect an array of size equal to "
                                          "getRawControlCount()");
                }
                ri->setRawControlValues(input_array.data());
            })

        .def("getPSDControlCount", &rl4::RigInstance::getPSDControlCount)
        .def("getPSDControl", &rl4::RigInstance::getPSDControl)
        .def("getPSDControlValues", &rl4::RigInstance::getPSDControlValues)

        .def("getMLControlCount", &rl4::RigInstance::getMLControlCount)
        .def("getMLControl", &rl4::RigInstance::getMLControl)
        .def("getMLControlValues", &rl4::RigInstance::getMLControlValues)

        .def("getNeuralNetworkCount", &rl4::RigInstance::getNeuralNetworkCount)
        .def("getNeuralNetworkMask", &rl4::RigInstance::getNeuralNetworkMask)
        .def("setNeuralNetworkMask", &rl4::RigInstance::setNeuralNetworkMask)

        .def("getRBFControlCount", &rl4::RigInstance::getRBFControlCount)
        .def("getRBFControl", &rl4::RigInstance::getRBFControl)
        .def("getRBFControlValues", &rl4::RigInstance::getRBFControlValues)

        .def("getJointOutputs", &rl4::RigInstance::getJointOutputs)

        .def("getBlendShapeOutputs", &rl4::RigInstance::getBlendShapeOutputs)

        .def("getAnimatedMapOutputs", &rl4::RigInstance::getAnimatedMapOutputs)

        .def("getRBFControlVgetLODalues", &rl4::RigInstance::getLOD)

        .def("setLOD", &rl4::RigInstance::setLOD);
}

// clang-format on

/// @endcond

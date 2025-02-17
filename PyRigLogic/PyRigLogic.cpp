
#include "riglogic/riglogic/RigInstance.h"
#include "riglogic/riglogic/RigLogic.h"

#pragma warning(push, 0)
#include "pybind11/pybind11.h"
#include "pybind11/stl.h"
#include "pybind11/functional.h"
#include "pybind11/numpy.h"
#pragma warning(pop)

using namespace pybind11::literals;

using namespace rl4;
using namespace pma;
// using namespace dna;

namespace py = pybind11;

template<typename T>
struct Deleter
{
    void operator()(T* ptr) const
    {
        T::destroy(ptr);
    }
};


PYBIND11_MODULE(PyRigLogic, rlpy)
{
    rlpy.doc() = "pybind11 for RigLogic";

    using ConstArrayViewFloat = ConstArrayView<float>;
    py::class_<ConstArrayViewFloat>(rlpy, "ArrayViewConstFloat", py::buffer_protocol())
        .def("__getitem__", [](const ConstArrayViewFloat& self, std::size_t i) {
            if (i >= self.size()) 
                throw py::index_error();
            return self[i];
        })
        .def("__len__", &ConstArrayViewFloat::size)
        .def("data", [](ConstArrayViewFloat& self) {
            return py::array_t<float const>({ self.size() }, { sizeof(float) }, self.data());
        }, py::return_value_policy::reference_internal)
        ;

    py::class_<av::StringView>(rlpy, "avStringView")
        .def("__str__", &av::StringView::c_str)  // Permet de convertir en string Python
        //.def("__repr__", [](const av::StringView& self) {
        //    return std::format("<avStringView '{}'>", self.c_str());
        //})
        .def("data", &av::StringView::c_str)  // Retourne la chaîne sous forme de string Python
        .def("__len__", &av::StringView::size)
        ;


    py::class_<dna::HeaderReader, std::unique_ptr<dna::HeaderReader, py::nodelete>>(rlpy, "HeaderReader")
        .def("getFileFormatGeneration", &dna::HeaderReader::getFileFormatGeneration)
        .def("getFileFormatVersion", &dna::HeaderReader::getFileFormatVersion)
        ;

    py::class_<dna::DescriptorReader, dna::HeaderReader, std::unique_ptr<dna::DescriptorReader, py::nodelete>>(rlpy, "DescriptorReader")
        .def("getName", &dna::DescriptorReader::getName)
        .def("getArchetype", &dna::DescriptorReader::getArchetype)
        .def("getGender", &dna::DescriptorReader::getGender)
        .def("getAge", &dna::DescriptorReader::getAge)
        .def("getMetaDataCount", &dna::DescriptorReader::getMetaDataCount)
        .def("getMetaDataKey", &dna::DescriptorReader::getMetaDataKey)
        .def("getMetaDataValue", &dna::DescriptorReader::getMetaDataValue)
        .def("getTranslationUnit", &dna::DescriptorReader::getTranslationUnit)
        .def("getRotationUnit", &dna::DescriptorReader::getRotationUnit)
        .def("getCoordinateSystem", &dna::DescriptorReader::getCoordinateSystem)
        .def("getLODCount", &dna::DescriptorReader::getLODCount)
        .def("getDBMaxLOD", &dna::DescriptorReader::getDBMaxLOD)
        .def("getDBComplexity", &dna::DescriptorReader::getDBComplexity)
        .def("getDBName", &dna::DescriptorReader::getDBName)
        ;

    py::class_<dna::DefinitionReader, dna::DescriptorReader, std::unique_ptr<dna::DefinitionReader, py::nodelete>>(rlpy, "DefinitionReader")
        .def("getGUIControlCount", &dna::DefinitionReader::getGUIControlCount)
        .def("getGUIControlName", &dna::DefinitionReader::getGUIControlName)
        .def("getRawControlCount", &dna::DefinitionReader::getRawControlCount)
        .def("getRawControlName", &dna::DefinitionReader::getRawControlName)
        .def("getJointCount", &dna::DefinitionReader::getJointCount)
        .def("getJointName", &dna::DefinitionReader::getJointName)
        .def("getJointIndexListCount", &dna::DefinitionReader::getJointIndexListCount)
        .def("getJointIndicesForLOD", &dna::DefinitionReader::getJointIndicesForLOD)
        .def("getJointParentIndex", &dna::DefinitionReader::getJointParentIndex)
        .def("getBlendShapeChannelCount", &dna::DefinitionReader::getBlendShapeChannelCount)
        .def("getBlendShapeChannelName", &dna::DefinitionReader::getBlendShapeChannelName)
        .def("getBlendShapeChannelIndexListCount", &dna::DefinitionReader::getBlendShapeChannelIndexListCount)
        .def("getBlendShapeChannelIndicesForLOD", &dna::DefinitionReader::getBlendShapeChannelIndicesForLOD)
        .def("getAnimatedMapCount", &dna::DefinitionReader::getAnimatedMapCount)
        .def("getAnimatedMapName", &dna::DefinitionReader::getAnimatedMapName)
        .def("getAnimatedMapIndexListCount", &dna::DefinitionReader::getAnimatedMapIndexListCount)
        .def("getAnimatedMapIndicesForLOD", &dna::DefinitionReader::getAnimatedMapIndicesForLOD)
        .def("getMeshCount", &dna::DefinitionReader::getMeshCount)
        .def("getMeshName", &dna::DefinitionReader::getMeshName)
        .def("getMeshIndexListCount", &dna::DefinitionReader::getMeshIndexListCount)
        .def("getMeshIndicesForLOD", &dna::DefinitionReader::getMeshIndicesForLOD)
        .def("getMeshBlendShapeChannelMappingCount", &dna::DefinitionReader::getMeshBlendShapeChannelMappingCount)
        .def("getMeshBlendShapeChannelMapping", &dna::DefinitionReader::getMeshBlendShapeChannelMapping)
        .def("getMeshBlendShapeChannelMappingIndicesForLOD", &dna::DefinitionReader::getMeshBlendShapeChannelMappingIndicesForLOD)
        .def("getNeutralJointTranslation", &dna::DefinitionReader::getNeutralJointTranslation)
        .def("getNeutralJointTranslationXs", &dna::DefinitionReader::getNeutralJointTranslationXs)
        .def("getNeutralJointTranslationYs", &dna::DefinitionReader::getNeutralJointTranslationYs)
        .def("getNeutralJointTranslationZs", &dna::DefinitionReader::getNeutralJointTranslationZs)
        .def("getNeutralJointRotation", &dna::DefinitionReader::getNeutralJointRotation)
        .def("getNeutralJointRotationXs", &dna::DefinitionReader::getNeutralJointRotationXs)
        .def("getNeutralJointRotationYs", &dna::DefinitionReader::getNeutralJointRotationYs)
        .def("getNeutralJointRotationZs", &dna::DefinitionReader::getNeutralJointRotationZs)
        ;

    py::class_<dna::BehaviorReader, dna::DefinitionReader, std::unique_ptr<dna::BehaviorReader, py::nodelete>>(rlpy, "BehaviorReader")
        .def("getGUIToRawInputIndices", &dna::BehaviorReader::getGUIToRawInputIndices)
        .def("getGUIToRawOutputIndices", &dna::BehaviorReader::getGUIToRawOutputIndices)
        .def("getGUIToRawFromValues", &dna::BehaviorReader::getGUIToRawFromValues)
        .def("getGUIToRawToValues", &dna::BehaviorReader::getGUIToRawToValues)
        .def("getGUIToRawSlopeValues", &dna::BehaviorReader::getGUIToRawSlopeValues)
        .def("getGUIToRawCutValues", &dna::BehaviorReader::getGUIToRawCutValues)
        .def("getPSDCount", &dna::BehaviorReader::getPSDCount)
        .def("getPSDRowIndices", &dna::BehaviorReader::getPSDRowIndices)
        .def("getPSDColumnIndices", &dna::BehaviorReader::getPSDColumnIndices)
        .def("getPSDValues", &dna::BehaviorReader::getPSDValues)
        .def("getJointRowCount", &dna::BehaviorReader::getJointRowCount)
        .def("getJointColumnCount", &dna::BehaviorReader::getJointColumnCount)
        .def("getJointVariableAttributeIndices", &dna::BehaviorReader::getJointVariableAttributeIndices)
        .def("getJointGroupCount", &dna::BehaviorReader::getJointGroupCount)
        .def("getJointGroupLODs", &dna::BehaviorReader::getJointGroupLODs)
        .def("getJointGroupInputIndices", &dna::BehaviorReader::getJointGroupInputIndices)
        .def("getJointGroupOutputIndices", &dna::BehaviorReader::getJointGroupOutputIndices)
        .def("getJointGroupValues", &dna::BehaviorReader::getJointGroupValues)
        .def("getJointGroupJointIndices", &dna::BehaviorReader::getJointGroupJointIndices)
        .def("getBlendShapeChannelLODs", &dna::BehaviorReader::getBlendShapeChannelLODs)
        .def("getBlendShapeChannelInputIndices", &dna::BehaviorReader::getBlendShapeChannelInputIndices)
        .def("getBlendShapeChannelOutputIndices", &dna::BehaviorReader::getBlendShapeChannelOutputIndices)
        .def("getAnimatedMapLODs", &dna::BehaviorReader::getAnimatedMapLODs)
        .def("getAnimatedMapInputIndices", &dna::BehaviorReader::getAnimatedMapInputIndices)
        .def("getAnimatedMapOutputIndices", &dna::BehaviorReader::getAnimatedMapOutputIndices)
        .def("getAnimatedMapFromValues", &dna::BehaviorReader::getAnimatedMapFromValues)
        .def("getAnimatedMapToValues", &dna::BehaviorReader::getAnimatedMapToValues)
        .def("getAnimatedMapSlopeValues", &dna::BehaviorReader::getAnimatedMapSlopeValues)
        .def("getAnimatedMapCutValues", &dna::BehaviorReader::getAnimatedMapCutValues)
        ;

    py::class_<dna::RBFBehaviorReader, dna::BehaviorReader, std::unique_ptr<dna::RBFBehaviorReader, py::nodelete>>(rlpy, "RBFBehaviorReader")
        .def("getRBFPoseCount", &dna::RBFBehaviorReader::getRBFPoseCount)
        .def("getRBFPoseName", &dna::RBFBehaviorReader::getRBFPoseName)
        .def("getRBFPoseJointOutputIndices", &dna::RBFBehaviorReader::getRBFPoseJointOutputIndices)
        .def("getRBFPoseBlendShapeChannelOutputIndices", &dna::RBFBehaviorReader::getRBFPoseBlendShapeChannelOutputIndices)
        .def("getRBFPoseAnimatedMapOutputIndices", &dna::RBFBehaviorReader::getRBFPoseAnimatedMapOutputIndices)
        .def("getRBFPoseJointOutputValues", &dna::RBFBehaviorReader::getRBFPoseJointOutputValues)
        .def("getRBFPoseScale", &dna::RBFBehaviorReader::getRBFPoseScale)
        .def("getRBFPoseControlCount", &dna::RBFBehaviorReader::getRBFPoseControlCount)
        .def("getRBFPoseControlName", &dna::RBFBehaviorReader::getRBFPoseControlName)
        .def("getRBFPoseInputControlIndices", &dna::RBFBehaviorReader::getRBFPoseInputControlIndices)
        .def("getRBFPoseOutputControlIndices", &dna::RBFBehaviorReader::getRBFPoseOutputControlIndices)
        .def("getRBFPoseOutputControlWeights", &dna::RBFBehaviorReader::getRBFPoseOutputControlWeights)
        .def("getRBFSolverCount", &dna::RBFBehaviorReader::getRBFSolverCount)
        .def("getRBFSolverIndexListCount", &dna::RBFBehaviorReader::getRBFSolverIndexListCount)
        .def("getRBFSolverIndicesForLOD", &dna::RBFBehaviorReader::getRBFSolverIndicesForLOD)
        .def("getRBFSolverName", &dna::RBFBehaviorReader::getRBFSolverName)
        .def("getRBFSolverRawControlIndices", &dna::RBFBehaviorReader::getRBFSolverRawControlIndices)
        .def("getRBFSolverRawControlValues", &dna::RBFBehaviorReader::getRBFSolverRawControlValues)
        .def("getRBFSolverType", &dna::RBFBehaviorReader::getRBFSolverType)
        .def("getRBFSolverRadius", &dna::RBFBehaviorReader::getRBFSolverRadius)
        .def("getRBFSolverAutomaticRadius", &dna::RBFBehaviorReader::getRBFSolverAutomaticRadius)
        .def("getRBFSolverWeightThreshold", &dna::RBFBehaviorReader::getRBFSolverWeightThreshold)
        .def("getRBFSolverDistanceMethod", &dna::RBFBehaviorReader::getRBFSolverDistanceMethod)
        .def("getRBFSolverNormalizeMethod", &dna::RBFBehaviorReader::getRBFSolverNormalizeMethod)
        .def("getRBFSolverFunctionType", &dna::RBFBehaviorReader::getRBFSolverFunctionType)
        .def("getRBFSolverTwistAxis", &dna::RBFBehaviorReader::getRBFSolverTwistAxis)
        ;

    py::class_<dna::GeometryReader, dna::DefinitionReader, std::unique_ptr<dna::GeometryReader, py::nodelete>>(rlpy, "GeometryReader")
        .def("getVertexPositionCount", &dna::GeometryReader::getVertexPositionCount)
        .def("getVertexPosition", &dna::GeometryReader::getVertexPosition)
        .def("getVertexPositionXs", &dna::GeometryReader::getVertexPositionXs)
        .def("getVertexPositionYs", &dna::GeometryReader::getVertexPositionYs)
        .def("getVertexPositionZs", &dna::GeometryReader::getVertexPositionZs)
        .def("getVertexTextureCoordinateCount", &dna::GeometryReader::getVertexTextureCoordinateCount)
        .def("getVertexTextureCoordinate", &dna::GeometryReader::getVertexTextureCoordinate)
        .def("getVertexTextureCoordinateUs", &dna::GeometryReader::getVertexTextureCoordinateUs)
        .def("getVertexTextureCoordinateVs", &dna::GeometryReader::getVertexTextureCoordinateVs)
        .def("getVertexNormalCount", &dna::GeometryReader::getVertexNormalCount)
        .def("getVertexNormal", &dna::GeometryReader::getVertexNormal)
        .def("getVertexNormalXs", &dna::GeometryReader::getVertexNormalXs)
        .def("getVertexNormalYs", &dna::GeometryReader::getVertexNormalYs)
        .def("getVertexNormalZs", &dna::GeometryReader::getVertexNormalZs)
        .def("getVertexLayoutCount", &dna::GeometryReader::getVertexLayoutCount)
        .def("getVertexLayout", &dna::GeometryReader::getVertexLayout)
        .def("getVertexLayoutPositionIndices", &dna::GeometryReader::getVertexLayoutPositionIndices)
        .def("getVertexLayoutTextureCoordinateIndices", &dna::GeometryReader::getVertexLayoutTextureCoordinateIndices)
        .def("getVertexLayoutNormalIndices", &dna::GeometryReader::getVertexLayoutNormalIndices)
        .def("getFaceCount", &dna::GeometryReader::getFaceCount)
        .def("getFaceVertexLayoutIndices", &dna::GeometryReader::getFaceVertexLayoutIndices)
        .def("getMaximumInfluencePerVertex", &dna::GeometryReader::getMaximumInfluencePerVertex)
        .def("getSkinWeightsCount", &dna::GeometryReader::getSkinWeightsCount)
        .def("getSkinWeightsValues", &dna::GeometryReader::getSkinWeightsValues)
        .def("getSkinWeightsJointIndices", &dna::GeometryReader::getSkinWeightsJointIndices)
        .def("getBlendShapeTargetCount", &dna::GeometryReader::getBlendShapeTargetCount)
        .def("getBlendShapeChannelIndex", &dna::GeometryReader::getBlendShapeChannelIndex)
        .def("getBlendShapeTargetDeltaCount", &dna::GeometryReader::getBlendShapeTargetDeltaCount)
        .def("getBlendShapeTargetDeltaXs", &dna::GeometryReader::getBlendShapeTargetDeltaXs)
        .def("getBlendShapeTargetDeltaYs", &dna::GeometryReader::getBlendShapeTargetDeltaYs)
        .def("getBlendShapeTargetDeltaZs", &dna::GeometryReader::getBlendShapeTargetDeltaZs)
        .def("getBlendShapeTargetVertexIndices", &dna::GeometryReader::getBlendShapeTargetVertexIndices)
        ;

    py::class_<dna::MachineLearnedBehaviorReader, dna::DefinitionReader, std::unique_ptr<dna::MachineLearnedBehaviorReader, py::nodelete>>(rlpy, "MachineLearnedBehaviorReader")
        .def("getMLControlCount", &dna::MachineLearnedBehaviorReader::getMLControlCount)
        .def("getMLControlName", &dna::MachineLearnedBehaviorReader::getMLControlName)
        .def("getNeuralNetworkCount", &dna::MachineLearnedBehaviorReader::getNeuralNetworkCount)
        .def("getNeuralNetworkIndexListCount", &dna::MachineLearnedBehaviorReader::getNeuralNetworkIndexListCount)
        .def("getNeuralNetworkIndicesForLOD", &dna::MachineLearnedBehaviorReader::getNeuralNetworkIndicesForLOD)
        .def("getMeshRegionCount", &dna::MachineLearnedBehaviorReader::getMeshRegionCount)
        .def("getMeshRegionName", &dna::MachineLearnedBehaviorReader::getMeshRegionName)
        .def("getNeuralNetworkIndicesForMeshRegion", &dna::MachineLearnedBehaviorReader::getNeuralNetworkIndicesForMeshRegion)
        .def("getNeuralNetworkInputIndices", &dna::MachineLearnedBehaviorReader::getNeuralNetworkInputIndices)
        .def("getNeuralNetworkOutputIndices", &dna::MachineLearnedBehaviorReader::getNeuralNetworkOutputIndices)
        .def("getNeuralNetworkLayerCount", &dna::MachineLearnedBehaviorReader::getNeuralNetworkLayerCount)
        .def("getNeuralNetworkLayerActivationFunction", &dna::MachineLearnedBehaviorReader::getNeuralNetworkLayerActivationFunction)
        .def("getNeuralNetworkLayerActivationFunctionParameters", &dna::MachineLearnedBehaviorReader::getNeuralNetworkLayerActivationFunctionParameters)
        .def("getNeuralNetworkLayerBiases", &dna::MachineLearnedBehaviorReader::getNeuralNetworkLayerBiases)
        .def("getNeuralNetworkLayerWeights", &dna::MachineLearnedBehaviorReader::getNeuralNetworkLayerWeights)
        ;

    py::class_<dna::JointBehaviorMetadataReader, dna::DefinitionReader, std::unique_ptr<dna::JointBehaviorMetadataReader, py::nodelete>>(rlpy, "JointBehaviorMetadataReader")
        .def("getJointTranslationRepresentation", &dna::JointBehaviorMetadataReader::getJointTranslationRepresentation)
        .def("getJointRotationRepresentation", &dna::JointBehaviorMetadataReader::getJointRotationRepresentation)
        .def("getJointScaleRepresentation", &dna::JointBehaviorMetadataReader::getJointScaleRepresentation)
        ;

    py::class_<dna::TwistSwingBehaviorReader, dna::DefinitionReader, std::unique_ptr<dna::TwistSwingBehaviorReader, py::nodelete>>(rlpy, "TwistSwingBehaviorReader")
        .def("getTwistCount", &dna::TwistSwingBehaviorReader::getTwistCount)
        .def("getTwistSetupTwistAxis", &dna::TwistSwingBehaviorReader::getTwistSetupTwistAxis)
        .def("getTwistInputControlIndices", &dna::TwistSwingBehaviorReader::getTwistInputControlIndices)
        .def("getTwistOutputJointIndices", &dna::TwistSwingBehaviorReader::getTwistOutputJointIndices)
        .def("getTwistBlendWeights", &dna::TwistSwingBehaviorReader::getTwistBlendWeights)
        .def("getSwingCount", &dna::TwistSwingBehaviorReader::getSwingCount)
        .def("getSwingSetupTwistAxis", &dna::TwistSwingBehaviorReader::getSwingSetupTwistAxis)
        .def("getSwingInputControlIndices", &dna::TwistSwingBehaviorReader::getSwingInputControlIndices)
        .def("getSwingOutputJointIndices", &dna::TwistSwingBehaviorReader::getSwingOutputJointIndices)
        .def("getSwingBlendWeights", &dna::TwistSwingBehaviorReader::getSwingBlendWeights)
        ;

    py::class_<dna::Reader, dna::RBFBehaviorReader, dna::GeometryReader, dna::MachineLearnedBehaviorReader,
        dna::JointBehaviorMetadataReader, dna::TwistSwingBehaviorReader, std::unique_ptr<dna::Reader, py::nodelete>>(rlpy, "DNAReader");

    py::class_<dna::StreamReader, dna::Reader>(rlpy, "StreamReader")
        .def("read", &dna::StreamReader::read)
        ;

    py::class_<dna::BinaryStreamReader, dna::StreamReader, std::unique_ptr<dna::BinaryStreamReader, Deleter<dna::BinaryStreamReader>>>(rlpy, "DNABinaryStreamReader")
        .def(py::init([](BoundedIOStream* stream, MemoryResource* memRes) {return BinaryStreamReader::create(stream, DataLayer::All, UnknownLayerPolicy::Preserve, 0u, memRes); }), py::keep_alive<1, 2>())
        //.def(py::init(static_cast<BinaryStreamReader* (*)(BoundedIOStream*, DataLayer, UnknownLayerPolicy, std::uint16_t, MemoryResource*)>(&BinaryStreamReader::create)), py::keep_alive<1, 2>(), 
        //    "stream"_a, py::arg("layer") = DataLayer::All, py::arg("policy") = UnknownLayerPolicy::Preserve, py::arg("maxLOD") = 0u, py::arg("memRes") = nullptr)
        // .def(py::init(static_cast<BinaryStreamReader* (*)(BoundedIOStream*, DataLayer, UnknownLayerPolicy, std::uint16_t, std::uint16_t, MemoryResource*)>(&BinaryStreamReader::create)), py::keep_alive<1, 2>())
        // .def(py::init(static_cast<BinaryStreamReader* (*)(BoundedIOStream*, DataLayer, UnknownLayerPolicy, std::uint16_t*, std::uint16_t, MemoryResource*)>(&BinaryStreamReader::create)), py::keep_alive<1, 2>())
        ;

    auto pyRigLogic = py::class_<RigLogic, std::unique_ptr<RigLogic, Deleter<RigLogic>>>(rlpy, "RigLogic");
    auto pyRigInstance = py::class_<RigInstance, std::unique_ptr<RigInstance, Deleter<RigInstance>>>(rlpy, "RigInstance");
    auto pyMemoryResource = py::class_<MemoryResource>(rlpy, "MemoryResource");
    auto pyDefaultMemoryResource = py::class_<DefaultMemoryResource, MemoryResource>(rlpy, "DefaultMemoryResource");
    auto pyConfiguration = py::class_<Configuration>(rlpy, "Configuration");
    auto pyBoundedIOStream = py::class_<BoundedIOStream>(rlpy, "BoundedIOStream");
    auto pyFileStream = py::class_<FileStream, BoundedIOStream, std::unique_ptr<FileStream, Deleter<FileStream>>>(rlpy, "FileStream");

    pyDefaultMemoryResource
        .def(py::init<>())
        ;

    py::enum_<trio::AccessMode>(rlpy, "AccessMode")
        .value("Read", trio::AccessMode::Read)
        .value("Write", trio::AccessMode::Write)
        .value("ReadWrite", trio::AccessMode::ReadWrite)
        .export_values();

    py::enum_<trio::OpenMode>(rlpy, "OpenMode")
        .value("Binary", trio::OpenMode::Binary)
        .value("Text", trio::OpenMode::Text)
        .export_values();

    pyFileStream
        .def(py::init(&FileStream::create), py::keep_alive<1, 5>())
        .def("open", &FileStream::open)
        .def("close", &FileStream::close)
        .def("size", &FileStream::size)
        .def("read", static_cast<std::size_t (FileStream::*)(char*, std::size_t)>(& FileStream::read))
        // .def("read", static_cast<std::size_t(FileStream::*)(trio::Writable*, std::size_t)>(&FileStream::read))
        .def("write", static_cast<std::size_t(FileStream::*)(char const*, std::size_t)>(&FileStream::write))
        // .def("write", static_cast<std::size_t(FileStream::*)(trio::Readable*, std::size_t)>(&FileStream::write))
        ;

    py::enum_<CalculationType>(rlpy, "CalculationType")
        .value("Scalar", CalculationType::Scalar)
        .value("SSE", CalculationType::SSE)
        .value("AVX", CalculationType::AVX)
        .value("NEON", CalculationType::NEON)
        .value("AnyVector", CalculationType::AnyVector)
        .export_values();

    py::enum_<TranslationType>(rlpy, "TranslationType")
        .value("Vector", TranslationType::Vector)
        .export_values();

    py::enum_<RotationType>(rlpy, "RotationType")
        .value("EulerAngles", RotationType::EulerAngles)
        .value("Quaternions", RotationType::Quaternions)
        .export_values();

    py::enum_<RotationOrder>(rlpy, "RotationOrder")
        .value("XYZ", RotationOrder::XYZ)
        .value("XZY", RotationOrder::XZY)
        .value("YXZ", RotationOrder::YXZ)
        .value("YZX", RotationOrder::YZX)
        .value("ZXY", RotationOrder::ZXY)
        .value("ZYX", RotationOrder::ZYX)
        .export_values();

    pyConfiguration
        .def(py::init<>())
        .def_readwrite("calculationType", &Configuration::calculationType)
        .def_readwrite("loadJoints", &Configuration::loadJoints)
        .def_readwrite("loadBlendShapes", &Configuration::loadBlendShapes)
        .def_readwrite("loadAnimatedMaps", &Configuration::loadAnimatedMaps)
        .def_readwrite("loadMachineLearnedBehavior", &Configuration::loadMachineLearnedBehavior)
        .def_readwrite("loadRBFBehavior", &Configuration::loadRBFBehavior)
        .def_readwrite("loadTwistSwingBehavior", &Configuration::loadTwistSwingBehavior)
        .def_readwrite("translationType", &Configuration::translationType)
        .def_readwrite("rotationType", &Configuration::rotationType)
        .def_readwrite("rotationOrder", &Configuration::rotationOrder)
        .def_readwrite("scaleType", &Configuration::scaleType)
        ;

    py::enum_<ScaleType>(rlpy, "ScaleType")
        .value("Vector", ScaleType::Vector)
        .export_values();

    pyRigLogic
        .def(py::init(&RigLogic::create), py::keep_alive<1, 2>())
        // .def_static("destroy", &RigLogic::destroy)
        .def_static("restore", &RigLogic::restore)
        .def("dump", &RigLogic::dump)
        .def("getLODCount", &RigLogic::getLODCount)
        .def("getNeutralJointValues", &RigLogic::getNeutralJointValues)
        .def("getJointVariableAttributeIndices", &RigLogic::getJointVariableAttributeIndices)
        .def("getJointGroupCount", &RigLogic::getJointGroupCount)
        .def("getNeuralNetworkCount", &RigLogic::getNeuralNetworkCount)
        .def("getRBFSolverCount", &RigLogic::getRBFSolverCount)
        .def("getMeshCount", &RigLogic::getMeshCount)
        .def("getMeshRegionCount", &RigLogic::getMeshRegionCount)
        .def("getNeuralNetworkIndices", &RigLogic::getNeuralNetworkIndices)
        .def("mapGUIToRawControls", &RigLogic::mapGUIToRawControls)
        .def("mapRawToGUIControls", &RigLogic::mapRawToGUIControls)
        .def("calculateControls", &RigLogic::calculateControls)
        .def("calculateMachineLearnedBehaviorControls", static_cast<void (RigLogic::*)(RigInstance*) const>(& RigLogic::calculateMachineLearnedBehaviorControls))
        .def("calculateMachineLearnedBehaviorControls", static_cast<void (RigLogic::*) (RigInstance*, std::uint16_t) const>(&RigLogic::calculateMachineLearnedBehaviorControls))
        .def("calculateRBFControls", static_cast<void (RigLogic::*) (RigInstance*) const>(&RigLogic::calculateRBFControls))
        .def("calculateRBFControls", static_cast<void (RigLogic::*) (RigInstance*, std::uint16_t) const>(&RigLogic::calculateRBFControls))
        .def("calculateJoints", static_cast<void (RigLogic::*)(RigInstance*) const>(&RigLogic::calculateJoints))
        .def("calculateJoints", static_cast<void (RigLogic::*)(RigInstance*, std::uint16_t) const>(&RigLogic::calculateJoints))
        .def("calculateBlendShapes", &RigLogic::calculateBlendShapes)
        .def("calculateAnimatedMaps", &RigLogic::calculateAnimatedMaps)
        .def("calculate", &RigLogic::calculate)
        ;
    
    pyRigInstance
        .def(py::init(&RigInstance::create), py::keep_alive<1, 2>())
        // .def_static("destroy", &RigInstance::destroy)
        
        .def("getGUIControlCount", &RigInstance::getGUIControlCount)
        .def("getGUIControl", &RigInstance::getGUIControl)
        .def("setGUIControl", &RigInstance::setGUIControl)
        .def("getGUIControlValues", &RigInstance::getGUIControlValues)
        .def("setGUIControlValues", &RigInstance::setGUIControlValues)
        
        .def("getRawControlCount", &RigInstance::getRawControlCount)
        .def("getRawControl", &RigInstance::getRawControl)
        .def("setRawControl", &RigInstance::setRawControl)
        .def("getRawControlValues", &RigInstance::getRawControlValues)
        .def("setRawControlValues", &RigInstance::setRawControlValues)

        .def("getPSDControlCount", &RigInstance::getPSDControlCount)
        .def("getPSDControl", &RigInstance::getPSDControl)
        .def("getPSDControlValues", &RigInstance::getPSDControlValues)
        
        .def("getMLControlCount", &RigInstance::getMLControlCount)
        .def("getMLControl", &RigInstance::getMLControl)
        .def("getMLControlValues", &RigInstance::getMLControlValues)

        .def("getNeuralNetworkCount", &RigInstance::getNeuralNetworkCount)
        .def("getNeuralNetworkMask", &RigInstance::getNeuralNetworkMask)
        .def("setNeuralNetworkMask", &RigInstance::setNeuralNetworkMask)

        .def("getRBFControlCount", &RigInstance::getRBFControlCount)
        .def("getRBFControl", &RigInstance::getRBFControl)
        .def("getRBFControlValues", &RigInstance::getRBFControlValues)

        .def("getJointOutputs", &RigInstance::getJointOutputs)

        .def("getBlendShapeOutputs", &RigInstance::getBlendShapeOutputs)

        .def("getAnimatedMapOutputs", &RigInstance::getAnimatedMapOutputs)

        .def("getRBFControlVgetLODalues", &RigInstance::getLOD)

        .def("setLOD", &RigInstance::setLOD)
        ;
}

// clang-format on

/// @endcond

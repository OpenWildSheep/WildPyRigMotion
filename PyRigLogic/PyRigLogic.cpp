
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

PYBIND11_MODULE(PyRigLogic, rlpy)
{
    rlpy.doc() = "pybind11 for RigLogic";

    py::class_<sc::StatusCode>(rlpy, "StatusCode").def_readwrite("code", &sc::StatusCode::code).def_readwrite("message", &sc::StatusCode::message);

    py::class_<sc::Status>(rlpy, "Status")
        .def_static("isOk", &sc::Status::isOk)
        .def_static("get", &sc::Status::get)
        .def_static("getHook", &sc::Status::getHook)
        .def_static("setHook", &sc::Status::setHook);

    // ************************************ PMA *********************************************************

    py::class_<pma::MemoryResource>(rlpy, "MemoryResource")
        .def("allocate", &pma::MemoryResource::allocate)
        .def("deallocate", &pma::MemoryResource::deallocate);

    py::class_<pma::DefaultMemoryResource, pma::MemoryResource>(rlpy, "DefaultMemoryResource").def(py::init<>());

    // ************************************ TRIO ********************************************************

    auto pyAccessMode = py::enum_<trio::AccessMode>(rlpy, "AccessMode")
                            .value("Read", trio::AccessMode::Read)
                            .value("Write", trio::AccessMode::Write)
                            .value("ReadWrite", trio::AccessMode::ReadWrite)
                            .export_values();

    auto pyOpenMode =
        py::enum_<trio::OpenMode>(rlpy, "OpenMode").value("Binary", trio::OpenMode::Binary).value("Text", trio::OpenMode::Text).export_values();

    py::class_<trio::Openable, std::unique_ptr<trio::Openable, py::nodelete>>(rlpy, "Openable").def("open", &trio::Openable::open);
    py::class_<trio::Closeable, std::unique_ptr<trio::Closeable, py::nodelete>>(rlpy, "Closeable").def("close", &trio::Closeable::close);
    py::class_<trio::Controllable, trio::Openable, trio::Closeable, std::unique_ptr<trio::Controllable, py::nodelete>>(rlpy, "Controllable");
    py::class_<trio::Readable, std::unique_ptr<trio::Readable, py::nodelete>>(rlpy, "Readable")
        .def("read", static_cast<std::size_t (trio::Readable::*)(char*, std::size_t)>(&trio::Readable::read))
        .def("read", static_cast<std::size_t (trio::Readable::*)(trio::Writable*, std::size_t)>(&trio::Readable::read));
    py::class_<trio::Writable, std::unique_ptr<trio::Writable, py::nodelete>>(rlpy, "Writable")
        .def("write", static_cast<std::size_t (trio::Writable::*)(char const*, std::size_t)>(&trio::Writable::write))
        .def("write", static_cast<std::size_t (trio::Writable::*)(trio::Readable*, std::size_t)>(&trio::Writable::write));
    py::class_<trio::Seekable, std::unique_ptr<trio::Seekable, py::nodelete>>(rlpy, "Seekable").def("seek", &trio::Seekable::seek);
    py::class_<trio::Bounded, std::unique_ptr<trio::Bounded, py::nodelete>>(rlpy, "Bounded").def("size", &trio::Bounded::size);

    auto pyBoundedIOStream =
        py::class_<trio::BoundedIOStream, trio::Controllable, trio::Readable, trio::Writable, trio::Seekable, trio::Bounded>(
            rlpy, "BoundedIOStream");
    pyBoundedIOStream.attr("AccessMode") = pyAccessMode;
    pyBoundedIOStream.attr("OpenMode") = pyOpenMode;

    // ************************************ DNA *********************************************************

    py::enum_<dna::DataLayer>(rlpy, "DataLayer")
        .value("Descriptor", dna::DataLayer::Descriptor)
        .value("Definition", dna::DataLayer::Definition)
        .value("Behavior", dna::DataLayer::Behavior)
        .value("Geometry", dna::DataLayer::Geometry)
        .value("GeometryWithoutBlendShapes", dna::DataLayer::GeometryWithoutBlendShapes)
        .value("MachineLearnedBehavior", dna::DataLayer::MachineLearnedBehavior)
        .value("RBFBehavior", dna::DataLayer::RBFBehavior)
        .value("JointBehaviorMetadata", dna::DataLayer::JointBehaviorMetadata)
        .value("TwistSwingBehavior", dna::DataLayer::TwistSwingBehavior)
        .value("All", dna::DataLayer::All)
        .export_values();

    py::enum_<dna::UnknownLayerPolicy>(rlpy, "UnknownLayerPolicy")
        .value("Preserve", dna::UnknownLayerPolicy::Preserve)
        .value("Ignore", dna::UnknownLayerPolicy::Ignore)
        .export_values();

    py::class_<dna::HeaderReader, std::unique_ptr<dna::HeaderReader, py::nodelete>>(rlpy, "HeaderReader")
        .def("getFileFormatGeneration", &dna::HeaderReader::getFileFormatGeneration)
        .def("getFileFormatVersion", &dna::HeaderReader::getFileFormatVersion);

    py::class_<dna::HeaderWriter, std::unique_ptr<dna::HeaderWriter, py::nodelete>>(rlpy, "HeaderWriter")
        .def("setFileFormatGeneration", &dna::HeaderWriter::setFileFormatGeneration)
        .def("setFileFormatVersion", &dna::HeaderWriter::setFileFormatVersion);

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
        .def("getDBName", &dna::DescriptorReader::getDBName);

    py::class_<dna::DescriptorWriter, dna::HeaderWriter, std::unique_ptr<dna::DescriptorWriter, py::nodelete>>(rlpy, "DescriptorWriter")
        .def("setName", &dna::DescriptorWriter::setName)
        .def("setArchetype", &dna::DescriptorWriter::setArchetype)
        .def("setGender", &dna::DescriptorWriter::setGender)
        .def("setAge", &dna::DescriptorWriter::setAge)
        .def("clearMetaData", &dna::DescriptorWriter::clearMetaData)
        .def("setMetaData", &dna::DescriptorWriter::setMetaData)
        .def("setTranslationUnit", &dna::DescriptorWriter::setTranslationUnit)
        .def("setRotationUnit", &dna::DescriptorWriter::setRotationUnit)
        .def("setCoordinateSystem", &dna::DescriptorWriter::setCoordinateSystem)
        .def("setLODCount", &dna::DescriptorWriter::setLODCount)
        .def("setDBMaxLOD", &dna::DescriptorWriter::setDBMaxLOD)
        .def("setDBComplexity", &dna::DescriptorWriter::setDBComplexity)
        .def("setDBName", &dna::DescriptorWriter::setDBName);

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
        .def("getNeutralJointRotationZs", &dna::DefinitionReader::getNeutralJointRotationZs);

    py::class_<dna::DefinitionWriter, dna::DescriptorWriter, std::unique_ptr<dna::DefinitionWriter, py::nodelete>>(rlpy, "DefinitionWriter")
        .def("clearGUIControlNames", &dna::DefinitionWriter::clearGUIControlNames)
        .def("setGUIControlName", &dna::DefinitionWriter::setGUIControlName)
        .def("clearRawControlNames", &dna::DefinitionWriter::clearRawControlNames)
        .def("setRawControlName", &dna::DefinitionWriter::setRawControlName)
        .def("clearJointNames", &dna::DefinitionWriter::clearJointNames)
        .def("setJointName", &dna::DefinitionWriter::setJointName)
        .def("clearJointIndices", &dna::DefinitionWriter::clearJointIndices)
        .def("setJointIndices", &dna::DefinitionWriter::setJointIndices)
        .def("clearLODJointMappings", &dna::DefinitionWriter::clearLODJointMappings)
        .def("setLODJointMapping", &dna::DefinitionWriter::setLODJointMapping)
        .def("clearBlendShapeChannelNames", &dna::DefinitionWriter::clearBlendShapeChannelNames)
        .def("setBlendShapeChannelName", &dna::DefinitionWriter::setBlendShapeChannelName)
        .def("clearBlendShapeChannelIndices", &dna::DefinitionWriter::clearBlendShapeChannelIndices)
        .def("setBlendShapeChannelIndices", &dna::DefinitionWriter::setBlendShapeChannelIndices)
        .def("clearLODBlendShapeChannelMappings", &dna::DefinitionWriter::clearLODBlendShapeChannelMappings)
        .def("setLODBlendShapeChannelMapping", &dna::DefinitionWriter::setLODBlendShapeChannelMapping)
        .def("clearAnimatedMapNames", &dna::DefinitionWriter::clearAnimatedMapNames)
        .def("setAnimatedMapName", &dna::DefinitionWriter::setAnimatedMapName)
        .def("clearAnimatedMapIndices", &dna::DefinitionWriter::clearAnimatedMapIndices)
        .def("setAnimatedMapIndices", &dna::DefinitionWriter::setAnimatedMapIndices)
        .def("clearLODAnimatedMapMappings", &dna::DefinitionWriter::clearLODAnimatedMapMappings)
        .def("setLODAnimatedMapMapping", &dna::DefinitionWriter::setLODAnimatedMapMapping)
        .def("clearMeshNames", &dna::DefinitionWriter::clearMeshNames)
        .def("setMeshName", &dna::DefinitionWriter::setMeshName)
        .def("clearMeshIndices", &dna::DefinitionWriter::clearMeshIndices)
        .def("setMeshIndices", &dna::DefinitionWriter::setMeshIndices)
        .def("clearLODMeshMappings", &dna::DefinitionWriter::clearLODMeshMappings)
        .def("setLODMeshMapping", &dna::DefinitionWriter::setLODMeshMapping)
        .def("clearMeshBlendShapeChannelMappings", &dna::DefinitionWriter::clearMeshBlendShapeChannelMappings)
        .def("setMeshBlendShapeChannelMapping", &dna::DefinitionWriter::setMeshBlendShapeChannelMapping)
        .def("setJointHierarchy", &dna::DefinitionWriter::setJointHierarchy)
        .def("setNeutralJointTranslations", &dna::DefinitionWriter::setNeutralJointTranslations)
        .def("setNeutralJointRotations", &dna::DefinitionWriter::setNeutralJointRotations);

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
        .def("getAnimatedMapCutValues", &dna::BehaviorReader::getAnimatedMapCutValues);

    py::class_<dna::BehaviorWriter, dna::DefinitionWriter, std::unique_ptr<dna::BehaviorWriter, py::nodelete>>(rlpy, "BehaviorWriter")
        .def("setGUIToRawInputIndices", &dna::BehaviorWriter::setGUIToRawInputIndices)
        .def("setGUIToRawOutputIndices", &dna::BehaviorWriter::setGUIToRawOutputIndices)
        .def("setGUIToRawFromValues", &dna::BehaviorWriter::setGUIToRawFromValues)
        .def("setGUIToRawToValues", &dna::BehaviorWriter::setGUIToRawToValues)
        .def("setGUIToRawSlopeValues", &dna::BehaviorWriter::setGUIToRawSlopeValues)
        .def("setGUIToRawCutValues", &dna::BehaviorWriter::setGUIToRawCutValues)
        .def("setPSDCount", &dna::BehaviorWriter::setPSDCount)
        .def("setPSDRowIndices", &dna::BehaviorWriter::setPSDRowIndices)
        .def("setPSDColumnIndices", &dna::BehaviorWriter::setPSDColumnIndices)
        .def("setPSDValues", &dna::BehaviorWriter::setPSDValues)
        .def("setJointRowCount", &dna::BehaviorWriter::setJointRowCount)
        .def("setJointColumnCount", &dna::BehaviorWriter::setJointColumnCount)
        .def("clearJointGroups", &dna::BehaviorWriter::clearJointGroups)
        .def("deleteJointGroup", &dna::BehaviorWriter::deleteJointGroup)
        .def("setJointGroupLODs", &dna::BehaviorWriter::setJointGroupLODs)
        .def("setJointGroupInputIndices", &dna::BehaviorWriter::setJointGroupInputIndices)
        .def("setJointGroupOutputIndices", &dna::BehaviorWriter::setJointGroupOutputIndices)
        .def("setJointGroupValues", &dna::BehaviorWriter::setJointGroupValues)
        .def("setJointGroupJointIndices", &dna::BehaviorWriter::setJointGroupJointIndices)
        .def("setBlendShapeChannelLODs", &dna::BehaviorWriter::setBlendShapeChannelLODs)
        .def("setBlendShapeChannelInputIndices", &dna::BehaviorWriter::setBlendShapeChannelInputIndices)
        .def("setBlendShapeChannelOutputIndices", &dna::BehaviorWriter::setBlendShapeChannelOutputIndices)
        .def("setAnimatedMapLODs", &dna::BehaviorWriter::setAnimatedMapLODs)
        .def("setAnimatedMapInputIndices", &dna::BehaviorWriter::setAnimatedMapInputIndices)
        .def("setAnimatedMapOutputIndices", &dna::BehaviorWriter::setAnimatedMapOutputIndices)
        .def("setAnimatedMapFromValues", &dna::BehaviorWriter::setAnimatedMapFromValues)
        .def("setAnimatedMapToValues", &dna::BehaviorWriter::setAnimatedMapToValues)
        .def("setAnimatedMapSlopeValues", &dna::BehaviorWriter::setAnimatedMapSlopeValues)
        .def("setAnimatedMapCutValues", &dna::BehaviorWriter::setAnimatedMapCutValues);

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
        .def("getRBFSolverTwistAxis", &dna::RBFBehaviorReader::getRBFSolverTwistAxis);

    py::class_<dna::RBFBehaviorWriter, dna::BehaviorWriter, std::unique_ptr<dna::RBFBehaviorWriter, py::nodelete>>(rlpy, "RBFBehaviorWriter")
        .def("clearRBFPoses", &dna::RBFBehaviorWriter::clearRBFPoses)
        .def("setRBFPoseName", &dna::RBFBehaviorWriter::setRBFPoseName)
        .def("setRBFPoseScale", &dna::RBFBehaviorWriter::setRBFPoseScale)
        .def("clearRBFPoseControlNames", &dna::RBFBehaviorWriter::clearRBFPoseControlNames)
        .def("setRBFPoseControlName", &dna::RBFBehaviorWriter::setRBFPoseControlName)
        .def("setRBFPoseInputControlIndices", &dna::RBFBehaviorWriter::setRBFPoseInputControlIndices)
        .def("setRBFPoseOutputControlIndices", &dna::RBFBehaviorWriter::setRBFPoseOutputControlIndices)
        .def("setRBFPoseOutputControlWeights", &dna::RBFBehaviorWriter::setRBFPoseOutputControlWeights)
        .def("clearRBFSolvers", &dna::RBFBehaviorWriter::clearRBFSolvers)
        .def("clearRBFSolverIndices", &dna::RBFBehaviorWriter::clearRBFSolverIndices)
        .def("setRBFSolverIndices", &dna::RBFBehaviorWriter::setRBFSolverIndices)
        .def("setLODRBFSolverMapping", &dna::RBFBehaviorWriter::setLODRBFSolverMapping)
        .def("setRBFSolverName", &dna::RBFBehaviorWriter::setRBFSolverName)
        .def("setRBFSolverRawControlIndices", &dna::RBFBehaviorWriter::setRBFSolverRawControlIndices)
        .def("setRBFSolverPoseIndices", &dna::RBFBehaviorWriter::setRBFSolverPoseIndices)
        .def("setRBFSolverRawControlValues", &dna::RBFBehaviorWriter::setRBFSolverRawControlValues)
        .def("setRBFSolverType", &dna::RBFBehaviorWriter::setRBFSolverType)
        .def("setRBFSolverRadius", &dna::RBFBehaviorWriter::setRBFSolverRadius)
        .def("setRBFSolverAutomaticRadius", &dna::RBFBehaviorWriter::setRBFSolverAutomaticRadius)
        .def("setRBFSolverWeightThreshold", &dna::RBFBehaviorWriter::setRBFSolverWeightThreshold)
        .def("setRBFSolverDistanceMethod", &dna::RBFBehaviorWriter::setRBFSolverDistanceMethod)
        .def("setRBFSolverNormalizeMethod", &dna::RBFBehaviorWriter::setRBFSolverNormalizeMethod)
        .def("setRBFSolverFunctionType", &dna::RBFBehaviorWriter::setRBFSolverFunctionType)
        .def("setRBFSolverTwistAxis", &dna::RBFBehaviorWriter::setRBFSolverTwistAxis);

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
        .def("getBlendShapeTargetVertexIndices", &dna::GeometryReader::getBlendShapeTargetVertexIndices);

    py::class_<dna::GeometryWriter, dna::DefinitionWriter, std::unique_ptr<dna::GeometryWriter, py::nodelete>>(rlpy, "GeometryWriter")
        .def("clearMeshes", &dna::GeometryWriter::clearMeshes)
        .def("deleteMesh", &dna::GeometryWriter::deleteMesh)
        .def("setVertexPositions", &dna::GeometryWriter::setVertexPositions)
        .def("setVertexTextureCoordinates", &dna::GeometryWriter::setVertexTextureCoordinates)
        .def("setVertexNormals", &dna::GeometryWriter::setVertexNormals)
        .def("setVertexLayouts", &dna::GeometryWriter::setVertexLayouts)
        .def("clearFaceVertexLayoutIndices", &dna::GeometryWriter::clearFaceVertexLayoutIndices)
        .def("setFaceVertexLayoutIndices", &dna::GeometryWriter::setFaceVertexLayoutIndices)
        .def("setMaximumInfluencePerVertex", &dna::GeometryWriter::setMaximumInfluencePerVertex)
        .def("clearSkinWeights", &dna::GeometryWriter::clearSkinWeights)
        .def("setSkinWeightsValues", &dna::GeometryWriter::setSkinWeightsValues)
        .def("setSkinWeightsJointIndices", &dna::GeometryWriter::setSkinWeightsJointIndices)
        .def("clearBlendShapeTargets", &dna::GeometryWriter::clearBlendShapeTargets)
        .def("setBlendShapeChannelIndex", &dna::GeometryWriter::setBlendShapeChannelIndex)
        .def("setBlendShapeTargetDeltas", &dna::GeometryWriter::setBlendShapeTargetDeltas)
        .def("setBlendShapeTargetVertexIndices", &dna::GeometryWriter::setBlendShapeTargetVertexIndices);

    py::class_<dna::MachineLearnedBehaviorReader, dna::DefinitionReader, std::unique_ptr<dna::MachineLearnedBehaviorReader, py::nodelete>>(
        rlpy, "MachineLearnedBehaviorReader")
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
        .def(
            "getNeuralNetworkLayerActivationFunctionParameters",
            &dna::MachineLearnedBehaviorReader::getNeuralNetworkLayerActivationFunctionParameters)
        .def("getNeuralNetworkLayerBiases", &dna::MachineLearnedBehaviorReader::getNeuralNetworkLayerBiases)
        .def("getNeuralNetworkLayerWeights", &dna::MachineLearnedBehaviorReader::getNeuralNetworkLayerWeights);

    py::class_<dna::MachineLearnedBehaviorWriter, dna::DefinitionWriter, std::unique_ptr<dna::MachineLearnedBehaviorWriter, py::nodelete>>(
        rlpy, "MachineLearnedBehaviorWriter")
        .def("clearMLControlNames", &dna::MachineLearnedBehaviorWriter::clearMLControlNames)
        .def("setMLControlName", &dna::MachineLearnedBehaviorWriter::setMLControlName)
        .def("clearNeuralNetworks", &dna::MachineLearnedBehaviorWriter::clearNeuralNetworks)
        .def("clearNeuralNetworkIndices", &dna::MachineLearnedBehaviorWriter::clearNeuralNetworkIndices)
        .def("setNeuralNetworkIndices", &dna::MachineLearnedBehaviorWriter::setNeuralNetworkIndices)
        .def("clearLODNeuralNetworkMappings", &dna::MachineLearnedBehaviorWriter::clearLODNeuralNetworkMappings)
        .def("setLODNeuralNetworkMapping", &dna::MachineLearnedBehaviorWriter::setLODNeuralNetworkMapping)
        .def(
            "clearMeshRegionNames",
            static_cast<void (dna::MachineLearnedBehaviorWriter::*)()>(&dna::MachineLearnedBehaviorWriter::clearMeshRegionNames))
        .def(
            "clearMeshRegionNames",
            static_cast<void (dna::MachineLearnedBehaviorWriter::*)(std::uint16_t)>(&dna::MachineLearnedBehaviorWriter::clearMeshRegionNames))
        .def("setMeshRegionName", &dna::MachineLearnedBehaviorWriter::setMeshRegionName)
        .def("clearNeuralNetworkIndicesPerMeshRegion", &dna::MachineLearnedBehaviorWriter::clearNeuralNetworkIndicesPerMeshRegion)
        .def("setNeuralNetworkIndicesForMeshRegion", &dna::MachineLearnedBehaviorWriter::setNeuralNetworkIndicesForMeshRegion)
        .def("deleteNeuralNetwork", &dna::MachineLearnedBehaviorWriter::deleteNeuralNetwork)
        .def("setNeuralNetworkInputIndices", &dna::MachineLearnedBehaviorWriter::setNeuralNetworkInputIndices)
        .def("setNeuralNetworkOutputIndices", &dna::MachineLearnedBehaviorWriter::setNeuralNetworkOutputIndices)
        .def("clearNeuralNetworkLayers", &dna::MachineLearnedBehaviorWriter::clearNeuralNetworkLayers)
        .def("setNeuralNetworkLayerActivationFunction", &dna::MachineLearnedBehaviorWriter::setNeuralNetworkLayerActivationFunction)
        .def(
            "setNeuralNetworkLayerActivationFunctionParameters",
            &dna::MachineLearnedBehaviorWriter::setNeuralNetworkLayerActivationFunctionParameters)
        .def("setNeuralNetworkLayerBiases", &dna::MachineLearnedBehaviorWriter::setNeuralNetworkLayerBiases)
        .def("setNeuralNetworkLayerWeights", &dna::MachineLearnedBehaviorWriter::setNeuralNetworkLayerWeights);

    py::class_<dna::JointBehaviorMetadataReader, dna::DefinitionReader, std::unique_ptr<dna::JointBehaviorMetadataReader, py::nodelete>>(
        rlpy, "JointBehaviorMetadataReader")
        .def("getJointTranslationRepresentation", &dna::JointBehaviorMetadataReader::getJointTranslationRepresentation)
        .def("getJointRotationRepresentation", &dna::JointBehaviorMetadataReader::getJointRotationRepresentation)
        .def("getJointScaleRepresentation", &dna::JointBehaviorMetadataReader::getJointScaleRepresentation);

    py::class_<dna::JointBehaviorMetadataWriter, dna::DefinitionWriter, std::unique_ptr<dna::JointBehaviorMetadataWriter, py::nodelete>>(
        rlpy, "JointBehaviorMetadataWriter")
        .def("clearJointRepresentations", &dna::JointBehaviorMetadataWriter::clearJointRepresentations)
        .def("setJointTranslationRepresentation", &dna::JointBehaviorMetadataWriter::setJointTranslationRepresentation)
        .def("setJointRotationRepresentation", &dna::JointBehaviorMetadataWriter::setJointRotationRepresentation)
        .def("setJointScaleRepresentation", &dna::JointBehaviorMetadataWriter::setJointScaleRepresentation);

    py::class_<dna::TwistSwingBehaviorReader, dna::DefinitionReader, std::unique_ptr<dna::TwistSwingBehaviorReader, py::nodelete>>(
        rlpy, "TwistSwingBehaviorReader")
        .def("getTwistCount", &dna::TwistSwingBehaviorReader::getTwistCount)
        .def("getTwistSetupTwistAxis", &dna::TwistSwingBehaviorReader::getTwistSetupTwistAxis)
        .def("getTwistInputControlIndices", &dna::TwistSwingBehaviorReader::getTwistInputControlIndices)
        .def("getTwistOutputJointIndices", &dna::TwistSwingBehaviorReader::getTwistOutputJointIndices)
        .def("getTwistBlendWeights", &dna::TwistSwingBehaviorReader::getTwistBlendWeights)
        .def("getSwingCount", &dna::TwistSwingBehaviorReader::getSwingCount)
        .def("getSwingSetupTwistAxis", &dna::TwistSwingBehaviorReader::getSwingSetupTwistAxis)
        .def("getSwingInputControlIndices", &dna::TwistSwingBehaviorReader::getSwingInputControlIndices)
        .def("getSwingOutputJointIndices", &dna::TwistSwingBehaviorReader::getSwingOutputJointIndices)
        .def("getSwingBlendWeights", &dna::TwistSwingBehaviorReader::getSwingBlendWeights);

    py::class_<dna::TwistSwingBehaviorWriter, dna::DefinitionWriter, std::unique_ptr<dna::TwistSwingBehaviorWriter, py::nodelete>>(
        rlpy, "TwistSwingBehaviorWriter")
        .def("clearTwists", &dna::TwistSwingBehaviorWriter::clearTwists)
        .def("deleteTwist", &dna::TwistSwingBehaviorWriter::deleteTwist)
        .def("setTwistSetupTwistAxis", &dna::TwistSwingBehaviorWriter::setTwistSetupTwistAxis)
        .def("setTwistInputControlIndices", &dna::TwistSwingBehaviorWriter::setTwistInputControlIndices)
        .def("setTwistOutputJointIndices", &dna::TwistSwingBehaviorWriter::setTwistOutputJointIndices)
        .def("setTwistBlendWeights", &dna::TwistSwingBehaviorWriter::setTwistBlendWeights)
        .def("clearSwings", &dna::TwistSwingBehaviorWriter::clearSwings)
        .def("deleteSwing", &dna::TwistSwingBehaviorWriter::deleteSwing)
        .def("setSwingSetupTwistAxis", &dna::TwistSwingBehaviorWriter::setSwingSetupTwistAxis)
        .def("setSwingInputControlIndices", &dna::TwistSwingBehaviorWriter::setSwingInputControlIndices)
        .def("setSwingOutputJointIndices", &dna::TwistSwingBehaviorWriter::setSwingOutputJointIndices)
        .def("setSwingBlendWeights", &dna::TwistSwingBehaviorWriter::setSwingBlendWeights);

    py::class_<
        dna::Reader,
        dna::RBFBehaviorReader,
        dna::GeometryReader,
        dna::MachineLearnedBehaviorReader,
        dna::JointBehaviorMetadataReader,
        dna::TwistSwingBehaviorReader,
        std::unique_ptr<dna::Reader, py::nodelete>>(rlpy, "Reader");

    py::class_<
        dna::Writer,
        dna::RBFBehaviorWriter,
        dna::GeometryWriter,
        dna::MachineLearnedBehaviorWriter,
        dna::JointBehaviorMetadataWriter,
        dna::TwistSwingBehaviorWriter,
        std::unique_ptr<dna::Writer, py::nodelete>>(rlpy, "Writer")
        .def("setFrom", &dna::Writer::setFrom);

    py::class_<dna::StreamReader, dna::Reader>(rlpy, "StreamReader").def("read", &dna::StreamReader::read);

    py::class_<dna::StreamWriter, dna::Writer>(rlpy, "StreamWriter").def("write", &dna::StreamWriter::write);

    py::class_<dna::BinaryStreamReader, dna::StreamReader, std::unique_ptr<dna::BinaryStreamReader, Deleter<dna::BinaryStreamReader>>>(
        rlpy, "BinaryStreamReader")
        .def(
            py::init(static_cast<
                     dna::BinaryStreamReader* (*)(trio::BoundedIOStream*, dna::DataLayer, dna::UnknownLayerPolicy, std::uint16_t, pma::MemoryResource*)>(
                &dna::BinaryStreamReader::create)),
            py::keep_alive<1, 2>(),
            py::arg("stream"),
            py::arg("layer") = dna::DataLayer::All,
            py::arg("policy") = dna::UnknownLayerPolicy::Preserve,
            py::arg("maxLOD") = 0u,
            py::arg("memRes") = (pma::MemoryResource*)nullptr)
        .def(
            py::init(static_cast<dna::BinaryStreamReader* (*)(trio::BoundedIOStream*,
                                                              dna::DataLayer,
                                                              dna::UnknownLayerPolicy,
                                                              std::uint16_t,
                                                              std::uint16_t,
                                                              pma::MemoryResource*)>(&dna::BinaryStreamReader::create)),
            py::keep_alive<1, 2>(),
            py::arg("stream"),
            py::arg("layer"),
            py::arg("policy"),
            py::arg("lods"),
            py::arg("lodCount"),
            py::arg("memRes") = (pma::MemoryResource*)nullptr);

    py::class_<dna::BinaryStreamWriter, dna::StreamWriter, std::unique_ptr<dna::BinaryStreamWriter, Deleter<dna::BinaryStreamWriter>>>(
        rlpy, "BinaryStreamWriter")
        .def(
            py::init(&dna::BinaryStreamWriter::create),
            py::keep_alive<1, 2>(),
            py::arg("stream"),
            py::arg("memRes") = (pma::MemoryResource*)nullptr);

    py::class_<dna::JSONStreamWriter, dna::StreamWriter, std::unique_ptr<dna::JSONStreamWriter, Deleter<dna::JSONStreamWriter>>>(
        rlpy, "JSONStreamWriter")
        .def(
            py::init(&dna::JSONStreamWriter::create),
            py::keep_alive<1, 2>(),
            py::arg("stream"),
            py::arg("indentWidth") = 4u,
            py::arg("memRes") = (pma::MemoryResource*)nullptr)
        .def(
            "setFrom",
            static_cast<void (dna::JSONStreamWriter::*)(
                const dna::BinaryStreamReader*, dna::DataLayer, dna::UnknownLayerPolicy, pma::MemoryResource*)>(
                &dna::JSONStreamWriter::setFrom),
            py::arg("source"),
            py::arg("layer") = dna::DataLayer::All,
            py::arg("policy") = dna::UnknownLayerPolicy::Preserve,
            py::arg("memRes") = (pma::MemoryResource*)nullptr)
        .def(
            "setFrom",
            static_cast<void (dna::JSONStreamWriter::*)(
                const dna::JSONStreamReader*, dna::DataLayer, dna::UnknownLayerPolicy, pma::MemoryResource*)>(&dna::JSONStreamWriter::setFrom),
            py::arg("source"),
            py::arg("layer") = dna::DataLayer::All,
            py::arg("policy") = dna::UnknownLayerPolicy::Preserve,
            py::arg("memRes") = (pma::MemoryResource*)nullptr);

    py::class_<dna::JSONStreamReader, dna::StreamReader, std::unique_ptr<dna::JSONStreamReader, Deleter<dna::JSONStreamReader>>>(
        rlpy, "JSONStreamReader")
        .def(
            py::init(&dna::JSONStreamReader::create),
            py::keep_alive<1, 2>(),
            py::arg("stream"),
            py::arg("memRes") = (pma::MemoryResource*)nullptr);

    py::class_<dna::FileStream, trio::BoundedIOStream, std::unique_ptr<dna::FileStream, Deleter<dna::FileStream>>>(rlpy, "FileStream")
        .def(
            py::init(&dna::FileStream::create),
            py::keep_alive<1, 5>(),
            py::arg("path"),
            py::arg("accessMode"),
            py::arg("openMode"),
            py::arg("memRes") = (pma::MemoryResource*)nullptr);

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

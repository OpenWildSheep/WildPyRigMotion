cmake_minimum_required(VERSION 3.13)

project(PyRigLogic)
set(CMAKE_CXX_STANDARD 11)

# Prevent in-source-tree builds
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)
# Create compilation database
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE BOOL "" FORCE)

# if(MSVC)
#     set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4 /WX /wd\"4996\" /wd\"4273\" /wd\"4324\" /wd\"4251\" /wd\"4625\" /wd\"4626\" /wd\"4820\" /wd\"4710\" /wd\"4711\" /wd\"4714\" /wd\"4514\" /wd\"4774\" /wd\"4571\" /wd\"4868\" /wd\"5026\" /wd\"5027\" /wd\"4623\"")
# else()
#     set(CMAKE_CXX_FLAGS  "-fPIC")
# endif()

include(FetchContent)

FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11
    GIT_TAG v2.13.6
)

FetchContent_MakeAvailable(pybind11)

set(RIGLOGIC_SOURCE_DIR "" CACHE PATH "Path to Unreal Engine RigLogic. Should look like this : UE_5.5/Engine/Plugins/Animation/RigLogic/Source/RigLogicLib")

if(NOT EXISTS "${RIGLOGIC_SOURCE_DIR}")
    message(FATAL_ERROR "RIGLOGIC_SOURCE_DIR is not set or does not exist!")
endif()

file(GLOB_RECURSE RIGLOGIC_SOURCES "${RIGLOGIC_SOURCE_DIR}/**.cpp" "${RIGLOGIC_SOURCE_DIR}/**.h" "${RIGLOGIC_SOURCE_DIR}/**.hpp")
list(FILTER RIGLOGIC_SOURCES EXCLUDE REGEX ".*/Private/RigLogicLib\.cpp")
add_library(RigLogic STATIC ${RIGLOGIC_SOURCES})
target_include_directories(RigLogic PUBLIC ${RIGLOGIC_SOURCE_DIR}/Public)
target_include_directories(RigLogic PRIVATE ${RIGLOGIC_SOURCE_DIR}/Private)
target_compile_definitions(RigLogic PRIVATE RL_BUILD_WITH_XYZ_ROTATION_ORDER)
target_compile_definitions(RigLogic PRIVATE RL_BUILD_WITH_XZY_ROTATION_ORDER)
target_compile_definitions(RigLogic PRIVATE RL_BUILD_WITH_YXZ_ROTATION_ORDER)
target_compile_definitions(RigLogic PRIVATE RL_BUILD_WITH_YZX_ROTATION_ORDER)
target_compile_definitions(RigLogic PRIVATE RL_BUILD_WITH_ZXY_ROTATION_ORDER)
target_compile_definitions(RigLogic PRIVATE RL_BUILD_WITH_ZYX_ROTATION_ORDER)


file(GLOB PYRIGLOGIC_SOURCES "PyRigLogic/PyRigLogic.cpp")
add_library(PyRigLogic SHARED ${PYRIGLOGIC_SOURCES})
target_include_directories(PyRigLogic PRIVATE ${pybind11_SOURCE_DIR}/include)
target_include_directories(PyRigLogic PRIVATE ${PYTHON_INCLUDE_DIRS})
target_link_libraries(PyRigLogic PRIVATE RigLogic)
target_link_libraries(PyRigLogic PRIVATE ${PYTHON_LIBRARIES})
set_target_properties(PyRigLogic PROPERTIES SUFFIX ".pyd")
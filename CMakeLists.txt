cmake_minimum_required(VERSION 3.5)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_FLAGS "-Wall")

project(vulkan-playground)

file(GLOB_RECURSE vpg_SOURCES_HEADERS "src/*.cpp" "include/*.hpp")

set(GLSLang_LIBS ${GLSLang_LIBS} ${CMAKE_SOURCE_DIR}/external/glslang/build/glslang/libglslang.a)
set(GLSLang_LIBS ${GLSLang_LIBS} ${CMAKE_SOURCE_DIR}/external/glslang/build/SPIRV/libSPIRV.a)
set(GLSLang_LIBS ${GLSLang_LIBS} ${CMAKE_SOURCE_DIR}/external/glslang/build/OGLCompilersDLL/libOGLCompiler.a)
set(GLSLang_LIBS ${GLSLang_LIBS} ${CMAKE_SOURCE_DIR}/external/glslang/build/glslang/OSDependent/Unix/libOSDependent.a)
set(GLSLang_LIBS ${GLSLang_LIBS} pthread)
set(GLSLang_LIBS ${GLSLang_LIBS} ${CMAKE_SOURCE_DIR}/external/glslang/build/hlsl/libHLSL.a)

include_directories(include)
include_directories(external)

add_executable(vulkan-playground ${vpg_SOURCES_HEADERS})
target_link_libraries(vulkan-playground vulkan xcb ${GLSLang_LIBS})

# CMakeLists.txt for HALCON extension package 'halconuser' example
# Copyright (c) 2019-2023 by MVTec Software GmbH

# Option for using HALCON XL.
option(HALCON_XL "Use HALCON XL" OFF)
# Option for enabling .NET support. This will require a more recent version
# of CMake to work.
# .NET / CSharp support is only meaningful on Windows; default OFF elsewhere.
if(WIN32)
  option(HALCON_DOTNET "Build .NET support" ON)
else()
  option(HALCON_DOTNET "Build .NET support" OFF)
endif()
# If the HALCON_DOTNET option is selected, check if this version of CMake
# has the required CSharp support. Note that the Visual Studio 2010 or later
# generator has to be used.
if(HALCON_DOTNET)
  cmake_minimum_required(VERSION 4.1.1)
  set(_CSHARP CSharp)
else()
  cmake_minimum_required(VERSION 3.20)
endif()
# When using CMake 3.9 or newer, do not let RPATH affect the install_name on
# macOS.
if(POLICY CMP0068)
  cmake_policy(SET CMP0068 NEW)
endif()
# If policy CMP0091 is available, set it to NEW to enable use of the
# CMAKE_MSVC_RUNTIME_LIBRARY variable.
if(POLICY CMP0091)
  cmake_policy(SET CMP0091 NEW)
endif()
# Start the project here





project(Halcon_DaHeng C CXX )
set(ProjectName Halcon_DaHeng)
set(CMAKE_BUILD_TYPE Debug)
# 输出目录：Windows 输出到 bin/；Linux 直接输出到 lib/x64-linux
if(WIN32)
    # 禁止生成 Debug/Release 子目录，直接输出到 bin
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG "${CMAKE_SOURCE_DIR}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE "${CMAKE_SOURCE_DIR}/bin")
else()
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/x64-linux")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/x64-linux")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/lib/x64-linux")
endif()
# Append HALCON's ikcmake directory to the module path. This requires the
# HALCONEXAMPLES environment variable to be set.
file(TO_CMAKE_PATH "$ENV{HALCONEXAMPLES}" HALCONEXAMPLES)
list(APPEND CMAKE_MODULE_PATH "${HALCONEXAMPLES}/cmake")
# Include common settings for HALCON projects
include(UseHALCON)
# Use the HALCON_AddExtensionPackage function defined in UseHalcon to create
# a HALCON extension package. See the description of HALCON_AddExtensionPackage
# <HALCONEXAMPLES>/cmake/UseHALCON.cmake for details on the allowed parameters.
##这以上都能不能动


HALCON_AddExtensionPackage(Halcon_DaHeng
  DEF_FILES
    def/Halcon_DaHeng.def
  SOURCES
    source/Halcon_DaHeng.c
    source/Halcon_DaHeng.cpp
  CHAPTERS
    userextensions
  CLASSES
    IKap
  OPERATORS
   
)

##三方库包含
set(HALCON_ROOT $ENV{HALCONROOT})   # 转成 CMake 变量，后面用着方便
# Halcon 头文件目录
set(HALCON_INCLUDE_DIR ${HALCON_ROOT}/include/halconcpp)#HALCON_INCLUDE_DIR变量是${HALCON_ROOT}/include的路径

# 平台相关的 HALCON 与大恒 (DaHeng) SDK 库路径
if(WIN32)
    set(HALCON_LIB_DIR ${HALCON_ROOT}/lib/x64-win64)
    set(HALCON_CPP_LIB ${HALCON_LIB_DIR}/halconcpp.lib)
    set(DAHENG_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/daheng/inc)
    set(DAHENG_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/daheng/lib/x64)
    set(DAHENG_LIBS
        ${DAHENG_LIB_DIR}/GxIAPI.lib
        ${DAHENG_LIB_DIR}/DxImageProc.lib
    )
elseif(UNIX AND NOT APPLE)
    set(HALCON_LIB_DIR ${HALCON_ROOT}/lib/x64-linux)
    set(HALCON_CPP_LIB ${HALCON_LIB_DIR}/libhalconcpp.so)
    # Linux 大恒 SDK 头文件与 Windows 略有差异（如 GX_FRAME_CALLBACK_PARAM 字段）
    set(DAHENG_INC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/daheng/inc_linux)
    set(DAHENG_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rd/daheng/lib/x86_64)
    # Linux 大恒 SDK 仅提供 libgxiapi.so，无 DxImageProc（源码中未实际使用）
    set(DAHENG_LIBS
        ${DAHENG_LIB_DIR}/libgxiapi.so
    )
else()
    message(FATAL_ERROR "Unsupported platform for Halcon_DaHeng extension")
endif()

file(GLOB_RECURSE PROJECT_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")
target_sources(${ProjectName} PRIVATE ${PROJECT_HEADERS})
# 1. 收集 .def 文件
file(GLOB DEF_FILES "${CMAKE_CURRENT_SOURCE_DIR}/def/*.def")
# 2. 添加到目标，但不参与编译（通过 HEADER_FILE_ONLY 属性）
target_sources(${ProjectName} PRIVATE ${DEF_FILES})
# 3. 设置属性：只显示，不编译、不链接
set_source_files_properties(${DEF_FILES} PROPERTIES
    HEADER_FILE_ONLY TRUE   # 关键：不让它参与构建
)


target_include_directories(${ProjectName}
    PUBLIC
        $<BUILD_INTERFACE:${HALCON_INCLUDE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        #往后增加3方库的头文件目录
        $<BUILD_INTERFACE:${DAHENG_INC_DIR}>


)
target_link_libraries(${ProjectName}
    ${HALCON_CPP_LIB}
    #往后增加3方库的lib文件
    ${DAHENG_LIBS}
)


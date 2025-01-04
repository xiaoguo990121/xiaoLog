cmake_minimum_required(VERSION 3.10)
project(xiaoLog)

option(BUILD_SHARED_LIBS "Build xiaoLog as a shared lib" OFF)
option(BUILD_TESTING "Build tests" OFF)
option(USE_SPDLOG "Allow using the spdlog logging library" OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/)

set(XIAO_MAJOR_VERSION 0)
set(XIAO_MINOR_VERSION 0)
set(XIAO_PATCH_VERSION 1)
set(XIAO_VERSION ${XIAO_MAJOR_VERSION}.${XIAO_MINOR_VERSION}.${XIAO_PATCH_VERSION})

message(STATUS ${CMAKE_INSTALL_LIBDIR})

add_library(${PROJECT_NAME})
if(BUILD_SHARED_LIBS)
    list(
        FIND
        CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES
        "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}"
        isSystemDir
    )
    if("${isSystemDir}" STREQUAL "-1")
        set(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/${INSTALL_LIB_DIR}")
    endif("${isSystemDir}" STREQUAL "-1")

    set_target_properties(
        ${PROJECT_NAME}
        PROPERTIES VERSION
                    ${XIAO_VERSION}
                    SOVERSION
                    ${XIAO_MAJOR_VERSION}   
    )
endif(BUILD_SHARED_LIBS)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/exports/xiaoLog/exports.h)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)


set(XIAO_SOURCES
    src/LogStream.cpp
    src/LogStream.h
    src/Date.cpp
    src/Date.h
    src/Funcs.h
    src/NonCopyable.h
    src/Logger.cpp
    src/Logger.h
    src/AsyncFileLogger.h
    src/AsyncFileLogger.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}> $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/exports>
)
set(PROJECT_BASE_PATH ${PROJECT_SOURCE_DIR})
message(${PROJECT_BASE_PATH})

if(BUILD_TESTING)
    add_subdirectory(tests)
    find_package(GTest)
    if(GTest_FOUND)
        enable_testing()
        add_subdirectory(unittests)
    endif()
endif()

target_sources(
    ${PROJECT_NAME}
    PRIVATE ${XIAO_SOURCES}
            ${CMAKE_CURRENT_BINARY_DIR}/exports/xiaoLog/exports.h

)
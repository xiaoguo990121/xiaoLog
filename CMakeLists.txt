cmake_minimum_required(VERSION 3.10)
project(xiaoLog)

option(BUILD_SHARED_LIBS "Build xiaoLog as a shared lib" OFF)
option(BUILD_TESTING "Build tests" OFF)
option(USE_SPDLOG "Allow using the spdlog logging library" OFF)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/)

set(XIAOLOG_MAJOR_VERSION 0)
set(XIAOLOG_MINOR_VERSION 0)
set(XIAOLOG_PATCH_VERSION 1)
set(XIAOLOG_VERSION ${XIAOLOG_MAJOR_VERSION}.${XIAOLOG_MINOR_VERSION}.${XIAOLOG_PATCH_VERSION})

include(GNUInstallDirs)
set(INSTALL_BIN_DIR 
    ${CMAKE_INSTALL_BINDIR}
    CACHE PATH "Installation directory for binaries")
set(INSTALL_LIB_DIR
    ${CMAKE_INSTALL_LIBDIR}
    CACHE PATH "Installation directory for libraries")
set(INSTALL_INCLUDE_DIR
    ${CMAKE_INSTALL_INCLUDEDIR}
    CACHE PATH "Installation directory for header files")
set(DEF_INSTALL_XIAOLOG_CMAKE_DIR ${CMAKE_INSTALL_LIBDIR}/cmake/XiaoLog)
set(INSTALL_XIAOLOG_CMAKE_DIR 
    ${DEF_INSTALL_XIAOLOG_CMAKE_DIR}
    CACHE PATH "Installation directory for cmake files")


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
                    ${XIAOLOG_VERSION}
                    SOVERSION
                    ${XIAOLOG_MAJOR_VERSION}   
    )
endif(BUILD_SHARED_LIBS)

include(GenerateExportHeader)
generate_export_header(${PROJECT_NAME} EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/exports/xiaoLog/exports.h)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 20)
set_target_properties(${PROJECT_NAME} PROPERTIES EXPORT_NAME XiaoLog)


set(XIAOLOG_INCLUDE
    inc/xiaoLog/LogStream.h
    inc/xiaoLog/Date.h
    inc/xiaoLog/NonCopyable.h
    inc/xiaoLog/Logger.h
    inc/xiaoLog/AsyncFileLogger.h
    inc/xiaoLog/Funcs.h
)

set(XIAOLOG_SOURCES
    src/LogStream.cpp
    src/Date.cpp
    src/Logger.cpp
    src/AsyncFileLogger.cpp
)

target_include_directories(
    ${PROJECT_NAME}
    PUBLIC $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/inc> 
           $<INSTALL_INTERFACE:${INSTALL_INCLUDE_DIR}>
           $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/exports>
)
set(PROJECT_BASE_PATH ${PROJECT_SOURCE_DIR})

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
    PRIVATE ${XIAOLOG_SOURCES}
            ${XIAOLOG_INCLUDE}
            ${CMAKE_CURRENT_BINARY_DIR}/exports/xiaoLog/exports.h)

install(
    TARGETS xiaoLog
    EXPORT XiaoLogTargets
    RUNTIME DESTINATION "${INSTALL_BIN_DIR}" COMPONENT bin
    ARCHIVE DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
    LIBRARY DESTINATION "${INSTALL_LIB_DIR}" COMPONENT lib
)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/exports/xiaoLog/exports.h DESTINATION ${INSTALL_INCLUDE_DIR}/xiaoLog)
install(FILES ${XIAOLOG_INCLUDE} DESTINATION ${INSTALL_INCLUDE_DIR}/xiaoLog)

include(CMakePackageConfigHelpers)
configure_package_config_file(
    cmake/templates/XiaoLogConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/XiaoLogConfig.cmake
    INSTALL_DESTINATION ${INSTALL_XIAOLOG_CMAKE_DIR}
)

write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/XiaoLogConfigVersion.cmake
    VERSION ${XIAOLOG_VERSION}
    COMPATIBILITY SameMajorVersion
)

install(
    FILES "${CMAKE_CURRENT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/XiaoLogConfig.cmake"
          "${CMAKE_CURRENT_BINARY_DIR}/XiaoLogConfigVersion.cmake"
    DESTINATION "${INSTALL_XIAOLOG_CMAKE_DIR}"
    COMPONENT dev
)

install(
    EXPORT XiaoLogTargets
    DESTINATION "${INSTALL_XIAOLOG_CMAKE_DIR}"
    NAMESPACE XiaoLog::
    COMPONENT dev
)


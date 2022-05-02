cmake_minimum_required(VERSION 3.21)

########################################################################################################################
## Define project
########################################################################################################################
project(
        CommonLibSSESamplePlugin
        VERSION 1.0.0
        DESCRIPTION "A sample SKSE plugin built with CommonLibSSE."
        LANGUAGES CXX)
set(PROJECT_FRIENDLY_NAME "CommonLibSSE Sample Plugin")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/version.rc.in
        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        @ONLY)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}.manifest.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/${PROJECT_NAME}.manifest
        @ONLY)

configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/PluginInfo.h.in
        ${CMAKE_CURRENT_BINARY_DIR}/src/PluginInfo.h
        @ONLY)

set(headers
        include/Sample/Sample.h)

set(sources
        src/Config.cpp
        src/HitCounterManager.cpp
        src/Main.cpp
        src/Papyrus.cpp

        ${CMAKE_CURRENT_BINARY_DIR}/version.rc
        src/${PROJECT_NAME}.manifest)

set(tests
        test/HitCounterManager.cpp)

source_group(
        TREE ${CMAKE_CURRENT_SOURCE_DIR}
        FILES
        ${headers}
        ${sources}
        ${tests})

########################################################################################################################
## Add CMake features
########################################################################################################################
include(CheckIPOSupported)
include(GNUInstallDirs)
include(GoogleTest)

#########################################################################################################################
### Build options
#########################################################################################################################
message("Options:")
option(BUILD_TESTS "Build unit tests." OFF)
message("\tTests: ${BUILD_TESTS}")

########################################################################################################################
## Find dependencies
########################################################################################################################
find_package(CommonLibSSE REQUIRED)
find_package(ryml CONFIG REQUIRED)

find_path(ARTICUNO_INCLUDE_DIRS "articuno/articuno.h")

if (BUILD_TESTS)
    find_package(GTest CONFIG REQUIRED)
endif ()

########################################################################################################################
## Configuration for all targets
########################################################################################################################
if (MSVC)
    add_compile_definitions(
            UNICODE
            _UNICODE
            NOMINMAX
            _AMD64_
            WIN32_LEAN_AND_MEAN
            _CRT_USE_BUILTIN_OFFSETOF # Fixes MSVC being non-compliant with offsetof behavior by default.
    )

    if (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        add_compile_definitions(
                __cpp_lib_char8_t
                __cpp_consteval
                __cpp_lib_format)
    else ()
        add_compile_options(/Zc:preprocessor)
    endif ()
endif ()

check_ipo_supported(RESULT USE_IPO OUTPUT IPO_OUTPUT)
if (USE_IPO)
    message("Enabling interprocedural optimizations.")
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
else ()
    message("Interprocedural optimizations are not supported.")
endif ()

########################################################################################################################
## Configure target DLL
########################################################################################################################
add_library(${PROJECT_NAME} SHARED
        ${headers}
        ${sources})

add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_compile_features(${PROJECT_NAME}
        PUBLIC
        cxx_std_23)

target_compile_options(${PROJECT_NAME}
        PRIVATE
        /wd4117 # macro with reserved name; silences warning re" _CRT_USE_BUILTIN_OFFSETOF, needed for standard behavior
        )

target_compile_options(${PROJECT_NAME}
        PUBLIC
        "$<$<CONFIG:RELEASE>:/O2;/Ob2;/Gregcall;/Gw>")

target_include_directories(${PROJECT_NAME}
        PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/src>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/src>
        $<INSTALL_INTERFACE:src>
        ${ARTICUNO_INCLUDE_DIRS})

target_include_directories(${PROJECT_NAME}
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(${PROJECT_NAME}
        PRIVATE
        ryml::ryml)

target_link_libraries(${PROJECT_NAME}
        PUBLIC
        CommonLibSSE::CommonLibSSE
        spdlog::spdlog)

target_precompile_headers(${PROJECT_NAME}
        PRIVATE
        src/PCH.h)

install(DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/include/Sample"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

install(TARGETS ${PROJECT_NAME}
        DESTINATION "${CMAKE_INSTALL_LIBDIR}")

# Automatic deployment to FOMOD directory.
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(BUILD_NAME "Debug")
else ()
    set(BUILD_NAME "Release")
endif ()
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}/")
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/Plugin${BUILD_NAME}/")
file(GLOB_RECURSE OUTPUT_DLLS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.dll")
file(GLOB_RECURSE OUTPUT_PDBS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.pdb")
file(GLOB_RECURSE OUTPUT_SCRIPTS "${CMAKE_CURRENT_SOURCE_DIR}/contrib/Distribution/**/*.pex")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_DLLS}")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_PDBS}")
set_property(TARGET ${PROJECT_NAME}
        APPEND PROPERTY ADDITIONAL_CLEAN_FILES "${OUTPUT_SCRIPTS}")

# Automatic deployment to Mod Organizer 2 mod directory.
foreach (DEPLOY_TARGET $ENV{CommonLibSSESamplePluginTargets})
    message("Adding deployment target ${DEPLOY_TARGET}.")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}> "${DEPLOY_TARGET}/SKSE/Plugins/")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_PDB_FILE:${PROJECT_NAME}> "${DEPLOY_TARGET}/SKSE/Plugins/")
endforeach ()

########################################################################################################################
## Configure unit tests
########################################################################################################################
if (BUILD_TESTS)
    enable_testing()

    add_executable(
            ${PROJECT_NAME}Tests
            ${headers}
            ${tests})

    target_compile_features(${PROJECT_NAME}Tests
            PUBLIC
            cxx_std_23)

    target_link_libraries(
            ${PROJECT_NAME}Tests
            PRIVATE
            ${PROJECT_NAME}
            GTest::gmock
            GTest::gtest
            GTest::gmock_main
            GTest::gtest_main)

    target_precompile_headers(${PROJECT_NAME}Tests
            PRIVATE
            src/PCH.h)

    # Disabled pending fixes to CMake/Google Test to handle environment variables properly.
    gtest_discover_tests(${PROJECT_NAME}Tests DISCOVERY_MODE PRE_TEST)
endif ()

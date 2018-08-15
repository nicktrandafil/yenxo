cmake_minimum_required(VERSION 2.8)

project(rproject)

#
# Common
#

if(CMAKE_COMPILER_IS_GNUCXX)
    set(GENERIC_CXX_COMPILE_OPTIONS
        $<$<CONFIG:Debug>:-O0>
        $<$<NOT:$<CONFIG:Debug>>:-O2 -DNDEBUG>
        -Wall
    )
else()
    message(FATAL_ERROR "${CMAKE_CXX_COMPILER_ID} is not supported")
endif()

set(CMAKE_BUILD_TYPE Release CACHE BOOL "Enable testing")

#
# Core
#

add_library(
    ${PROJECT_NAME}_core

    include/rproject/meta.hpp

    include/rproject/pimpl.hpp
    include/rproject/pimpl_impl.hpp

    src/variant.cpp
    include/rproject/variant.hpp

    include/rproject/variant_mixins.hpp
    include/rproject/ostream_mixins.hpp
    include/rproject/comparison_mixins.hpp

    include/rproject/type_name.hpp
)

set_target_properties(${PROJECT_NAME}_core PROPERTIES CXX_STANDARD 17)

target_include_directories(
    ${PROJECT_NAME}_core

    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/rproject>

    INTERFACE
    $<INSTALL_INTERFACE:include>
)

target_compile_options(
    ${PROJECT_NAME}_core

    PRIVATE
    ${GENERIC_CXX_COMPILE_OPTIONS}
)

#
# App
#

add_executable(
    ${PROJECT_NAME} 
    src/main.cpp
)

target_compile_options(
    ${PROJECT_NAME}

    PRIVATE
    ${GENERIC_CXX_COMPILE_OPTIONS}
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)
target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_core)

set(TESTING ON CACHE BOOL "Enable testing")

#
# Testing
#

if(NOT TESTING)
    return()
endif()

enable_testing()

find_package(Catch2 QUIET REQUIRED)

add_executable(
    test_${PROJECT_NAME}_core

    test/meta.cpp
    test/variant.cpp

    test/main.cpp

    test/variant_mixins.cpp
    test/ostream_mixins.cpp
    test/comparison_mixins.cpp

    test/type_name.cpp
)

set_target_properties(test_${PROJECT_NAME}_core PROPERTIES CXX_STANDARD 17)

target_link_libraries(
    test_${PROJECT_NAME}_core
    PRIVATE

    Catch2::Catch2
    ${PROJECT_NAME}_core
)

target_compile_options(
    test_${PROJECT_NAME}_core

    PRIVATE
    ${GENERIC_CXX_COMPILE_OPTIONS}
)

#
# Test cases
#

add_test(
    check_variant test_${PROJECT_NAME}_core
    "Check Variant")

add_test(
    mixins_var_update_from_var test_${PROJECT_NAME}_core
    "[variant_mixin_helpers]")

add_test(
    variant_mixins test_${PROJECT_NAME}_core
    "Check mixin::Var and mixin::UpdateFromVar")

add_test(
    mixins_var_def test_${PROJECT_NAME}_core
    "Check mixin::VarDef")

add_test(
    mixins_var_def_explicit test_${PROJECT_NAME}_core
    "Check mixin::VarDefExplicit")

add_test(
    mixins_ostream test_${PROJECT_NAME}_core
    "Check mixin::OStream")

add_test(
    mixins_equality_comparison test_${PROJECT_NAME}_core
    "Check mixin::EqualityComparison")

add_test(
    type_name test_${PROJECT_NAME}_core
    "Check typeName")

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

set(CMAKE_BUILD_TYPE Release CACHE BOOL "Build type")

#
# 3rd party
#

find_package(RapidJSON QUIET REQUIRED)
find_package(Boost QUIET REQUIRED)

#
# Target
#

add_library(
    ${PROJECT_NAME}

    include/rproject/meta.hpp

    include/rproject/pimpl.hpp
    include/rproject/pimpl_impl.hpp

    src/variant.cpp
    include/rproject/variant.hpp

    include/rproject/variant_traits.hpp
    include/rproject/variant_conversion.hpp
    include/rproject/ostream_traits.hpp
    include/rproject/comparison_traits.hpp

    include/rproject/type_name.hpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

target_include_directories(
    ${PROJECT_NAME}

    PUBLIC
    $<BUILD_INTERFACE:
        ${CMAKE_CURRENT_SOURCE_DIR}/include/rproject
        ${RAPIDJSON_INCLUDE_DIRS}
    >
    ${Boost_INCLUDE_DIRS}

    INTERFACE
    $<INSTALL_INTERFACE:
        include
        ${RAPIDJSON_INCLUDE_DIRS}
    >
)

target_compile_options(
    ${PROJECT_NAME}

    PRIVATE
    ${GENERIC_CXX_COMPILE_OPTIONS}
)

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
    test_${PROJECT_NAME}

    test/meta.cpp
    test/variant.cpp

    test/main.cpp

    test/variant_traits.cpp
    test/variant_traits_non_intrusive.cpp
    test/ostream_traits.cpp
    test/comparison_traits.cpp

    test/type_name.cpp

    test/json_struct.cpp

    test/type_safe.cpp

    test/error_diagnostic.cpp
)

set_target_properties(test_${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

target_link_libraries(
    test_${PROJECT_NAME}
    PRIVATE

    Catch2::Catch2
    ${PROJECT_NAME}
)

target_compile_options(
    test_${PROJECT_NAME}

    PRIVATE
    ${GENERIC_CXX_COMPILE_OPTIONS}
)

#
# Test cases
#

add_test(
    check_variant test_${PROJECT_NAME}
    "Check Variant")

add_test(
    traits_var_update_from_var test_${PROJECT_NAME}
    "[variant_trait_helpers]")

add_test(
    variant_traits test_${PROJECT_NAME}
    "Check trait::Var and trait::UpdateFromVar")

add_test(
    traits_var_def test_${PROJECT_NAME}
    "Check trait::VarDef")

add_test(
    traits_var_def_explicit test_${PROJECT_NAME}
    "Check trait::VarDefExplicit")

add_test(
    traits_ostream test_${PROJECT_NAME}
    "Check trait::OStream")

add_test(
    traits_equality_comparison test_${PROJECT_NAME}
    "Check trait::EqualityComparison")

add_test(
    type_name test_${PROJECT_NAME}
    "Check typeName")

add_test(
    simple_json_to_struct test_${PROJECT_NAME}
    "Check simple json to struct")

add_test(
    traits_var_fails test_${PROJECT_NAME}
    "Check trait::Var fails")

add_test(
    traits_var_redefine test_${PROJECT_NAME}
    "Check trait::Var redefine")

add_test(
    traits_var_strong_typedef test_${PROJECT_NAME}
    "Check trait::Var with type_safe::strong_typedef")

add_test(
    traits_var_constraint_type test_${PROJECT_NAME}
    "Check trait::Var with type_safe::constraint_type")

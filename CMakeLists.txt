cmake_minimum_required(VERSION 2.8)

project(rproject)

#
# Targets
#

add_library(
    ${PROJECT_NAME}_core

    pimpl.hpp
    pimpl_impl.hpp

    variant.cpp
    variant.hpp

    variant_mixins.hpp
    ostream_mixins.hpp
    comparison_mixins.hpp

    type_name.hpp
)

set_target_properties(${PROJECT_NAME}_core PROPERTIES CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME}_core PUBLIC ${PROJECT_SOURCE_DIR})

add_executable(
    ${PROJECT_NAME} 
    main.cpp
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
    ${PROJECT_NAME}_core)

add_test(
    check_to_and_from test_${PROJECT_NAME}_core
    "Check to and from types")

add_test(
    mixins_var_update_from_var test_${PROJECT_NAME}_core
    "[variant_mixin_helpers]")

add_test(
    variant_mixins test_${PROJECT_NAME}_core
    "Check Var and UpdateFromVar")

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

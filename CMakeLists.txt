cmake_minimum_required(VERSION 2.8)
cmake_policy(SET CMP0048 NEW)
cmake_policy(SET CMP0022 NEW)

project(serialize VERSION 0.1)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(serialize)


# if inside subdirectory
if(CMAKE_CURRENT_SOURCE_DIR STREQUAL CMAKE_SOURCE_DIR)
    set(${PROJECT_NAME}_sub OFF)
else()
    set(${PROJECT_NAME}_sub ON)
endif()


# Options

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type" FORCE)
endif()

include(CMakeDependentOption)

option(${PROJECT_NAME}_enable_type_safe "Enable type_safe support" ON)
if(${PROJECT_NAME}_enable_type_safe)
    set(_${PROJECT_NAME}_enable_type_safe 1)
else()
    set(_${PROJECT_NAME}_enable_type_safe 0)
endif()

option(${PROJECT_NAME}_test_define_struct "Enable testing of DEFINE_STRUCT" OFF)

cmake_dependent_option(${PROJECT_NAME}_TESTING
  "Enable testing"
  ON "NOT ${PROJECT_NAME}_sub" OFF
)

cmake_dependent_option(${PROJECT_NAME}_COVERAGE
  "Enable binary instrumentation to collect test coverage information in the DEBUG configuration"
  ON "NOT ${PROJECT_NAME}_sub AND ${PROJECT_NAME}_TESTING AND ${CMAKE_BUILD_TYPE} STREQUAL Debug" OFF
)


# 3rd party

if(NOT ${PROJECT_NAME}_sub)
  find_package(RapidJSON QUIET REQUIRED)
  find_package(Boost 1.65 QUIET REQUIRED)
endif()

include(external/external.cmake)


# Target

add_library(
    ${PROJECT_NAME} STATIC

    include/${PROJECT_NAME}/meta.hpp
    include/${PROJECT_NAME}/preprocessor.hpp

    include/${PROJECT_NAME}/pimpl.hpp
    include/${PROJECT_NAME}/pimpl_impl.hpp

    include/${PROJECT_NAME}/variant.hpp
    include/${PROJECT_NAME}/variant_fwd.hpp

    include/${PROJECT_NAME}/variant_traits.hpp
    include/${PROJECT_NAME}/variant_conversion.hpp
    include/${PROJECT_NAME}/ostream_traits.hpp
    include/${PROJECT_NAME}/comparison_traits.hpp

    include/${PROJECT_NAME}/type_name.hpp

    include/${PROJECT_NAME}/string_conversion.hpp
    include/${PROJECT_NAME}/algorithm/string.hpp

    include/${PROJECT_NAME}/config.hpp

    include/${PROJECT_NAME}/enum_traits.hpp
    include/${PROJECT_NAME}/define_enum.hpp

    include/${PROJECT_NAME}/define_struct.hpp
    include/${PROJECT_NAME}/value_tag.hpp

    include/${PROJECT_NAME}/genuine_struct.hpp

    include/serialize.hpp

    src/variant.cpp
)

set_target_properties(${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

target_include_directories(${PROJECT_NAME}
    PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)

target_include_directories(${PROJECT_NAME}
    SYSTEM
    PUBLIC
    $<INSTALL_INTERFACE:include>
    ${Boost_INCLUDE_DIRS}
    ${RAPIDJSON_INCLUDE_DIRS}
)

target_compile_definitions(${PROJECT_NAME}
    PUBLIC
    BOOST_HANA_CONFIG_ENABLE_STRING_UDL
    SERIALIZE_ENABLE_TYPE_SAFE=${_${PROJECT_NAME}_enable_type_safe}
)

if(${PROJECT_NAME}_enable_type_safe)
    target_link_libraries(${PROJECT_NAME} PUBLIC type_safe)
endif()


# compile options/definitions
if(NOT ${PROJECT_NAME}_sub)
  target_link_libraries(${PROJECT_NAME} PRIVATE ${PROJECT_NAME}_development)
endif()


# Setup package config

install(DIRECTORY include/${PROJECT_NAME} DESTINATION include)

if(NOT ${PROJECT_NAME}_sub AND ${PROJECT_NAME}_all_imported_deps)
    include(CMakePackageConfigHelpers)
    set(CONFIG_PACKAGE_INSTALL_DIR lib/cmake/${PROJECT_NAME})

    write_basic_package_version_file(
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        COMPATIBILITY SameMajorVersion
    )

    install(TARGETS ${PROJECT_NAME} ${PROJECT_NAME}_development
        EXPORT ${PROJECT_NAME}-targets
        DESTINATION lib
    )

    install(EXPORT ${PROJECT_NAME}-targets
        DESTINATION ${CONFIG_PACKAGE_INSTALL_DIR}
    )

    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${PROJECT_NAME}-config.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        @ONLY
    )

    # because of nested @vars@
    configure_file(${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        @ONLY
    )

    install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake
        DESTINATION ${CONFIG_PACKAGE_INSTALL_DIR}
    )
else()
    install(TARGETS ${PROJECT_NAME} DESTINATION lib)
endif()

# Doc
add_subdirectory(doc)


# Testing

if(NOT ${PROJECT_NAME}_TESTING)
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
    test/string_conversion.cpp
    test/string.cpp

    test/variant_conversion.cpp

    test/define_enum.cpp

    $<$<BOOL:${${PROJECT_NAME}_test_define_struct}>:test/define_struct.cpp>
    $<$<BOOL:${${PROJECT_NAME}_test_define_struct}>:test/define_struct_part2.cpp>
    $<$<BOOL:${${PROJECT_NAME}_test_define_struct}>:test/define_struct_part3.cpp>
)

set_target_properties(test_${PROJECT_NAME} PROPERTIES CXX_STANDARD 17)

target_link_libraries(
    test_${PROJECT_NAME}
    PRIVATE

    Catch2::Catch2
    ${PROJECT_NAME}
    ${PROJECT_NAME}_development
)


# Test cases

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
    traits_var_nonintrusive test_${PROJECT_NAME}
    "Check trait::Var nonintrusive")

add_test(
    traits_var_strong_typedef test_${PROJECT_NAME}
    "Check trait::Var with type_safe::strong_typedef")

add_test(
    traits_var_constraint_type test_${PROJECT_NAME}
    "Check trait::Var with type_safe::constraint_type")

add_test(
    string_conversion test_${PROJECT_NAME}
    "Check toString/fromString")

add_test(
    iparse_with_suffix test_${PROJECT_NAME}
    "Check IparseWithSuffix")

add_test(
    algorithm_string test_${PROJECT_NAME}
    "Check Check iendsWith")

add_test(
    traits_var_type_safe test_${PROJECT_NAME}
    "Check trait::Var with type_safe build-in types")

add_test(
    variant_conversion test_${PROJECT_NAME}
    "Check toVariant/fromVariant")

add_test(
    define_enum test_${PROJECT_NAME}
    "Check DEFINE_ENUM")

add_test(
    define_struct test_${PROJECT_NAME}
    "[define_struct]")


# Coverage

if(${PROJECT_NAME}_COVERAGE)
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/cov.sh
        COMMAND ${CMAKE_COMMAND} -E copy ${PROJECT_SOURCE_DIR}/scripts/cov.sh ${CMAKE_BINARY_DIR}
        DEPENDS scripts/cov.sh)
    add_custom_target(${PROJECT_NAME}_cov ALL DEPENDS ${CMAKE_BINARY_DIR}/cov.sh)
endif()

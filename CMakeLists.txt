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
)

set_target_properties(test_${PROJECT_NAME}_core PROPERTIES CXX_STANDARD 17)
target_link_libraries(
    test_${PROJECT_NAME}_core
    PRIVATE
    Catch2::Catch
    ${PROJECT_NAME}_core)

set(${PROJECT_NAME}_all_imported_deps ON)


if(${PROJECT_NAME}_enable_type_safe)
    if(NOT type_safe_FOUND)
        find_package(type_safe QUIET)
    endif()

    if(NOT type_safe_FOUND)
        set(${PROJECT_NAME}_all_imported_deps OFF)
        if(TARGET type_safe)
            message(STATUS "Using inherited type_safe target")
        else()
            message(STATUS "Installing type_safe via submodule")
            add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/type_safe EXCLUDE_FROM_ALL)
        endif()
    endif()
endif()

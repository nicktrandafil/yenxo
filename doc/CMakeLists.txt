find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen was not found; the 'doc' and 'docset' targets "
                    "will be unavailable.")
    return()
endif()

configure_file(Doxyfile.in documentation.doxygen @ONLY)

add_custom_target(doc
    COMMAND ${CMAKE_COMMAND} -E remove_directory html
    COMMAND ${DOXYGEN_EXECUTABLE} documentation.doxygen
    COMMENT "Generating API documentation with Doxygen"
    VERBATIM
)

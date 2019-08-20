find_package(Doxygen)
if (NOT DOXYGEN_FOUND)
    message(WARNING "Doxygen was not found; the 'doc' and 'docset' targets "
                    "will be unavailable.")
    return()
endif()


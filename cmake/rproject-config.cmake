include(CMakeFindDependencyMacro)

find_dependency(type_safe)
find_dependency(Boost)
find_package(RapidJSON)

include(${CMAKE_CURRENT_LIST_DIR}/rproject-targets.cmake)

set(rproject_LIBRARY rproject)
set(rproject_LIBRARIES rproject)

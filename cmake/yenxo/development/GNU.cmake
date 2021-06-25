set(GNU_base_flags -Wall -Wextra -Wpedantic -Werror -ftemplate-backtrace-limit=0
    -Wno-error=maybe-uninitialized)
set(GNU_debug_flags -g -O0)
set(GNU_coverage_flags --coverage)
set(GNU_release_flags -O2 -DNDEBUG -Wno-unused-parameter)

string(CONCAT compile_options
  "${GNU_base_flags};"
  "$<$<CONFIG:DEBUG>:${GNU_debug_flags};>"
  "$<$<CONFIG:RELWITHDEBINFO>:${GNU_release_flags};>"
  "$<$<CONFIG:MINSIZEREL>:${GNU_release_flags};>"
  "$<$<CONFIG:RELEASE>:${GNU_release_flags};>"
  "$<$<BOOL:${${PROJECT_NAME}_COVERAGE}>:${GNU_coverage_flags};>")

string(CONCAT link_options
  "$<$<BOOL:${${PROJECT_NAME}_COVERAGE}>:${GNU_coverage_flags};>")

target_compile_options(${PROJECT_NAME}_development INTERFACE
  $<$<CXX_COMPILER_ID:GNU>:${compile_options}>)

target_link_options(${PROJECT_NAME}_development INTERFACE
  $<$<CXX_COMPILER_ID:GNU>:${link_options}>)

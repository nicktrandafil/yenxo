set(Clang_base_flags -Wall -Wextra -Wpedantic -Werror -ftemplate-backtrace-limit=0 -Wno-unneeded-internal-declaration)
set(Clang_debug_flags -g -O0)
set(Clang_coverage_flags --coverage)
set(Clang_release_flags -O2 -DNDEBUG -Wno-unused-parameter)

string(CONCAT compile_options
  "${Clang_base_flags};"
  "$<$<CONFIG:DEBUG>:${Clang_debug_flags};>"
  "$<$<CONFIG:RELWITHDEBINFO>:${Clang_release_flags};>"
  "$<$<CONFIG:MINSIZEREL>:${Clang_release_flags};>"
  "$<$<CONFIG:RELEASE>:${Clang_release_flags};>"
  "$<$<BOOL:${${PROJECT_NAME}_COVERAGE}>:${Clang_coverage_flags};>")

string(CONCAT link_options
  "$<$<BOOL:${${PROJECT_NAME}_COVERAGE}>:${Clang_coverage_flags};>")

target_compile_options(${PROJECT_NAME}_development INTERFACE
  $<$<CXX_COMPILER_ID:Clang>:${compile_options}>)

target_link_options(${PROJECT_NAME}_development INTERFACE
  $<$<CXX_COMPILER_ID:Clang>:${link_options}>)

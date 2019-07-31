set(serialize_Clang_base_flags -Wall -Wextra -Wpedantic -Werror -ftemplate-backtrace-limit=0)
set(serialize_Clang_debug_flags -g -O0)
set(serialize_Clang_coverage_flags --coverage)
set(serialize_Clang_release_flags -O2 -DNDEBUG -Wno-unused-parameter)

string(CONCAT generator
  "${serialize_Clang_base_flags};"
  "$<$<OR:$<CONFIG:DEBUG>,"
         "$<CONFIG:RELWITHDEBINFO>>:${serialize_Clang_debug_flags};>"
  "$<$<OR:$<CONFIG:RELEASE>,"
         "$<CONFIG:RELWITHDEBINFO>,"
         "$<CONFIG:MINSIZEREL>>:${serialize_Clang_release_flags};>"
  "$<$<BOOL:${PROJECT_NAME}_COVERAGE>:${serialize_Clang_coverage_flags};>")

target_compile_options(${PROJECT_NAME}_development INTERFACE
  $<$<CXX_COMPILER_ID:Clang>:${generator}>)

target_link_libraries(${PROJECT_NAME}_development INTERFACE
$<$<CXX_COMPILER_ID:Clang>:${generator}>)

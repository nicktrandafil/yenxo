set(rproject_GNU_base_flags -Wall -Wextra -Wpedantic -Werror -ftemplate-backtrace-limit=0)
set(rproject_GNU_debug_flags -g -O0)
set(rproject_GNU_coverage_flags --coverage)
set(rproject_GNU_release_flags -O2 -DNDEBUG -Wno-unused-parameter)

string(CONCAT generator
  "${rproject_GNU_base_flags};"
  "$<$<OR:$<CONFIG:DEBUG>,"
         "$<CONFIG:RELWITHDEBINFO>>:${rproject_GNU_debug_flags};>"
  "$<$<OR:$<CONFIG:RELEASE>,"
         "$<CONFIG:RELWITHDEBINFO>,"
         "$<CONFIG:MINSIZEREL>>:${rproject_GNU_release_flags};>"
  "$<$<AND:$<CONFIG:DEBUG>,"
          "$<BOOL:${coverage}>>:${rproject_GNU_coverage_flags};>")

target_compile_options(development INTERFACE
  $<$<CXX_COMPILER_ID:GNU>:${generator}>)

target_link_libraries(development INTERFACE
$<$<CXX_COMPILER_ID:GNU>:${generator}>)

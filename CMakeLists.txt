cmake_minimum_required(VERSION 2.6)
project(electric_sheep)

set(es_VERSION_MAJOR 1)
set(es_VERSION_MINOR 0)

include_directories(.)
include_directories(external/include)
link_directories(external/lib/x64)
configure_file(external/lib/x64/SDL2.dll ${PROJECT_BINARY_DIR}/SDL2.dll COPYONLY)

file(GLOB es_lib_src ./es_lib/*.h ./es_lib/*.cpp)
file(GLOB es_console_src ./es_console/*.h ./es_console/*.cpp)
file(GLOB es_tests_src ./es_tests/*.cpp)

add_library(es_lib STATIC ${es_lib_src})
add_executable(es_console ${es_console_src})
add_executable(es_tests ${es_tests_src})

target_link_libraries(es_console es_lib)
target_link_libraries(es_console SDL2)
target_link_libraries(es_tests es_lib)
target_link_libraries(es_tests SDL2)

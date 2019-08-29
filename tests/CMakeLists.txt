
#file(GLOB test_files ../src/tests/*.cpp)
set(test_files
        ../src/tests/node.cc)

include_directories()

add_executable(tests main.cc ${test_files})
target_link_libraries(tests object_property_tree)

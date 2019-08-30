set(test_files
        ../src/tests/node.cc
        ../src/tests/node_path.cc
        ../src/tests/property_tree.cc
        )

include_directories()

add_executable(tests main.cc ${test_files})
target_link_libraries(tests object_property_tree)

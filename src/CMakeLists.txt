set(LIB_SOURCES
        node.cc
        node_path.cc
        object_property_tree.cc
        property_tree.cc
        )

add_library(object_property_tree SHARED ${LIB_SOURCES})
target_link_libraries(object_property_tree ${Boost_LIBRARIES})

## set the shared library soname
set_target_properties(object_property_tree PROPERTIES
        VERSION ${OBJECT_PROPERTY_TREE_VERSION}
        SOVERSION ${VERSION_MAJOR})

## install the shared library
install(TARGETS object_property_tree
        LIBRARY DESTINATION lib)

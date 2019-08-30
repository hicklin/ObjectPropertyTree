#include "catch.hpp"
#include "object_property_tree.h"

TEST_CASE("ObjectTree") {
  ObjectPropertyTree object_tree;

  // Set and get objects.
  int object1 = 42;
  std::string object2 = "I wonder if it will be friends with me";
  char object3 = 'H';

  ObjectPath path1;
  path1.ToList("object1");

  object_tree.SetObject(path1, object1);
  object_tree.SetObject("object1.object2", object2);
  object_tree.SetObject("object3", object3);

  int object1_get = object_tree.GetObject<int>(path1);
  REQUIRE(object1 == object1_get);
  std::string object2_get = object_tree.GetObject<std::string>("object1.object2");
  REQUIRE(object2 == object2_get);
  char object3_get = object_tree.GetObject<char>("object3");
  REQUIRE(object3 == object3_get);

  // Set and get pointers.
  int object4 = 2;
  std::string object5 = "oh no!";
  char object6 = 'G';

  ObjectPath path4;
  path4.ToList("object4");

  object_tree.SetObjectPointer(path4, &object4);
  object_tree.SetObjectPointer("objects.object5", &object5);
  object_tree.SetObjectPointer("objects.object6", &object6);

  auto *object4_get = object_tree.GetObjectPointer<int>(path4);
  REQUIRE(object4_get == &object4);
  auto *object5_get = object_tree.GetObjectPointer<std::string>("objects.object5");
  REQUIRE(object5_get == &object5);
  auto *object6_get = object_tree.GetObjectPointer<char>("objects.object6");
  REQUIRE(object6_get == &object6);

  object_tree.remove("object1.object2");

  object_tree.PrintTree();

  // Handling mistyping.
  auto object2_fail = object_tree.GetObject<char>("object1.object2");
  auto object2_ptr_fail = object_tree.GetObjectPointer<int>("object1.object2");

  REQUIRE(!object2_fail);
  REQUIRE(!object2_ptr_fail);
}

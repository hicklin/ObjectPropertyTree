#include "catch.hpp"
#include "property_tree.h"

TEST_CASE("PropertyTree") {
  PropertyTree<std::string, int> tree;
  std::vector<std::string> children;

  // Changed
  REQUIRE(!tree.changed());
  tree.SetChanged();
  REQUIRE(tree.changed());
  tree.ClearChanged();
  REQUIRE(!tree.changed());

  // Adding a node
  tree.SetData("child_a.child_1", 1);
  REQUIRE(tree.changed());
  REQUIRE(tree.exists("child_a"));
  REQUIRE(tree.exists("child_a.child_1"));
  int data;
  tree.GetData("child_a.child_1", data);
  REQUIRE(data == 1);
  REQUIRE(tree.ListChildren(children) == 1);
  REQUIRE(tree.ListChildren("child_a", children) == 1);

  // Getting a node. Root:
  Node<std::string, int> node = tree.GetRootNode();
  REQUIRE(node.HasChild("child_a"));
  REQUIRE(node.GetChild("child_a")->HasChild("child_1"));
  // Child node:
  Node<std::string, int> *child_node = tree.GetNode("child_a");
  REQUIRE(child_node->HasChild("child_1"));

  // Getting node path
  Node<std::string, int> *deep_child_node = tree.GetNode("child_a.child_1");
  NodePath<std::string> deep_child_path;
  tree.GetFullPath(deep_child_node, deep_child_path);
  NodePath<std::string> deep_child_path_test;
  deep_child_path_test.ToList("child_a.child_1");
  REQUIRE(deep_child_path == deep_child_path_test);

  // Clear
  tree.ClearChanged();
  tree.clear();
  REQUIRE(tree.changed());
  tree.ClearChanged();
  REQUIRE(tree.ListChildren(children) == 0);
}
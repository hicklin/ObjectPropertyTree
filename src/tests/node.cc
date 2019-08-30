#include "catch.hpp"
#include "node.h"
#include <boost/any.hpp>

TEST_CASE("Property tree node") {
  std::string node_name = "Parent test node";
  std::string child1_name = "Child1 test node";
  std::string child2_name = "Child2 test node";
  std::string child3_name = "Child3 test node";

  // Create parent.
  Node<std::string, boost::any> parent_node(node_name);
  REQUIRE(parent_node.name() == node_name);
  REQUIRE(parent_node.children().empty());
  // Set and get data.
  parent_node.SetData(12);
  REQUIRE(boost::any_cast<int>(parent_node.data()) == 12);

  // Create and add child.
  REQUIRE(!parent_node.HasChild(child2_name));
  Node<std::string, boost::any> child1_node(child1_name);
  parent_node.AddChild(&child1_node);

  // Add child.
  Node<std::string, boost::any>* child2_node = parent_node.CreateChild(child2_name, &parent_node);
  REQUIRE(child2_node->name() == child2_name);

  // Clear children.
  REQUIRE(parent_node.HasChild(child1_name));
  REQUIRE(parent_node.HasChild(child2_name));
  REQUIRE(!parent_node.HasChild("no name"));
  REQUIRE(parent_node.children().size() == 2);
  parent_node.ClearChildren();
  REQUIRE(!parent_node.HasChild(child1_name));
  REQUIRE(!parent_node.HasChild(child2_name));
  REQUIRE(parent_node.children().empty());

  // Get child
  Node<std::string, boost::any>* child3_node = parent_node.CreateChild(child3_name, &parent_node);
  REQUIRE(child3_node == parent_node.GetChild(child3_name));

  // Reset child name
  std::string new_child3_name = "New Child3 name";
  child3_node->SetName(new_child3_name);
  REQUIRE(child3_node->name() == new_child3_name);
  REQUIRE(parent_node.HasChild(new_child3_name));

  // Confirm that the parent is correct
  REQUIRE(&parent_node == child3_node->parent());

  // Change parent
  Node<std::string, boost::any> new_parent_node("new_parent");
  child3_node->SetParent(&new_parent_node);
  REQUIRE(!parent_node.HasChild(new_child3_name));
  REQUIRE(new_parent_node.HasChild(new_child3_name));

  // Find
  REQUIRE(child3_node == new_parent_node.Find(new_child3_name));

  // Remove child
  REQUIRE(new_parent_node.HasChild(new_child3_name));
  parent_node.RemoveChild(new_child3_name);
  REQUIRE(!parent_node.HasChild(new_child3_name));
  REQUIRE(parent_node.children().empty());

  // Add nodes
  parent_node.Add("child1.child2");
  REQUIRE(parent_node.HasChild("child1"));
  REQUIRE(parent_node.GetChild("child1")->HasChild("child2"));
  parent_node.Add("child1.child2.child3");
  REQUIRE(parent_node.GetChild("child1")->GetChild("child2")->HasChild("child3"));

  // Remove nodes
  parent_node.Remove("child1.child2.child3");
  REQUIRE(parent_node.GetChild("child1")->HasChild("child2"));
  REQUIRE(!parent_node.GetChild("child1")->GetChild("child2")->HasChild("child3"));
  parent_node.Remove("child1");
  REQUIRE(!parent_node.HasChild("child1"));
  REQUIRE(parent_node.GetChild("child1") == nullptr);

}
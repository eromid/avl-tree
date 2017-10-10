/*
avl-test-helper.h
Copyright (c) Eromid (Olly) 2017

Friend of avltree class which contains functions only used in testing.
*/

#ifndef AVL_TEST_HELPER_H
#define AVL_TEST_HELPER_H

#include <algorithm>
#include <cmath>
#include <assert.h>

using std::max;

#include <iostream>
using std::cout;
using std::endl;

template<typename K, typename V>
class test_helper
{
private:  
  test_helper() {} // instantiation not needed (or allowed)
public:
  static unsigned int count_nodes(const avltree<K, V>& tree)
  {
    if (tree.root)
      return count_descendants(tree.root);
    else
      return 0u;
  }

  static bool is_avl(const avltree<K, V>& tree)
  {
    return is_avl(tree.root);
  }

  static bool valid_balance_factors(const avltree<K, V>& tree)
  {
    return valid_balance_factors(tree.root);
  }

  static void test_meta_functions()
  {
    {
      // Counting nodes
      avltree<int, double> tree;
      assert(count_nodes(tree) == 0);  // Empty tree should have 0 nodes.
      tree.insert(1, 1.0);
      assert(count_nodes(tree) == 1);  // one node inserted
      tree.insert(1, 1.0);
      assert(count_nodes(tree) == 1);  // same node inserted twice
      tree.insert(2, 2.0);
      assert(count_nodes(tree) == 2);  // second unique node inserted
      tree.remove(3);
      assert(count_nodes(tree) == 2);  // non existent node removed
      tree.remove(1);
      assert(count_nodes(tree) == 1);  // one node removed
      tree.remove(2);
      assert(count_nodes(tree) == 0);
    }
    {
      // subtree height
      avltree<int, double> tree;
      assert(subtree_height(tree.root) == 0);  // root is null, height is 0.
      tree.insert(5, 5.0);
      assert(subtree_height(tree.root) == 1);  // single node height should be 1.
      tree.insert(8, 8.0);
      assert(subtree_height(tree.root) == 2);  // height should be 2; root has a single right child
      tree.insert(3, 3.0);
      assert(subtree_height(tree.root) == 2);  // height should be 2; root has a left and right child
      tree.insert(7, 7.0);
      assert(subtree_height(tree.root) == 3);  // height should be 2; root has a left and right child    
    }
  }

private:
  static bool is_avl(const shared_ptr<typename avltree<K, V>::node> root_node)
  {
    if (!root_node) return true;  // Base case; an empty tree is always AVL.
    bool left_is_avl, right_is_avl;
    unsigned int left_height, right_height;
    left_is_avl = is_avl(root_node->left_child);
    left_height = subtree_height(root_node->left_child);
    right_is_avl = is_avl(root_node->right_child);
    right_height = subtree_height(root_node->right_child);
    if (root_node->parent.lock() == nullptr)
    {
      cout << "left subtree: " << left_height << " | right subtree: " << right_height << endl;
      cout << "left_is_avl: " << left_is_avl << " | right_is_avl: " << right_is_avl << endl;
    }
    return left_is_avl && right_is_avl && (abs(left_height - right_height) <= 1u);
  }

  static bool valid_balance_factors(const shared_ptr<typename avltree<K, V>::node> node)
  {
    if (!node) return true;
    cout << "left_height: " << subtree_height(node->left_child) << " | right_height: "
         << subtree_height(node->right_child) << " | balance_factor: " << static_cast<int>(node->balance_factor) << endl;
    return (subtree_height(node->left_child) - subtree_height(node->right_child) == node->balance_factor)
            && valid_balance_factors(node->left_child) && valid_balance_factors(node->right_child);
  }

  static unsigned int count_descendants(const shared_ptr<typename avltree<K, V>::node> node)
  {
    unsigned int left_count = node->left_child ? count_descendants(node->left_child) : 0;
    unsigned int right_count = node->right_child ? count_descendants(node->right_child) : 0;
    return 1u + left_count + right_count;
  }

  static unsigned int subtree_height(const shared_ptr<typename avltree<K, V>::node> node)
  {
    if (!node) return 0u;  // no subtree root -> height = 0.
    unsigned int left_height, right_height;
    left_height = right_height = 0u;
    left_height = subtree_height(node->left_child);
    right_height = subtree_height(node->right_child);
    return 1 + max(left_height, right_height);
  }
};

#endif

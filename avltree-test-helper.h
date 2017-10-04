/*
optional.h
Copyright (c) Eromid (Olly) 2017

Friend of avltree class which contains functions only used in testing.
*/

#ifndef AVL_TEST_HELPER_H
#define AVL_TEST_HELPER_H

#include <algorithm>
#include <cmath>

using std::max;

#include <iostream>
using std::cout;
using std::endl;

template<typename K, typename V>
class test_helper
{
public:
  static unsigned int count_nodes(const avltree<K, V>& tree)
  {
    if (tree.root)
      return count_descendants(tree.root);
    else
      return 0u;
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

  static bool is_avl(const avltree<K, V>& tree)
  {
    return is_avl(tree.root);
  }

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
      if (DBG) { cout << "left subtree: " << left_height << " | right subtree: " << right_height << endl; }
      if (DBG) { cout << "left_is_avl: " << left_is_avl << " | right_is_avl: " << right_is_avl << endl; }
    }
    return left_is_avl && right_is_avl && (abs(left_height - right_height) <= 1u);
  }

  static bool valid_balance_factors(const avltree<K, V>& tree)
  {
    return valid_balance_factors(tree.root);
  }


  static bool valid_balance_factors(const shared_ptr<typename avltree<K, V>::node> node)
  {
    if (!node) return true;
    if (DBG) { cout << "left_height: " << subtree_height(node->left_child) << " | right_height: "
         << subtree_height(node->right_child) << " | balance_factor: " << static_cast<int>(node->balance_factor) << endl; }
    return (subtree_height(node->left_child) - subtree_height(node->right_child) == node->balance_factor)
            && valid_balance_factors(node->left_child) && valid_balance_factors(node->right_child);
  }

private:
  test_helper() {} // instantiation not needed (or allowed)
};

#endif

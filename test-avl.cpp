/*
optional.h
Copyright (c) Eromid (Olly) 2017

Unit tests for AVL tree implementation in avltree.h.
*/

#include <assert.h>
#include "avltree.h"
#include "avltree-test-helper.h"

#include <iostream>
using std::cout;
using std::endl;
using std::left;
using std::right;

#include <iomanip>
using std::setw;
using std::setfill;

using tests = test_helper<int, double>;

#define TEST_CASE(fn)\
cout << "================================================================================" << endl;\
cout << "    Running " << #fn << "...";                                                            \
fn();                                                                                              \
cout << " Complete. " << endl;                                                                     \
cout << "================================================================================" << endl;


// Run the tests inside the test-helper class template
void test_meta_functions()
{
  tests::test_meta_functions();
}

// Test empty tree behaves correctly
void test_empty_tree()
{
  avltree<int, double> tree;
  assert(tests::is_avl(tree));            // Tree should be an AVL tree
  assert(tests::count_nodes(tree) == 0);  // Empty tree should have 0 nodes.
  tree.remove(123);                       // Removing non-existing key should have no effect.
  assert(tests::count_nodes(tree) == 0);  // Empty tree should still have 0 nodes.
  assert(tests::is_avl(tree));            // Tree should be an AVL tree still
  assert(tests::valid_balance_factors(tree)); // Are all balance factors correct (none)
}

// Test tree with only one node behaves correctly
void test_single_node()
{
  const double value = 42.0;
  const int key = 1;
  avltree<int, double> tree;
  tree.insert(key, value);

  // After insertion of single node:
  assert(tests::count_nodes(tree) == 1);      // Empty tree should have 0 nodes.
  assert(tests::is_avl(tree));                // Tree should be an AVL tree still
  tree.remove(123);                           // Removing non-existing key should have no effect.
  assert(tests::count_nodes(tree) == 1);      // Tree should now have one node
  assert(tests::is_avl(tree));                // Tree should be an AVL tree still
  assert(tree.get(key).has_value() == true);  // query should return a value
  assert(tree.get(key).value() == value);     // value should be what we put in
  assert(tests::valid_balance_factors(tree)); // Are all balance factors correct

  // after removing single node:
  tree.remove(key);
  assert(tests::count_nodes(tree) == 0);      // Empty tree should still have 0 nodes.
  assert(tests::is_avl(tree));    
  assert(tree.get(key).has_value() == false);      // query should return a value
  assert(tests::valid_balance_factors(tree)); // Are all balance factors correct

}

// Test repeated right insertions don't imbalance the tree
void test_multiple_right_insertions()
{
  avltree<int, double> tree;
  const static int n_insertions = 100;
  for (unsigned int i = 1; i <= n_insertions; ++i)
  {
    tree.insert(i, static_cast<double>(i));   // insert increasing numbers
    assert(tests::is_avl(tree));              // check tree remains AVL
    assert(tests::count_nodes(tree) == i);          // Check the node made is in
    assert(tests::valid_balance_factors(tree)); // Are all balance factors correct
  }
  for (int i = 1; i < n_insertions; ++i)
  {
    assert(tree.get(i).has_value() == true);  // query should return a value
    assert(tree.get(i).value() == static_cast<double>(i));     // value should be what we put in
  }
}

// Test repeated left insertions don't imbalance the tree
void test_multiple_left_insertions()
{
  avltree<int, double> tree;
  const static int n_insertions = 100;
  for (unsigned int i = n_insertions; i >= 1; --i)
  {
    tree.insert(i, static_cast<double>(i));   // insert increasing numbers
    assert(tests::is_avl(tree));              // check tree remains AVL
    assert(tests::count_nodes(tree) == n_insertions - (i -1));          // Check the node made is in
    assert(tests::valid_balance_factors(tree)); // Are all balance factors correct
  }
  for (int i = 1; i < n_insertions; ++i)
  {
    assert(tree.get(i).has_value() == true);  // query should return a value
    assert(tree.get(i).value() == static_cast<double>(i));     // value should be what we put in
  }
}

// Test that a right-left rotation correctly balances the tree
void test_right_left_rotation()
{
  avltree<int, double> tree;
  tree.insert(10, 10.0);      // Root
  tree.insert(20, 20.0);      // Right child of root
  tree.insert(15, 15.0);      // left child of right child of root -> triggers RL rotation
  assert(tests::is_avl(tree));
  assert(tests::valid_balance_factors(tree));

}

// Test that a left-right rotation correctly balances the tree
void test_left_right_rotation()
{
  avltree<int, double> tree;
  tree.insert(20, 20.0);        // Root node
  tree.insert(10, 10.0);        // Left child
  tree.insert(15, 15.0);        // Right child of left child -> triggers LR rotation
  assert(tests::is_avl(tree));
  assert(tests::valid_balance_factors(tree));
}

// Call all the above cases
int main()
{
  TEST_CASE(test_meta_functions);
  TEST_CASE(test_empty_tree);
  TEST_CASE(test_single_node);
  TEST_CASE(test_multiple_right_insertions);
  TEST_CASE(test_multiple_left_insertions);
  TEST_CASE(test_right_left_rotation);
  TEST_CASE(test_left_right_rotation);
  return 0;
}

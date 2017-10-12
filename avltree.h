/*
avltree.h
Copyright (c) Eromid 2017

A template for an AVL tree - a self balancing binary search tree.

*/
#ifndef AVLTREE_H
#define AVLTREE_H

#include <memory>
#include <cinttypes>
#include <cmath>

#include "optional.h"

#define LEFT_HEAVY (1)
#define RIGHT_HEAVY (-1)
#define BALANCED (0)

using std::shared_ptr;
using std::make_shared;
using std::weak_ptr;

// Set DBG true to enable detailed output.
#define DBG false
#include <iostream>
using std::cout;
using std::endl;

// Forward declaration of test_helper class template so we can declare its friendship.
// The test_helper class contains some meta functionality to check the implementation is valid.
template<typename K, typename V>
class test_helper;

// A class template implementing an AVL tree - a kind of self balancing binary search tree.
// The class supports the typical operations; insertion, removal and search.
// No exceptions are thrown. The class uses an 'optional' type to return results of searching
// since the key might not be present in the tree.
//
// Key type (K) should have well defined strict ordering, implemented with comparison operators
// '<' and '>'.
// The value type (V) can be anything.
template <typename K, typename V>
class avltree
{
public:

  // Add a key-value pair to the tree.
  //   K& key: The key.
  //   T& value: The value associated with the key.
  void insert(const K& key, const V& value);

  
  // Find the value associated with a given key. Returns an `optional` struct since the key isn't
  // guaranteed to be present in the tree.
  optional<V> get(const K& key) const;


  // Remove node from the tree with given key. Doesn't matter if the node isn't there.
  void remove(const K& key);

protected:

  // A node in the tree, containing the key, some value, and pointers to the two children as well
  // as a `balance factor` used to keep the tree balanced (as an AVL tree).
  // `node` is an implementation detail, it shouldn't be returned or accessible by users of the
  // template.
  struct node
  {
    // Create a new node with a given key and value, under given parent node.
    node(K key, V value, shared_ptr<node> parent) : key(key), value(value), parent(parent),
      balance_factor(0) {}
    // The key
    K key;
    V value;
    shared_ptr<node> left_child;
    shared_ptr<node> right_child;
    weak_ptr<node> parent;
    int8_t balance_factor;
  
    // returns true if the node is the left child of its parent.
    bool is_left_child() const;

    // returns true if the node is the right child of its parent.
    bool is_right_child() const;

    // Get the sibling node of a parent or nullptr if there is none.
    shared_ptr<node> sibling() const;

    // Assuming tree is AVL and both children have correct balance factors...
    void correct_balance();

    friend void set_left_child(shared_ptr<node> parent_node, shared_ptr<node> child)
    {
      parent_node->left_child = child;
      if (child) { child->parent = parent_node; }
    }

    friend void set_right_child(shared_ptr<node> parent_node, shared_ptr<node> child)
    {
      parent_node->right_child = child;
      if (child) { child->parent = parent_node; }
    }
  };

  // A pointer to the root `node` of the tree. If this is null, then the tree is empty.
  shared_ptr<node> root;


  // Find a node with the given key; a helper for insertion, retrieval and removal.
  // Returns:
  //   1. If the tree is empty --> null pointer.
  //   2. If the key exists --> A pointer to the node with that key.
  //   2. If the key doesn't exist, but the tree isn't empty --> a pointer to its parent.
  shared_ptr<node> _node_search(K key) const;


  // After insertion retrace from this node back to the root, check for imbalance and correct it.
  void _retrace_insertion(shared_ptr<node> inserted_node);
  
  // After deletion retrace from this node back to the root, check for imbalance and correct it.
  // The balance_factor_change argument indicates which side the child of the subtree root had
  // a child removed.
  void _retrace_deletion(shared_ptr<node> subtree_root, int8_t balance_factor_change);

  // Perform a left rotation to rebablance a subtree rooted at old_subtree_root, returning
  // a pointer to the new root of the subtree.
  shared_ptr<node> _left_rotate(shared_ptr<node> old_subtree_root);

  // Perform a right rotation to rebablance a subtree rooted at old_subtree_root, returning
  // a pointer to the new root of the subtree.
  shared_ptr<node> _right_rotate(shared_ptr<node> old_subtree_root);

  // Perform a left-right rotation to rebablance a subtree rooted at old_subtree_root, returning
  // a pointer to the new root of the subtree.
  shared_ptr<node> _left_right_rotate(shared_ptr<node> old_subtree_root);

  // Perform a right-left rotation to rebablance a subtree rooted at old_subtree_root, returning
  // a pointer to the new root of the subtree.
  shared_ptr<node> _right_left_rotate(shared_ptr<node> old_subtree_root);

  // The test_helper class contains some meta functionality to check the implementation is valid.
  friend class test_helper<K, V>;
};



// ============================================================================================ //
// |                              `avltree` method definitions                                | //
// ============================================================================================ //

// Insert a node with a given key.
template <typename K, typename V>
void avltree<K,V>::insert(const K& key, const V& value)
{
  shared_ptr<node> target = _node_search(key);
  if (!target)    // Base case, we have an empty tree, the inserted node is the new root.
    root = make_shared<node>(key, value, nullptr);
  else if (target->key == key)  // The key exists already, we update its value.
    target->value = value;
  else if (target->key > key)  // new node is left child
  {
    target->left_child = make_shared<node>(key, value, target);
    _retrace_insertion(target->left_child);
  }
  else // new node is right child
  {
    target->right_child = make_shared<node>(key, value, target);
    _retrace_insertion(target->right_child);
  }
}

// Get (maybe) a node with a given key.
template <typename K, typename V>
optional<V> avltree<K, V>::get(const K& key) const
{
  shared_ptr<node> found_node = _node_search(key);
  if (found_node && (key == found_node->key))
    return optional<V>(found_node->value);
  return optional<V>();
}

// Remove a node with given key from the tree
template <typename K, typename V>
void avltree<K, V>::remove(const K& key)
{
  shared_ptr<node> target = _node_search(key);
  // does the target node exist?
  if (!(target && (key == target->key)))
    return;

  // removed node has >0 children?
  if (target->left_child || target->right_child)
  {
    // removed node has 2 children?
    if (target->left_child && target->right_child)
    {
      if (DBG) { cout << "removing node with 2 children... " << endl; }
      // find in-order successor (node with smallest key that is > than this key)
      shared_ptr<node> successor = target->right_child;
      shared_ptr<node> successor_parent;
      while (successor->left_child)
        successor = successor->left_child;
      successor_parent = successor->parent.lock();

      // successor has a right child? (if it had left-child, it wouldn't be in-order successor!)
      if (successor->right_child)
      {
        if (DBG) { cout << "In order successor has a right child, reseating this to successor node." << endl; }
        // We need to reseat this child where "successor" lived.
        if (successor->is_left_child())
          set_left_child(successor_parent, successor->right_child);
        else // successor is a right child
          set_right_child(successor_parent, successor->right_child);
      }
      else  // Successor had no children, can just be removed.
      {
        if (DBG) { cout << "successor had no children, removing it." << endl; }
        if (successor->is_left_child())
          successor_parent->left_child = nullptr;
        else // successor is a right child
          successor_parent->right_child = nullptr;
      }
      // We are moving this node's key and value to our deleted node lived, and deleting this.
      target->key = successor->key;
      target->value = successor->value;
      if (successor->is_left_child())
      {
        _retrace_deletion(successor_parent, -LEFT_HEAVY);
      }
      else
      {
        _retrace_deletion(successor_parent, -RIGHT_HEAVY);
      }
    }
    // removed node has 1 child?
    else
    {
      if (DBG) { cout << "node had one child, moving that orphan to position of removed node..." << endl; }
      // get pointer to the node that would be orphaned
      shared_ptr<node> orphan = (target->left_child) ? target->left_child : target->right_child;
      // If the root is being deleted, this orphan must become the new root
      if (target == root)
        root = orphan;
      // otherwise, assign it to the side of parent that our target node lived
      else if (target->is_left_child())
      {
        set_left_child(target->parent.lock(), orphan);
      }
      else
        set_right_child(target->parent.lock(), orphan);
    }
  }
  // Removed node had 0 children?
  else
  {
    if (DBG) { cout << "node had no children, removing and retracing from parent." << endl; }
    if (target == root)  // parent == null in this case
    {
      if (DBG) { cout << "node was root, tree is now empty" << endl; }
      root = nullptr;     // we remove the root, leaving an empty tree, no balancing required
    }
    else if (target->is_left_child())
    {
      target->parent.lock()->left_child = nullptr;
      _retrace_deletion(target->parent.lock(), -LEFT_HEAVY);
    }
    else
    {
      target->parent.lock()->right_child = nullptr;
      _retrace_deletion(target->parent.lock(), -RIGHT_HEAVY);
    }
  }
}

// Find a node with given key; returning null if there are no nodes, a pointer to the would-be
// parent if the node doesn't exist, or a pointer to the node itself if it does.
template <typename K, typename V>
shared_ptr<typename avltree<K, V>::node> avltree<K, V>::_node_search(K key) const
{
  // Base case, we have an empty tree.
  if (!root)
    return nullptr;
  // Iterative search. We follow branch directions based on comparing the keys.
  shared_ptr<node> current = root;
  for (;;)
  {
    if (key < current->key)
      if (current->left_child)
        current = current->left_child;
      else
        return current;
    else if (key > current->key)
      if (current->right_child)
        current = current->right_child;
      else
        return current;
    else // (key == current->key)
      return current;
  }
}

// Retrace after a node is inserted in order to check tree is still AVL and, if not, rebalance it.
template <typename K, typename V>
void avltree<K, V>::_retrace_insertion(shared_ptr<node> inserted_node)
{
  shared_ptr<node> current;
  shared_ptr<node> parent;
  if (DBG) { cout << "\n---------------- Retracing insertion ----------------" << endl; }
  for (current = inserted_node; current->parent.lock() != nullptr ; current = parent)
  {
    if (DBG) { cout << "backtracing:" << endl; }
    parent = current->parent.lock();
    if (current->is_left_child())
    {
      if (DBG) { cout << "Child is left child" << endl; }
      if (parent->balance_factor == LEFT_HEAVY)
      {
        //tree is unbalanced, left too heavy
        if (DBG) { cout << "Parent is already LEFT_HEAVY, rebalance required." << endl; }
        if (current->balance_factor == RIGHT_HEAVY)
        {
          if (DBG) { cout << "Found RIGHT_HEAVY subsubtree in LEFT_HEAVY subtree, performing right-left rotation." << endl; }
          // left child in right subtree -> right-left double rotation
          _left_right_rotate(parent);
          return;
        }
        else // (parent->is_right_child() || parent == root) necessarily.
        {
          // left child in left subtree -> single right rotation
          if (DBG) { cout << "Found LEFT_HEAVY subsubtree in LEFT_HEAVY subtree, performing right rotation." << endl; }
          _right_rotate(parent);
          return;
        }
      }
      else
      {
        if (DBG) { cout << "Adding LEFT_HEAVY to parent balance factor." << endl; }
        parent->balance_factor += LEFT_HEAVY;
        if (parent->balance_factor == BALANCED)
        {
          if (DBG) { cout << "Tree is now AVL, returning!" << endl; }
          return;
        }
        else
          continue;
      }
    }
    else // (current->is_right_child()) necessarily, we exited loop if there was no parent, so it can't be root.
    {
      if (DBG) { cout << "Child is right child" << endl; }
      if (parent->balance_factor == RIGHT_HEAVY)
      {
        if (DBG) { cout << "parent is RIGHT_HEAVY" << endl;  }
        //tree is unbalanced, right too heavy
        if (current->balance_factor == LEFT_HEAVY)
        {
          // right child in left subtree -> left-right double rotation
          if (DBG) { cout << "Found LEFT_HEAVY subsubtree in RIGHT_HEAVY subtree, performing right-left rotation." << endl; }
          _right_left_rotate(parent);
          return;
        }
        else // (parent->is_right_child() || parent == root) necessarily.
        {
          // right child in right subtree -> single left rotation
          if (DBG) { cout << "Found RIGHT_HEAVY subsubtree in RIGHT_HEAVY subtree, performing left rotation." << endl; }
          _left_rotate(parent);
          return;
        }
      }
      else
      {
        if (DBG) { cout << "Adding RIGHT_HEAVY to parent balance factor." << endl; }
        parent->balance_factor += RIGHT_HEAVY;
        if (parent->balance_factor == BALANCED)
        {
          if (DBG) { cout << "Tree is now AVL, returning!" << endl; }
          return;
        }
        else
          continue;
      }
    }
  }
  if (DBG) { cout << "Fell through, we made it to root? " << (current == root) << endl; }
}

// Retrace after a node is deleted in order to check tree is still AVL and, if not, rebalance it.
template <typename K, typename V>
void avltree<K, V>::_retrace_deletion(shared_ptr<node> subtree_root, int8_t balance_factor_change)
{
  shared_ptr<node> current = subtree_root;
  shared_ptr<node> parent;

  if (DBG) { cout << "\n---------------- Retracing deletion ----------------" << endl; }
  if (current->balance_factor == BALANCED)
  {
    if (DBG) { cout << "Subtree was balanced, no illegal subtree height change." << endl; }
    current->balance_factor += balance_factor_change;
    return;
  }

  else if ((current->balance_factor + balance_factor_change) != BALANCED)
  {
    if (DBG) { cout << "Subtree is imbalanced here, need to rotate." << endl; }
    if (balance_factor_change == LEFT_HEAVY)
    {
      if (subtree_root->left_child->balance_factor == LEFT_HEAVY)
      {
        if (DBG) { cout << "Found LEFT_HEAVY subsubtree in LEFT_HEAVY subtree, performing right rotation." << endl; }
        _right_rotate(subtree_root);
      }
      else
      {
        if (DBG) { cout << "Found RIGHT_HEAVY subsubtree in LEFT_HEAVY subtree, performing left-right rotation." << endl; }
        _left_right_rotate(subtree_root);
      }
    }
    else // RIGHT_HEAVY
    {
      if (subtree_root->right_child->balance_factor == RIGHT_HEAVY)
      {
          if (DBG) { cout << "Found RIGHT_HEAVY subsubtree in RIGHT_HEAVY subtree, performing left rotation." << endl; }
        _left_rotate(subtree_root);
      }
      else
      {
        if (DBG) { cout << "Found LEFT_HEAVY subsubtree in RIGHT_HEAVY subtree, performing right-left rotation." << endl; }
        _right_left_rotate(subtree_root);
      }
    }
    return;
  }
  else
  {
    if (DBG) { cout << "Subtree is balanced, but height has reduced by one, need to retrace..." << endl; }
  }

  for (current = subtree_root; current->parent.lock() != nullptr ; current = parent)
  {
    if (DBG) { cout << "backtracing:" << endl; }
    parent = current->parent.lock();
    if (current->is_left_child())
    {
      if (DBG) { cout << "Left subtree height has been reduced." << endl; }
      if (parent->balance_factor == RIGHT_HEAVY)
      {
        if (current->balance_factor == RIGHT_HEAVY)
        {
          // left rotate
          if (DBG) { cout << "Found RIGHT_HEAVY subsubtree in RIGHT_HEAVY subtree, performing left rotation." << endl; }
          _left_rotate(parent);
          return;
        }
        else
        {
          // right-left rotate
          if (DBG) { cout << "Found LEFT_HEAVY subsubtree in RIGHT_HEAVY subtree, performing right-left rotation." << endl; }
          _right_left_rotate(parent);
          return;
        }
      }
      else
      {
        if (DBG) { cout << "Subtracting LEFT_HEAVY from parent balance factor." << endl; }
        parent->balance_factor -= LEFT_HEAVY;
        if (parent->balance_factor == BALANCED)
        {
          if (DBG) { cout << "Tree is now AVL, returning!" << endl; }
          return;
        }
        else
          continue;
      }
    }
    else // (current is a right child necessarily, we exited loop if there was no parent, so it can't be root.
    {
      if (DBG) { cout << "Child is right child" << endl; }
      if (parent->balance_factor == LEFT_HEAVY)
      {
        if (current->balance_factor == LEFT_HEAVY)
        {
          if (DBG) { cout << "Found LEFT_HEAVY subsubtree in LEFT_HEAVY subtree, performing right rotation." << endl; }
          _right_rotate(parent);
          return;
        }
        else
        {
          if (DBG) { cout << "Found LEFT_HEAVY subsubtree in RIGHT_HEAVY subtree, performing left-right rotation." << endl; }
          _left_right_rotate(parent);
          return;
        }
      }
      else
      {
        if (DBG) { cout << "Adding RIGHT_HEAVY to parent balance factor." << endl; }
        parent->balance_factor -= RIGHT_HEAVY;
        if (parent->balance_factor == BALANCED)
        {
          if (DBG) { cout << "Tree is now AVL, returning!" << endl; }
          return;
        }
        else
          continue;
      }
    }
  }
  if (DBG) { cout << "Fell through, we made it to root? " << (current == root) << endl; }
}

// Perform left rotation around given node.
template <typename K, typename V>
shared_ptr<typename avltree<K, V>::node>
avltree<K, V>::_left_rotate(shared_ptr<avltree<K, V>::node> old_subtree_root)
{
  if (DBG) { cout << "performing left rotation..." << endl; }
  shared_ptr<node> new_subtree_root = old_subtree_root->right_child;
  shared_ptr<node> grandparent = old_subtree_root->parent.lock();
  shared_ptr<node> orphan = new_subtree_root->left_child;
  
  if (DBG) { cout << "  old root = " << old_subtree_root->value << endl; }
  if (DBG) { cout << "  new root = " << new_subtree_root->value << endl; }

  if (old_subtree_root->is_left_child())
    set_left_child(grandparent, new_subtree_root);
  else if (old_subtree_root->is_right_child())
    set_right_child(grandparent, new_subtree_root);
  else // old_subtree_root is whole tree root.
  {
    root = new_subtree_root;
    new_subtree_root->parent.reset();
  }
  set_left_child(new_subtree_root, old_subtree_root);
  set_right_child(old_subtree_root, orphan);

  if (old_subtree_root->balance_factor == BALANCED)
  {
    new_subtree_root->balance_factor = RIGHT_HEAVY;
    old_subtree_root->balance_factor = LEFT_HEAVY;
  }
  else
  {
    new_subtree_root->balance_factor = BALANCED;
    old_subtree_root->balance_factor = BALANCED;
  }
  return new_subtree_root;
}

// Perform right rotation around given node.
template <typename K, typename V>
shared_ptr<typename avltree<K, V>::node>
avltree<K, V>::_right_rotate(shared_ptr<avltree<K, V>::node> old_subtree_root)
{
  if (DBG) { cout << "performing right rotation..." << endl; }
  shared_ptr<node> new_subtree_root = old_subtree_root->left_child;
  shared_ptr<node> grandparent = old_subtree_root->parent.lock();
  shared_ptr<node> orphan = new_subtree_root->right_child;  // may be nullptr

  if (DBG) { cout << "  old root = " << old_subtree_root->value << endl; }
  if (DBG) { cout << "  new root = " << new_subtree_root->value << endl; }

  if (old_subtree_root->is_left_child())
    set_left_child(grandparent, new_subtree_root);
  else if (old_subtree_root->is_right_child())
    set_right_child(grandparent, new_subtree_root);
  else // old_subtree_root is whole tree root.
  {
    root = new_subtree_root;
    new_subtree_root->parent.reset();
  }
  set_right_child(new_subtree_root, old_subtree_root);
  set_left_child(old_subtree_root, orphan);

  if (old_subtree_root->balance_factor == BALANCED)
  {
    new_subtree_root->balance_factor = LEFT_HEAVY;
    old_subtree_root->balance_factor = RIGHT_HEAVY;
  }
  else
  {
    new_subtree_root->balance_factor = BALANCED;
    old_subtree_root->balance_factor = BALANCED;
  }
  return new_subtree_root;
}

// Perform left-right rotation around a given node.
template <typename K, typename V>
shared_ptr<typename avltree<K, V>::node> 
avltree<K, V>::_left_right_rotate(shared_ptr<avltree<K, V>::node> old_subtree_root)
{
  if (DBG) { cout << "performing left-right rotation..." << endl; }
  _left_rotate(old_subtree_root->left_child);
  return _right_rotate(old_subtree_root);
}

// Perform right-left rotation around a given node.
template <typename K, typename V>
shared_ptr<typename avltree<K, V>::node> 
avltree<K, V>::_right_left_rotate(shared_ptr<avltree<K, V>::node> old_subtree_root)
{
  if (DBG) { cout << "performing right-left rotation..." << endl; }
  _right_rotate(old_subtree_root->right_child);
  return _left_rotate(old_subtree_root);
}



// ============================================================================================ //
// |                        `avltree::node` method definitions                                | //
// ============================================================================================ //

// returns true if the node is the left child of its parent.
template <typename K, typename V>
bool avltree<K, V>::node::is_left_child() const
{
  shared_ptr<avltree<K, V>::node> parent_sp = parent.lock();
  if (parent_sp)
  {
    if (parent_sp->left_child)
    {
      return key == parent_sp->left_child->key;
    }
  }
  return false;
}

// returns true if the node is the right child of its parent.
template <typename K, typename V>
bool avltree<K, V>::node::is_right_child() const
{
  shared_ptr<avltree<K, V>::node> parent_sp = parent.lock();
  if (parent_sp)
  {
    if (parent_sp->right_child)
    {
      return key == parent_sp->right_child->key;
    }
  }
  return false;
}

// get the sibling node of a parent or nullptr if there is none.
template <typename K, typename V>
shared_ptr<typename avltree<K, V>::node> avltree<K, V>::node::sibling() const
{
  if (is_left_child())
    return parent.lock()->left_child;
  else
    return parent.lock()->right_child;
}

// Assuming tree is AVL and both children have correct balance factors...
template <typename K, typename V>
void avltree<K, V>::node::correct_balance()
{
  if (left_child && right_child)
  {
    if (abs(left_child->balance_factor) == abs(right_child->balance_factor))  /// WRONG!!
      balance_factor = 0;
    else if (left_child->balance_factor)
      balance_factor = LEFT_HEAVY;
    else
      balance_factor = RIGHT_HEAVY;
  }
  else if (left_child)
    balance_factor = LEFT_HEAVY;
  else
    balance_factor = RIGHT_HEAVY;
}

#endif

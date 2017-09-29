/*
example.cpp
Copyright (c) Eromid (Olly) 2017

A template for an AVL tree - a self balancing binary search tree.

*/

#include "avltree.h"
#include <iostream>
#include <string>

using std::cout;
using std::endl;
using std::string;

void findAndPrint(avltree<int, string> tree, int key)
{
  const optional<string> &result = tree.search(key);
  if (result.has_value())
    cout << key << " --> " << result.value() << endl;
  else
    cout << key << " --> NULL" << endl;
}

void printTreeNodes(avltree<int, string> tree)
{
  findAndPrint(tree, 1);
  findAndPrint(tree, 2);
  findAndPrint(tree, 3);
  findAndPrint(tree, 4);
  findAndPrint(tree, 5);
  findAndPrint(tree, 6);
  findAndPrint(tree, 7);
}

int main(/*int argc, char const *argv[]*/)
{
  avltree<int, string> tree;
  tree.insert(2, "Bee");
  printTreeNodes(tree);
  tree.insert(1, "Ant");
  printTreeNodes(tree);
  tree.insert(5, "Eel");
  printTreeNodes(tree);
  tree.insert(4, "Donkey");
  printTreeNodes(tree);
  tree.insert(6, "Fox");
  printTreeNodes(tree);
  tree.insert(3, "Cat");
  printTreeNodes(tree);
  tree.insert(7, "Goat");
  printTreeNodes(tree);
  cout << "Initialised tree with 9 nodes." << endl;


  tree.remove(2);
  cout << "removed 2" << endl;
  printTreeNodes(tree);
  tree.remove(1);
  cout << "removed 1" << endl;
  printTreeNodes(tree);
  tree.remove(5);
  cout << "removed 5" << endl;
  printTreeNodes(tree);
  tree.remove(7);
  cout << "removed 7" << endl;
  printTreeNodes(tree);
  tree.remove(6);
  cout << "removed 6" << endl;
  printTreeNodes(tree);
  tree.remove(3);
  cout << "removed 3" << endl;
  printTreeNodes(tree);
  tree.remove(4);
  cout << "removed 4" << endl;
  printTreeNodes(tree);
  return 0;
}

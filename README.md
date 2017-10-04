# AVL-Tree #

This is a template class implementing an AVL tree -  a kind of self balancing binary search tree. It uses C++11, namely smart pointers to manage the memory of objects stored in the tree. There is also a very basic "optional" type to handle cases where we are returning from a search possibly without finding the key.

## Usage ##

An example of usage can be found in [example.cpp](example.cpp). Essentially, you include the header, then declare a tree with the key and value types specified:

```
#include "avltree.h"
...
  avltree<int, string> tree;
```

You can then *insert* or *overwrite* things:
```
  tree.insert(1, "foo")
  tree.insert(42, "foo");
  tree.insert(42, "foobar");
```

*Searching* for a value by key from the tree is simple, and won't throw any exceptions:
```
  const optional<string> &result = tree.search(42);
  if (result.has_value())
    std::cout << key << " --> " << result.value() << std::endl;
  else
    std::cout << "Key not in tree! << std::endl 

Output:
  42 --> foobar

```

*Removing* an item from the tree:
```
  tree.remove(42);
```

## Testing ##

I wouldn't advise using the library in production in its current form. There are unit tests which all currently pass, however there are more to be added. Also no consideration has been made for optimisation, other than checking for memory leaks with `valgrind`.

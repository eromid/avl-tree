/*
optional.h
Copyright (c) Eromid (Olly) 2017

A template for an object which may or may not hold a value.
*/

#ifndef OPTIONAL_H
#define OPTIONAL_H

// A struct which may or may not hold a value of type T.
template <typename T>
struct optional
{
  // Construct without value
  optional()         : _has_value(false)               {}

  // Construct with value
  optional(T& value) : _value(value), _has_value(true) {}

  // Return true if the struct has a value.
  bool has_value() const { return _has_value; }
  
  // Get the value. Will still return reference to default constructed value if _has_value is false, so it's up to
  // the user to check if the structure contains a valid value before using it.
  T& value()             { return _value; }
  const T& value() const { return _value; }

private:
  T _value;
  bool _has_value;
};

#endif  // OPTIONAL_H

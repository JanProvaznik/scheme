#include <cstdint>
#include <memory>
#include <iostream>
#include <vector>
#include <functional>

#ifndef SCHEME_TYPES_H
#define SCHEME_TYPES_H

enum class ValueType {
  List, String, Bool, Integer, Float, Function, Symbol, Nil, Closure, Environment
};


class Value {
protected:
ValueType type;
std::string name;
public:
Value(ValueType type) : type(type) {
}

virtual ~Value() {
}

virtual double to_double() const {
  return 0;
}

std::string get_name() const {
  return name;
}

ValueType get_type() const {
  return type;
}

virtual bool is_true() const {
  return false;
}


virtual std::string to_string() const {
  return "abstract value";
};

void print() const {
  std::cout << to_string() << std::endl;
}

};

class ListValue : public Value {
std::vector<std::shared_ptr<Value>> values;
public:
ListValue() : Value(ValueType::List), values() {
}
ListValue(std::vector<std::shared_ptr<Value> > values) : Value(ValueType::List), values(std::move(values)) {
}

std::string to_string() const override;

void add_value(const std::shared_ptr<Value> &value);

std::shared_ptr<Value> get_value(size_t index) const;

size_t size() const;
// get_sublist
std::shared_ptr<ListValue> get_sublist(size_t start) const;
void insert_value(size_t index, const std::shared_ptr<Value> &value);
void set_value(size_t index, const std::shared_ptr<Value> &value);
void pop_back();


};

class SymbolValue : public Value {
std::string value;
public:
SymbolValue(std::string value) : Value(ValueType::Symbol), value(std::move(value)) {
for (auto &c : this->value) {
  c = std::tolower(c);
}
}

std::string to_string() const override;
};

class NumberValue : public Value {
protected:
NumberValue(ValueType type) : Value(type) {
}
};

// don't use raw pointers but shared pointers
//using FunctionSharedPointer = std::shared_ptr<Value> (*)(size_t, std::vector<std::shared_ptr<Value> > &);
using FunctionSharedPointer = std::function<std::shared_ptr<Value>(size_t, std::vector<std::shared_ptr<Value>>&)>;
// with

class FunctionValue : public Value {
public:
explicit FunctionValue(FunctionSharedPointer function) : Value(ValueType::Function), function(function) {
}

FunctionSharedPointer get_function() const {
  return function;
}

std::string to_string() const;

private:
FunctionSharedPointer function{nullptr};

};

class IntegerValue : public NumberValue {
std::int64_t value;
public:
explicit IntegerValue(std::int64_t value);

std::int64_t get_value() const {
  return value;
}

double to_double() const override {
  return value;
}


std::string to_string() const override;
};

class FloatValue : public NumberValue {
double value;
public:
FloatValue(double value) : NumberValue(ValueType::Float) {
  this->value = value;
}

double get_value() const {
  return value;
}

double to_double() const override {
  return value;
}


std::string to_string() const override;
};

class StringValue : public Value {
std::string value;
public:
StringValue(std::string value) : Value(ValueType::String), value(std::move(value)) {
}

std::string to_string() const override;
};


class BoolValue : public Value {
bool value;
public:
explicit BoolValue(bool value) : Value(ValueType::Bool), value(value) {
}

std::string to_string() const override;

bool is_true() const override {
  return value;
}
};


class NilValue : public Value {
public:
NilValue() : Value(ValueType::Nil) {
}

std::string to_string() const override;
};



// class Complex : public Number {}; # TODO
// class Rational : public Number {};

#endif //SCHEME_TYPES_H

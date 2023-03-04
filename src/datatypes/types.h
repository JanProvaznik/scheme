#include <cstdint>
#include <memory>
#include <iostream>
#include <vector>

#ifndef SCHEME_TYPES_H
#define SCHEME_TYPES_H

enum class ValueType {
    ListValue,
    Pair,
    String
    ,Bool


};


class Value
{
    ValueType type;
    std::string name;
public:
    Value(ValueType type) : type(type) {}
    virtual ~Value() {}
    std::string get_name() const { return name; }
    ValueType get_type() const { return type; }
    virtual std::string to_string() const {return "abstract value";};
    void print() const { std::cout << to_string() << std::endl; }

};

class PairValue : public Value
{
    std::shared_ptr<Value> car;
    std::shared_ptr<Value> cdr;
public:
    PairValue(std::shared_ptr<Value> car, std::shared_ptr<Value> cdr) : Value(ValueType::ListValue), car(car), cdr(cdr) {}
    std::string to_string() const override;
    void set_car(std::shared_ptr<Value> car);
    void set_cdr(std::shared_ptr<Value> cdr);
};

class ListValue : public Value
{
    std::vector<std::shared_ptr<Value> > values;
public:
    ListValue() : Value(ValueType::ListValue), values() {
    }
    std::string to_string() const override;
    void add_value(const std::shared_ptr<Value> & value);
};

class SymbolValue : public Value
{
    std::string value;
public:
    SymbolValue(std::string value) : Value(ValueType::String), value(value) {}
    std::string to_string() const override;
};

class NumberValue : public Value
{
protected:
    NumberValue(ValueType type) : Value(type) {
    }
};


class IntegerValue : public NumberValue {
    std::int64_t value;
public:
    explicit IntegerValue(std::int64_t value) : NumberValue(ValueType::ListValue)  {
        this->value = value;
    }

    std::string to_string() const override;
};
class FloatValue : public NumberValue {
    FloatValue() : NumberValue(ValueType::ListValue) {}

    double value;

};
class StringValue : public Value {
    std::string value;
public:
    StringValue(std::string value) : Value(ValueType::String), value(value) {}
    std::string to_string() const override
    {
        return value;
    }
};


class BooleanValue : public Value {
    bool value;

public:
    explicit BooleanValue(bool value) : Value(ValueType::Bool), value(value) {
    }
    std::string to_string() const override
    {
        return value ? "#t" : "#f";
    }
};

// class Complex : public Number {}; # TODO
// class Rational : public Number {};

#endif //SCHEME_TYPES_H

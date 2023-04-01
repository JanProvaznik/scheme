#include <cstdint>
#include <memory>
#include <iostream>
#include <utility>
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
    explicit Value(ValueType type) : type(type) {
    }

    virtual ~Value() = default;

    [[nodiscard]] virtual double to_double() const {
        return 0;
    }

    [[nodiscard]] std::string get_name() const {
        return name;
    }

    [[nodiscard]] ValueType get_type() const {
        return type;
    }

    [[nodiscard]] virtual bool is_true() const {
        return false;
    }


    [[nodiscard]] virtual std::string to_string() const {
        return "abstract value";
    };

    void print() const {
        std::cout << to_string() << std::endl;
    }

};

class ListValue : public Value {
    std::vector<std::shared_ptr<Value> > values;
public:
    ListValue() : Value(ValueType::List), values() {
    }

    explicit ListValue(std::vector<std::shared_ptr<Value> > values) : Value(ValueType::List),
                                                                      values(std::move(values)) {
    }

    [[nodiscard]] std::string to_string() const override;

    void add_value(const std::shared_ptr<Value> &value);

    [[nodiscard]] std::shared_ptr<Value> get_value(size_t index) const;

    [[nodiscard]] size_t size() const;

// get_sublist
    [[nodiscard]] std::shared_ptr<ListValue> get_sublist(size_t start) const;

    void insert_value(size_t index, const std::shared_ptr<Value> &value);

    void set_value(size_t index, const std::shared_ptr<Value> &value);

    void pop_back();


};

class SymbolValue : public Value {
    std::string value;
public:
    explicit SymbolValue(std::string value) : Value(ValueType::Symbol), value(std::move(value)) {
        for (auto &c: this->value) {
            c = std::tolower(c);
        }
    }

    [[nodiscard]] std::string to_string() const override;
};

class NumberValue : public Value {
protected:
    explicit NumberValue(ValueType type) : Value(type) {
    }
};

// don't use raw pointers but shared pointers
//using FunctionSharedPointer = std::shared_ptr<Value> (*)(size_t, std::vector<std::shared_ptr<Value> > &);
using FunctionSharedPointer = std::function<std::shared_ptr<Value>(size_t, std::vector<std::shared_ptr<Value> > &)>;
// with

class FunctionValue : public Value {
public:
    explicit FunctionValue(FunctionSharedPointer function) : Value(ValueType::Function), function(std::move(function)) {
    }

    [[nodiscard]] FunctionSharedPointer get_function() const {
        return function;
    }

    [[nodiscard]] std::string to_string() const override;

private:
    FunctionSharedPointer function{nullptr};

};

class IntegerValue : public NumberValue {
    std::int64_t value;
public:
    explicit IntegerValue(std::int64_t value);

    [[nodiscard]] std::int64_t get_value() const {
        return value;
    }

    [[nodiscard]] double to_double() const override {
        return value;
    }


    [[nodiscard]] std::string to_string() const override;
};

class FloatValue : public NumberValue {
    double value;
public:
    explicit FloatValue(double value) : NumberValue(ValueType::Float) {
        this->value = value;
    }

    [[nodiscard]] double get_value() const {
        return value;
    }

    [[nodiscard]] double to_double() const override {
        return value;
    }


    [[nodiscard]] std::string to_string() const override;
};

class StringValue : public Value {
    std::string value;
public:
    explicit StringValue(std::string value) : Value(ValueType::String), value(std::move(value)) {
    }

    [[nodiscard]] std::string to_string() const override;
};


class BoolValue : public Value {
    bool value;
public:
    explicit BoolValue(bool value) : Value(ValueType::Bool), value(value) {
    }

    [[nodiscard]] std::string to_string() const override;

    [[nodiscard]] bool is_true() const override {
        return value;
    }
};


class NilValue : public Value {
public:
    NilValue() : Value(ValueType::Nil) {
    }

    [[nodiscard]] std::string to_string() const override;
};



// class Complex : public Number {}; # TODO
// class Rational : public Number {};

#endif //SCHEME_TYPES_H

#ifndef SCHEME_CLOSURE_H
#define SCHEME_CLOSURE_H


#include "types.h"
#include "environment.h"
#include "../util.h"
#include <vector>
#include <string>

class Closure : public Value {
    EnvironmentPtr env_;
    std::shared_ptr<ListValue> formal_params_; // TODO make this normal list of strings of names
    std::shared_ptr<Value> body_;

public:
    Closure(const EnvironmentPtr &env, std::shared_ptr<ListValue> formal_params,
            std::shared_ptr<Value> body) : Value(ValueType::Closure), env_(env), formal_params_(formal_params),
                                           body_(body) {
    };

    ValuePtr call(const std::vector<ValuePtr> &args);

    ValueType get_type() const {
        return ValueType::Closure;
    }

    std::string to_string() const override {
        return "#<Closure>";
    }

    ValuePtr get_body() const {
        return body_;
    }

    EnvironmentPtr get_env() const {
        return env_;
    }

    std::shared_ptr<ListValue> get_formal_params() const {
        return formal_params_;
    }
};


#endif //SCHEME_CLOSURE_H

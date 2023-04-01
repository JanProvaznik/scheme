#ifndef SCHEME_ENVIRONMENT_H
#define SCHEME_ENVIRONMENT_H

#include "../printer.h"
#include "../reader.h"
//#include "types.h"
#include <string>
#include <sstream>
#include <fstream>
#include <map>
#include <iostream>


class Environment : public Value {
public:
    void set(const std::string &key, const std::shared_ptr<Value> &value);

    Environment() : Value(ValueType::Environment), outer(nullptr) {
    }

    explicit Environment(std::shared_ptr<Environment> outer) : Value(ValueType::Environment), outer(outer) {
    }

    Environment(std::shared_ptr<Environment> outer, const std::shared_ptr<ListValue> &binds,
                const std::shared_ptr<ListValue> &exprs);


    std::shared_ptr<Value> get(const std::string &key) const;

    std::shared_ptr<Environment> find(const std::string &key);

    std::shared_ptr<Environment> get_outer() const;

    void update_existing(const std::string &key, const std::shared_ptr<Value> &value);

protected:
    std::map<std::string, std::shared_ptr<Value> > data;
    std::shared_ptr<Environment> outer;


};

class BaseEnvironment : public Environment {

public:
    BaseEnvironment();
};

#endif //SCHEME_ENVIRONMENT_H

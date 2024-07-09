#pragma once
#include <unordered_map>
#include <string>
#include <variant>
struct EDNObject;
struct EDNValue;
struct EDNNull{};
struct EDNObject{
    std::unordered_map<std::string, EDNValue*> object;
};

struct EDNValue{
    std::variant<long, double, std::string, bool, EDNNull, EDNObject*> value;
};



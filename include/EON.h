#pragma once
#include <unordered_map>
#include <string>
#include <variant>
struct DONObject;
struct DONValue;
struct DONNull{};
struct DONObject{
    std::unordered_map<std::string, DONValue*> object;
};

struct DONValue{
    std::variant<long, double, std::string, bool, DONNull, DONObject*> value;
};



#pragma once
#include <unordered_map>
#include <string>
#include <variant>
#include <vector>
struct DONObject;
struct DONValue;
struct DONNull{};
struct DONArray{
    std::vector<DONValue> values;
};
struct DONObject{
    std::unordered_map<std::string, DONValue*> object;
};

struct DONValue{
    std::variant<long, double, std::string, bool, DONNull, DONArray, DONObject*> value;
};



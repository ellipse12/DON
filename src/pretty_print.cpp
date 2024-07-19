#include "pretty_print.h"
#include <iostream>
#include <map>
std::string pprint(DONObject* object){
    std::string out = "";
    auto obj = object->object; 
    out += "{";
    int i = 0;
    for(auto [key, value] : obj){
        i++;
        out += key + ": " + pprint(*value);
        if(obj.size() - 1 > i - 1){
            out += ", ";
        }
    }
    out += "}";
    return out;
}
std::string pprint(DONValue object){
    std::string out = "";
    DONValue value = object;
    if(std::holds_alternative<DONNull>(value.value)){
        out += "null";
    }else if(std::holds_alternative<long>(value.value)){
        out += std::to_string(std::get<long>(value.value));
    }else if(std::holds_alternative<double>(value.value)){
        out += std::to_string(std::get<double>(value.value));
    }else if(std::holds_alternative<std::string>(value.value)){
        out += std::get<std::string>(value.value);
    }else if(std::holds_alternative<bool>(value.value)){
        out += std::get<bool>(value.value) ? "true" : "false";
    }else if(std::holds_alternative<DONObject*>(value.value)){
        out += pprint(std::get<DONObject*>(value.value));
    }else if(std::holds_alternative<DONArray>(value.value)){
        out += "[";
        DONArray arr = std::get<DONArray>(value.value);
        if(arr.values.size() <= 0) return "[]";
        for(int i = 0; i < arr.values.size() - 1; i++){
            out += pprint(arr.values[i]);
            out += ", ";
        }
        out += pprint(arr.values[arr.values.size() - 1]);
        out += "]";
    }
    return out;
}


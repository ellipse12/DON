#include "pretty_print.h"
#include <iostream>
#include <map>
std::string pprint(DONObject* object){
    std::string out = "";
    auto obj = object->object; 
    out += "{";
    for(auto [key, value] : obj){
        out += key + ": " + pprint(*value) + ", ";
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
    }
    return out;
}


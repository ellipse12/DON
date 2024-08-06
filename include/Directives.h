#pragma once
#include <DON.h>
struct Directive{
    DONObject* (*func)(DONObject*, size_t argc, DONValue* argv, DONObject*);
};

struct DONContext{
    std::unordered_map<std::string, Directive*> directives;
    DONObject* replacements;
    void register_directive(std::string name, Directive* directive);
    void remove_directive(std::string name);
    DONValue* get_replacement(std::string name);
    void add_replacement(std::string, DONValue* value);
    void remove_replacement(std::string name);
    bool contains_replacement(std::string name);
    Directive get_directive(std::string name);
};

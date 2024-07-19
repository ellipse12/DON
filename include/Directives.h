#pragma once
#include <string>
#include <vector>
#include <DON.h>
#include <parser.h>

struct Directive{
    DONObject* (*func)(DONObject* document, std::vector<DONObject> args, DONObject* replacements);
};

struct DONContext{
    std::unordered_map<std::string, Directive> directives;
    DONObject* replacements;
    bool register_directive(std::string name, Directive directive);
    void remove_directive(std::string name);
    DONValue get_replacement(std::string name);
    void add_replacement(std::string, DONValue value);
    void remove_replacement(std::string name);
};

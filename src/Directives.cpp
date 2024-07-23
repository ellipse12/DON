#include "Directives.h"

void DONContext::add_replacement(std::string name, DONValue* value){
    DONContext::replacements->object.insert({name, value});
}

DONValue* DONContext::get_replacement(std::string name){
    return DONContext::replacements->object[name];
}

void DONContext::register_directive(std::string name, Directive* directive){
    DONContext::directives.insert({name, directive});
}
void DONContext::remove_directive(std::string name){
    DONContext::directives.erase(name);
}

Directive DONContext::get_directive(std::string name){
    return *DONContext::directives[name];
}

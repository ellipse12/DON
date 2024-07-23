#include "DON.h"
#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "pretty_print.h"
#include "Directives.h"
#include <memory>
DONObject* test(DONObject* document, size_t argc, DONValue* argv, DONObject* replacement){
    if(argc <= 0){
        std::cout << "oops!!" << std::endl;
        return new DONObject{};
    }
    std::cout << "arg2: " << pprint(argv[0]) << std::endl;
    return new DONObject{};
}
int main(int argc, char* argv[]){
    if(argc <= 1){
        throw "edn requires an argument.";
    }
    std::stringstream data;
    std::ifstream in(argv[1]);
    data << in.rdbuf();
    DONContext* ctx = new DONContext;
    ctx->register_directive("test", new Directive{&test});
    DONObject* object = writes(data.str(), ctx);
    //std::cout << pprint(object) << std::endl;
    return 0;
}

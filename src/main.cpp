#include "DON.h"
#include "parser.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include "pretty_print.h"
int main(int argc, char* argv[]){
    if(argc <= 1){
        throw "edn requires an argument.";
    }
    std::stringstream data;
    std::ifstream in(argv[1]);
    data << in.rdbuf();
    DONObject* object = writes(data.str() + "\0");
    std::cout << pprint(object) << std::endl;
    return 0;
}

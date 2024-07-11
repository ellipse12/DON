#include "parser.h"
#include <variant>
#include <cctype>
#include <vector>
#include <iostream>
#include "pretty_print.h"
#include <sstream>
enum TokenType{
    LITERAL,
    ID,
    ASSIGN,
//  ARRAY,
    LBRACK,
    RBRACK
//    DIRECTIVE
};

struct Token{
    TokenType type;
    DONValue value;
    int line;
};



long parse_hex(std::string input, int* i){
    int current = *i;
    std::string temp = "";
    while(std::isxdigit(input[current])){
        temp += input[current++];
        std::cout << input[current];
    }
    *i = current;
    return std::stol(temp.c_str(), 0, 16);
}
long parse_octal(std::string input, int* i){
    int current = *i;
    std::string temp = "";
    while(input[current] >= '0' && input[current] <= '7'){
        temp += input[current++];
    }
    *i = current;
    return std::stol(temp.c_str(), 0, 8);
}
long parse_bin(std::string input, int* i){
    int current = *i;
    std::string temp = "";
    while(input[current] == '0' || input[current] == '1'){
        temp += input[current++];
    }
    *i = current;
    return std::stol(temp.c_str(), 0, 2);
}

std::vector<Token> lex(std::string input){
    std::string temp = "";
    std::vector<Token> tokens;
    int line = 0;
    for(int i = 0; i < input.size(); i++){
        if(input[i] == '\n'){
            line++;
            continue;
        }
        if(input[i] == '/' && input[i+1] == '/'){
            while(input[++i] != '\n'){}
        }
        if(std::isspace(static_cast<unsigned char>(input[i]))){continue;}
        if(std::isalpha(static_cast<unsigned char>(input[i])) || input[i] == '_'){
            int k = i;
            while(isalnum(static_cast<unsigned char>(input[k])) || input[k] == '_' || input[k] == '-'){
                temp += input[k++];
            }
            if(temp.compare("true") == 0){
                tokens.push_back(Token{LITERAL, true, line});
            }else if(temp.compare("false") == 0){
                tokens.push_back(Token{LITERAL, false, line});
            }else if(temp.compare("null") == 0){
                tokens.push_back(Token{LITERAL, DONNull{}, line});
            }else{
                tokens.push_back(Token{ID, temp, line});
            }
            temp.clear();
            i = k;
            continue;
        }
        if(std::isdigit(static_cast<unsigned char>(input[i])) || input[i] == '.' || input[i] == '-'){
            std::string temp = "";
            bool is_float = false;
            int negative = 1;
            if(input[i] == '-'){
                negative = -1;
                i++;
            }
            if(input[i] == '0'){
                switch(input[i + 1]){
                    case 'x': {
                        i += 2;
                        tokens.push_back(Token{LITERAL, parse_hex(input, &i)});
                        continue;
                    }
                    case 'o': {
                        i += 2;
                        tokens.push_back(Token{LITERAL, parse_octal(input, &i)});
                        continue;
                    }
                    case 'b': {
                        i += 2;
                        tokens.push_back(Token{LITERAL, parse_bin(input, &i)});
                        continue;
                    }
                }
                continue;
            }
            while(std::isdigit(static_cast<unsigned char>(input[i])) || input[i] == '.'){
                if(input[i] == '.' && !is_float){
                    is_float = true;
                }else if(input[i] == '.' && is_float){
                    std::cout << "Too many decimal points\n";
                    throw ("too many decimal points");
                }
                temp += input[i++];
            }
            
            if(is_float){
                tokens.push_back(Token{LITERAL, negative * stof(temp), line});
            }else{
                tokens.push_back(Token{LITERAL, negative * stol(temp), line});
            }
            continue;
        
        }

        switch (input[i]){
            case '=': tokens.push_back(Token{ASSIGN, DONNull{}}); break;
            case '{': tokens.push_back(Token{LBRACK, DONNull{}}); break;
            case '}': tokens.push_back(Token{RBRACK, DONNull{}}); break;
            case '\"': {
                           int k = i + 1;
                           std::string temp = "";
                           while(input[k] != '\"' && input[k] != '\n' && input[k] != '\0'){
                                temp += input[k++];
                           }
                           if(input[k] == '\n') line++;
                           tokens.push_back(Token{LITERAL, temp, line});
                           i = k;
                           break;
                       }
        }
    }
    return tokens;
}

std::string print_token_type(TokenType ttype){
    std::string type = "";
    switch (ttype){
        case LITERAL: type = "LITERAL"; break;
        case ID: type = "ID"; break;
        case ASSIGN: type = "="; break;
        case LBRACK: type = "{"; break;
        case RBRACK: type = "}"; break;
    }
    return type;
}

std::string print_token(Token token){
    std::string type = print_token_type(token.type);
    std::string out = "";
    out += "{\n    \"" + type + "\", ";
    DONValue value = token.value;
    if(std::holds_alternative<DONNull>(value.value)){
        out += "null\n";
    }else if(std::holds_alternative<long>(value.value)){
        out += std::to_string(std::get<long>(value.value)) + "\n";
    }else if(std::holds_alternative<double>(value.value)){
        out += std::to_string(std::get<double>(value.value)) + "\n";
    }else if(std::holds_alternative<std::string>(value.value)){
        out += std::get<std::string>(value.value) + "\n";
    }else if(std::holds_alternative<bool>(value.value)){
        out += std::to_string(std::get<bool>(value.value)) + "\n";
    }
    out += "}\n";
    return out;
}

struct Scanner{
    int cursor;
    std::vector<Token> tokens;
    Token peek(){
        return tokens.at(cursor);
    }
    Token consume(){
        cursor++;
        return tokens.at(cursor-1);
    }
    Token get(){
        return tokens.at(cursor-1);
    }
    Token expect(TokenType type){
        if(peek().type == type){
            return consume();
        }
        std::cout << "Expect Error: expected token: " + print_token_type(type) + ", got: " + print_token(peek()) + ", line: " + std::to_string(peek().line) << std::endl;
        throw "Expect Error";
    }
    bool is_next(TokenType type){
        return peek().type == type;
    }
    bool next_if(TokenType type){
        if(peek().type == type){
            consume();
            return true;
        }
        return false;
    }
    bool has_next(){
        return tokens.size() > cursor + 1;
    }
};


DONValue* expr(Scanner* scanner);

void assign(Scanner* scanner, DONObject* document){
    if(scanner->is_next(ID)){
        std::string id = std::get<std::string>(scanner->consume().value.value);
        scanner->expect(ASSIGN);
        document->object.insert_or_assign(id, expr(scanner));
    }
}
DONValue* expr(Scanner* scanner){
    if(scanner->next_if(LBRACK)){
        DONObject* out = new DONObject;
        while(!scanner->is_next(RBRACK)) assign(scanner, out);
        scanner->expect(RBRACK);
        DONValue* out2 = new DONValue();
        out2->value = out;
        return out2;
    }
    DONValue* outt = new DONValue{scanner->consume().value};
    return outt;

}

DONObject* writes(std::string input){
    DONObject* out = new DONObject;
    parse(input, out);
    return out;
}

void parse(std::string input, DONObject* document){
    std::vector<Token> tokens = lex(input);
    Scanner scanner{0, tokens};
    while(scanner.has_next())
        assign(&scanner, document);
   
}

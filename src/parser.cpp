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
    EDNValue value;
    int line;
};


std::vector<Token> lex(std::string input){
    std::string temp = "";
    std::vector<Token> tokens;
    int line = 0;
    for(int i = 0; i < input.size(); i++){
        if(input[i] == '\n'){
            line++;
            continue;
        }
        if(std::isspace(static_cast<unsigned char>(input[i]))){continue;}
        if(std::isalpha(static_cast<unsigned char>(input[i]))){
            int k = i;
            while(isalnum(static_cast<unsigned char>(input[k]))){
                temp += input[k++];
            }
            if(temp.compare("true") == 0){
                tokens.push_back(Token{LITERAL, true, line});
            }else if(temp.compare("false") == 0){
                tokens.push_back(Token{LITERAL, false, line});
            }else if(temp.compare("null") == 0){
                tokens.push_back(Token{LITERAL, EDNNull{}, line});
            }else{
                tokens.push_back(Token{ID, temp, line});
            }
            temp.clear();
            i = k;
            continue;
        }
        if(std::isdigit(static_cast<unsigned char>(input[i])) || input[i] == '.'){
            std::string temp = "";
            bool is_float = false;
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
                tokens.push_back(Token{LITERAL, stof(temp), line});
            }else{
                tokens.push_back(Token{LITERAL, stol(temp), line});
            }
            continue;
        
        }

        switch (input[i]){
            case '=': tokens.push_back(Token{ASSIGN, EDNNull{}}); break;
            case '{': tokens.push_back(Token{LBRACK, EDNNull{}}); break;
            case '}': tokens.push_back(Token{RBRACK, EDNNull{}}); break;
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
    EDNValue value = token.value;
    if(std::holds_alternative<EDNNull>(value.value)){
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


EDNValue* expr(Scanner* scanner);

void assign(Scanner* scanner, EDNObject* document){
    if(scanner->is_next(ID)){
        std::string id = std::get<std::string>(scanner->consume().value.value);
        scanner->expect(ASSIGN);
        document->object.insert_or_assign(id, expr(scanner));
    }
}
EDNValue* expr(Scanner* scanner){
    if(scanner->next_if(LBRACK)){
        EDNObject* out = new EDNObject;
        while(!scanner->is_next(RBRACK)) assign(scanner, out);
        scanner->expect(RBRACK);
        EDNValue* out2 = new EDNValue();
        out2->value = out;
        return out2;
    }
    EDNValue* outt = new EDNValue{scanner->consume().value};
    return outt;

}

EDNObject* writes(std::string input){
    EDNObject* out = new EDNObject;
    parse(input, out);
    return out;
}

void parse(std::string input, EDNObject* document){
    std::vector<Token> tokens = lex(input);
    Scanner scanner{0, tokens};
    while(scanner.has_next())
        assign(&scanner, document);
   
}

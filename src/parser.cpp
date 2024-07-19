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
    LBRACK,
    RBRACK,
    LSQ,
    RSQ,
    PREDIR,
    COMMA,
    NONE
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
    }
    *i = current;
    return std::stol(temp.c_str(), 0, 16);
}
bool check_octal(char c){
    if(std::isdigit(static_cast<unsigned char>(c)) && c != '8' && c != '9'){
        return true;
    }
    if(c=='8' || c=='9'){
        std::cout << "Unkown octal digit: \'" << c << "\'\n";
        throw ("Parse error");
        
    }
    return false;
}
long parse_octal(std::string input, int* i){
    int current = *i;
    std::string temp = "";
    while(check_octal(input[current])){
        temp += input[current++];
    }
    *i = current;
    return std::stol(temp.c_str(), 0,8);
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
    std::vector<Token> tokens;
    int line = 0;
    for(int i = 0; i < input.size(); i++){
        if(input[i] == '\n'){
            line++;
            continue;
        }
        if(input[i] == '/' && input[i+1] == '/'){
            while(input[i+1] != '\n'){i++;}
            line++;
            continue;
        }
        switch (input[i]){
            case '=': tokens.push_back(Token{ASSIGN, DONNull{}}); continue;
            case '{': tokens.push_back(Token{LBRACK, DONNull{}}); continue;
            case '}': tokens.push_back(Token{RBRACK, DONNull{}}); continue;
            case '[': tokens.push_back(Token{LSQ, DONNull{}}); continue;
            case ']': tokens.push_back(Token{RSQ, DONNull{}}); continue;
            case ',': tokens.push_back(Token{COMMA, DONNull{}}); continue;
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
        if(std::isspace(static_cast<unsigned char>(input[i]))){continue;}
        if(input[i] == '#'){
            std::string temp = "";
            TokenType out = PREDIR;
            i++;
            while(!std::isspace(static_cast<unsigned char>(input[i]))){
                temp += input[i++];
            }
            tokens.push_back(Token{out, temp, line});
            continue;
        }
        if(std::isalpha(static_cast<unsigned char>(input[i])) || input[i] == '_'){
            std::string temp = "";
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
            i = k - 1;
            continue;
        }
        if(std::isdigit(input[i]) || input[i] == '.' || input[i] == '-'){
            std::string temp2 = "";
            bool is_float = false;
            int negative = 1;
            if(input[i] == '-' && std::isdigit(static_cast<unsigned char>(input[i+1]))){
                negative = -1;
                i++;
            }
            if(input[i] == '0'){
                switch(input[i + 1]){
                    case 'x': {
                        i += 2;
                        tokens.push_back(Token{LITERAL, parse_hex(input, &i)});
                        i--;
                        continue;
                    }
                    case 'o': {
                        i += 2;
                        tokens.push_back(Token{LITERAL, parse_octal(input, &i)});
                        i--;
                        continue;
                    }
                    case 'b': {
                        i += 2;
                        tokens.push_back(Token{LITERAL, parse_bin(input, &i)});
                        i--;
                        continue;
                    }
                }
            }
            while(std::isdigit(static_cast<unsigned char>(input[i])) || input[i] == '.'){
                if(input[i] == '.' && !is_float){
                    is_float = true;
                }else if(input[i] == '.' && is_float){
                    std::cout << "Too many decimal points\n";
                    throw ("too many decimal points");
                }
                temp2 += input[i++];
            }
            if(is_float){
                tokens.push_back(Token{LITERAL, negative * std::stof(temp2), line});
            }else{
                tokens.push_back(Token{LITERAL, negative * std::stol(temp2), line});
            }
            i--;
            continue;
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
        case LSQ: type = "["; break;
        case RSQ: type = "]"; break;
        case COMMA: type = ","; break;
        case PREDIR: type = "DIRECTIVE"; break;
        case NONE: type = "NONE"; break;
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
    }else if(std::holds_alternative<DONArray>(value.value)){
        out += "[array]";
    }
    out += "}\n";
    return out;
}

struct Scanner{
    int cursor;
    std::vector<Token> tokens;
    DONObject* replacements;
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
        std::cout << "Expect Error: expected token: \'" + print_token_type(type) + "\', got: \'" + print_token(peek()) + "\', line: " + std::to_string(peek().line) << std::endl;
        throw "Expect Error";
    }
    bool is_next(TokenType type){
        return peek().type == type;
    }
    Token next_if(TokenType type){
        if(peek().type == type){
            return consume();
            
        }
        return Token{NONE, DONNull{}};
    }
    bool has_next(){
        return tokens.size() > cursor + 1;
    }
};


void assign(Scanner* scanner, DONObject* document);
void directive(Scanner* scanner, DONObject* document);

void expr(Scanner* scanner, DONObject* document){
    if(scanner->is_next(ID)){
        assign(scanner, document);
    }else if(scanner->is_next(PREDIR)){
        directive(scanner, document);
    }
}

DONValue* literal(Scanner* scanner){
    if(scanner->is_next(LBRACK)){
        scanner->consume();
        DONObject* out = new DONObject;
        while(!scanner->is_next(RBRACK)) expr(scanner, out);
        scanner->expect(RBRACK);
        DONValue* outt = new DONValue{out};
        return outt;
    }else if(scanner->is_next(LSQ)){
        scanner->consume();
        DONArray* arr = new DONArray;
        while(!scanner->is_next(RSQ)) {
            DONValue* lit = literal(scanner);
            arr->values.push_back(*lit);
            if(!scanner->is_next(RSQ)){
                scanner->expect(COMMA);
            }
        }
        scanner->expect(RSQ);
        DONValue* out = new DONValue{*arr};
        return out;
    }
    DONValue* out = new DONValue{scanner->consume().value};
    return out;

}
void directive(Scanner* scanner, DONObject* document){
    if(scanner->is_next(PREDIR)){
        std::string id = std::get<std::string>(scanner->consume().value.value);
        DONObject* values = new DONObject;
        values->object.insert_or_assign(id, literal(scanner));
        
    }
}
void assign(Scanner* scanner, DONObject* document){
    std::string id = std::get<std::string>(scanner->expect(ID).value.value);
    scanner->expect(ASSIGN);
    document->object.insert_or_assign(id, literal(scanner));
}

void parse(std::string input, DONObject* document){
    std::vector<Token> tokens = lex(input);
    Scanner* scanner = new Scanner{0, tokens, new DONObject};
    //for(Token token: tokens){
        //std::cout << print_token(token) << std::endl;
    //}
    while(scanner->has_next())
        expr(scanner, document);
   
}
DONObject* writes(std::string input){
    DONObject* out = new DONObject;
    parse(input, out);
    return out;
}

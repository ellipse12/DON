#pragma once
#include "EDN.h"
#include <string>
void parse(std::string input, EDNObject* document);
EDNObject* writes(std::string input);

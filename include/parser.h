#pragma once
#include "DON.h"
#include "Directives.h"
#include <string>
void parse(std::string input, DONObject* document);
DONObject* writes(std::string input, DONContext* ctx);

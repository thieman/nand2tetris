#include <iostream>

#include "parse.hpp"

namespace vm {
    void vm(std::string input, std::string output) {
        auto parsed_instructions = vmParse::parseFile(std::move(input));        
    }   
}
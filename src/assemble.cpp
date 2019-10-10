#include <iostream>
#include <map>
#include <variant>

#include "parse.hpp"
#include "overloaded.hpp"

namespace assemble {
  using SymbolMap = std::map<std::string, int>;

  const SymbolMap built_in_symbols = {
    {"SP", 0},
    {"LCL", 1},
    {"ARG", 2},
    {"THIS", 3},
    {"THAT", 4},
    {"R0", 0}, {"R1", 1}, 
    {"R0", 0}, {"R1", 1}, {"R2", 0}, {"R3", 1}, {"R4", 0}, {"R5", 1}, {"R6", 0}, {"R7", 1}, 
    {"R8", 0}, {"R9", 1}, {"R10", 0}, {"R11", 1}, {"R12", 0}, {"R13", 1}, {"R14", 0}, {"R15", 1},
    {"SCREEN", 16384},
    {"KBD", 24576},
  };

  SymbolMap buildUserSymbols(std::vector<parse::Instruction> instructions) {
    SymbolMap table;

    uint current_address = 0;
    for (auto instruction : instructions) {
      std::visit(overloaded {
        [&current_address](parse::AInstruction) { current_address++; },                
        [&current_address](parse::CInstruction) { current_address++; },
        [&table, &current_address](parse::Label label) { table.insert(std::pair<std::string, int>(label.name, current_address)); }
      }, instruction);      
    }

    return table;
  }

  int resolve_symbol(const SymbolMap& user_symbols, std::string name) {
    auto builtin_hit = built_in_symbols.find(name);
    if (builtin_hit != built_in_symbols.end()) { return builtin_hit->second; }
    auto user_hit = user_symbols.find(name);
    if (user_hit != user_symbols.end()) { return user_hit->second; }
    throw std::out_of_range("Could not find reference to symbol " + name);
  }

  std::vector<std::string> assemble_to_strings(std::vector<parse::Instruction> instructions, SymbolMap user_symbols) {
    std::vector<std::string> results;

    for (auto inst: instructions) {
      if (auto i = std::get_if<parse::AInstruction>(&inst)) {
        int address = isalpha(i->value[0]) ? resolve_symbol(user_symbols, i->value) : stoi(i->value);
        std::cout << address << std::endl;
      }
    }

    return results;
  }

  void assemble(std::string input, std::string output) {
    auto parsed_instructions = parse::parseFile(std::move(input));

    SymbolMap user_symbols = buildUserSymbols(parsed_instructions);

    auto assembled = assemble_to_strings(parsed_instructions, user_symbols);
    
  }
}

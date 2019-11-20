#include <bitset>
#include <fstream>
#include <iostream>
#include <map>
#include <variant>

#include "parse.hpp"
#include "overloaded.hpp"

namespace assemble {
  using SymbolMap = std::map<std::string, int>;
  using BitLookupMap = std::map<std::string, int>;

  const SymbolMap built_in_symbols = {
    {"SP", 0},
    {"LCL", 1},
    {"ARG", 2},
    {"THIS", 3},
    {"THAT", 4},
    {"R0", 0}, {"R1", 1}, {"R2", 2}, {"R3", 3}, {"R4", 4}, {"R5", 5}, {"R6", 6}, {"R7", 7}, 
    {"R8", 8}, {"R9", 9}, {"R10", 10}, {"R11", 11}, {"R12", 12}, {"R13", 13}, {"R14", 14}, {"R15", 15},
    {"SCREEN", 16384},
    {"KBD", 24576},
  };

  const BitLookupMap comp_map = {
    {"0", 0b0101010},
    {"1", 0b0111111},
    {"-1", 0b0111010},
    {"D", 0b0001100},
    {"A", 0b0110000},
    {"!D", 0b0001101},
    {"!A", 0b0110011},
    {"-D", 0b0001111},
    {"-A", 0b0110011},
    {"D+1", 0b0011111},
    {"A+1", 0b0110111},
    {"D-1", 0b0001110},
    {"A-1", 0b0110010},
    {"D+A", 0b0000010},
    {"D-A", 0b0010011},
    {"A-D", 0b0000111},
    {"D&A", 0b0000000},
    {"D|A", 0b0010101},
    {"M", 0b1110000},
    {"!M", 0b1110001},
    {"-M", 0b1110011},
    {"M+1", 0b1110111},
    {"M-1", 0b1110010},
    {"D+M", 0b1000010},
    {"D-M", 0b1010011},
    {"M-D", 0b1000111},
    {"D&M", 0b1000000},
    {"D|M", 0b1010101},  
  };

  const BitLookupMap dest_map {
    {"M", 0b001},
    {"D", 0b010},
    {"MD", 0b011},
    {"A", 0b100},
    {"AM", 0b101},
    {"AD", 0b110},
    {"AMD", 0b111},
  };

  SymbolMap buildUserSymbols(std::vector<parse::Instruction> instructions) {
    SymbolMap table;
    table["__variables_defined"] = 0;

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

  int resolve_symbol(SymbolMap& user_symbols, std::string name) {
    auto builtin_hit = built_in_symbols.find(name);
    if (builtin_hit != built_in_symbols.end()) { return builtin_hit->second; }
    auto user_hit = user_symbols.find(name);
    if (user_hit != user_symbols.end()) { return user_hit->second; }

    // Variable symbols. Any symbol Xxx appearing in an assembly program that is
    // not predefined and is not defined elsewhere using the (Xxx) command is treated as
    // a variable. Variables are mapped to consecutive memory locations as they are first
    // encountered, starting at RAM address 16 (0x0010).
    int variables_defined = user_symbols.at("__variables_defined");
    const int variable_value = 16 + variables_defined;
    user_symbols.insert(std::pair<std::string, int>(name, variable_value));
    user_symbols["__variables_defined"]++;
    return variable_value;
  }

  std::vector<std::string> assemble_to_strings(std::vector<parse::Instruction> instructions, SymbolMap user_symbols) {
    std::vector<std::string> results;

    for (auto inst: instructions) {
      if (auto i = std::get_if<parse::AInstruction>(&inst)) {
        int address = isalpha(i->value[0]) ? resolve_symbol(user_symbols, i->value) : stoi(i->value);

        std::bitset<16> a_inst = address;
        a_inst.set(15, false);
        results.push_back(a_inst.to_string());
      } 
      
      else if (auto i = std::get_if<parse::CInstruction>(&inst)) {
        std::bitset<16> c_inst = i->jump.has_value() ? i->jump.value() + 1 : 0;

        auto comp = comp_map.find(i->comp);
        if (comp == comp_map.end()) { 
          throw std::out_of_range("Invalid comp " + i->comp);
        }
        c_inst |= comp->second << 6;

        if (i->dest.has_value()) {
          auto dest = dest_map.find(i->dest.value());
          if (dest == dest_map.end()) {
            throw std::out_of_range("Invalid dest " + i->dest.value());
          }
          c_inst |= dest->second << 3;
        }  

        c_inst |= 0b1110000000000000;
        results.push_back(c_inst.to_string());
      }
    }

    return results;
  }

  void assemble(std::string input, std::string output) {
    auto parsed_instructions = parse::parseFile(std::move(input));

    SymbolMap user_symbols = buildUserSymbols(parsed_instructions);

    auto assembled = assemble_to_strings(parsed_instructions, user_symbols);

    std::ofstream output_file(output, std::ofstream::out | std::ofstream::trunc);
    if (!output_file.is_open()) {
      throw std::invalid_argument("Could not find file" + output);
    }    

    for (auto line : assembled) {
      output_file << line << std::endl;
    }
  }
}

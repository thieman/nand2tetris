#include <fstream>
#include <iostream>
#include <string>
#include <variant>
#include <optional>
#include <map>

#include <boost/format.hpp>
#include "overloaded.hpp"

/* 
Grammar
A-instruction: @value
C-instruction: dest=comp;jump
               Either dest or jump may be empty
               If dest is empty, omit =
               If jump is empty, omit ;
Label: (labelname)
*/

namespace parse {
  enum Jump { JGT, JEQ, JGE, JLT, JNE, JLE, JMP };

  std::map<std::string, Jump> jump_lookup {
      {"JGT", Jump::JGT},
      {"JEQ", Jump::JEQ},
      {"JGE", Jump::JGE},
      {"JLT", Jump::JLT},
      {"JNE", Jump::JNE},
      {"JLE", Jump::JLE},
      {"JMP", Jump::JMP},
  };

  struct AInstruction {
    std::string value;
  };

  struct CInstruction {
    std::optional<std::string> dest;
    std::string comp;
    std::optional<Jump> jump;
  };

  struct Label {
    std::string name;
  };

  using Instruction = std::variant<AInstruction, CInstruction, Label>;

  void print(Instruction instruction) {
    return std::visit(overloaded {
      [](AInstruction a) { std::cout << boost::format("AInstruction {value %s}") % a.value << std::endl; },
      [](CInstruction c) { std::cout << boost::format("CInstruction {dest %s comp %s jump %d}")
      % c.dest.value_or(".")
      % c.comp
      % (c.jump.has_value() ? c.jump.value() : -1)
      << std::endl; },
      [](Label l) { std::cout << boost::format("Label {name %s}") % l.name << std::endl; },
    }, instruction);
  }

  std::optional<Instruction> parseLine(std::string line) {
    // Get rid of whitespace and comments and junk
    line.erase(std::remove_if(line.begin(), line.end(), isspace), line.end());
    auto from_comment_char = line.find("//");
    if (from_comment_char != std::string::npos) {
      line.erase(from_comment_char);
    }

    if (line.length() == 0) { return std::nullopt; }

    if (line[0] == '@') {
      return AInstruction {line.substr(1)};
    } else if (line[0] == '(') {
      return Label { line.substr(1, line.length() - 2) };
    } else {
      CInstruction cinst {};
      auto equalPos = line.find("=");
      if (equalPos != std::string::npos) {
        cinst.dest = std::optional(line.substr(0, equalPos));
    }

      auto nextStart = equalPos == std::string::npos ? 0 : equalPos + 1;
      auto semicolonPos = line.find(";");
      cinst.comp = line.substr(nextStart, semicolonPos - nextStart);

      if (semicolonPos != std::string::npos) {
        auto jump_inst = line.substr(semicolonPos + 1);
        auto jump_enum = jump_lookup.find(jump_inst);
        if (jump_enum == jump_lookup.end()) {
          throw std::out_of_range("Invalid Jump instruction " + jump_inst);
        }
        cinst.jump = std::optional(jump_enum->second);
      }

      return cinst;
    }
    
    std::cout << line << std::endl;
    return std::nullopt;
  }

  std::vector<Instruction> parseFile(std::string input_filepath) {
    std::vector<Instruction> parsed;

    std::ifstream input_file {input_filepath};
    if (!input_file.is_open()) {
      throw std::invalid_argument("Could not find file" + input_filepath);
    }

    std::string line;
    while (std::getline(input_file, line)) {
      auto parsed_line = parseLine(line);
      if (parsed_line.has_value()) {
        print(parsed_line.value());
        parsed.push_back(parsed_line.value());
      }
    }    

    return parsed;
  }
}
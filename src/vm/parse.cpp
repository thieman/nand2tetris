#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <optional>
#include <variant>
#include <vector>

#include <boost/algorithm/string.hpp>
#include <boost/format.hpp>

#include "overloaded.hpp"

/* 
Grammar

Arithmetic and Logic (no args)
add
sub
neg
eq
gt
lt
and
or
not

Memory access commands
pop segment i
push segment i
i is non-negative integer

Valid segments
local
argument
this
that
constant
static
temp
pointer
*/

namespace vmParse {
    enum LogicCommand { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };

    std::map<std::string, LogicCommand> logicCommandLookup {
        {"add", LogicCommand::ADD},
        {"sub", LogicCommand::SUB},
        {"neg", LogicCommand::NEG},
        {"eq", LogicCommand::EQ},
        {"gt", LogicCommand::GT},
        {"lt", LogicCommand::LT},
        {"and", LogicCommand::AND},
        {"or", LogicCommand::OR},
        {"not", LogicCommand::NOT},
    };

    enum MemoryCommand { PUSH, POP };
    std::map<std::string, MemoryCommand> memoryCommandLookup {
        {"push", MemoryCommand::PUSH},
        {"pop", MemoryCommand::POP},
    };

    enum MemorySegment { LOCAL, ARGUMENT, THIS, THAT, CONSTANT, STATIC, TEMP, POINTER };
    std::map<std::string, MemorySegment> memorySegmentLookup {
        {"local", MemorySegment::LOCAL},
        {"argument", MemorySegment::ARGUMENT},
        {"this", MemorySegment::THIS},
        {"that", MemorySegment::THAT},
        {"constant", MemorySegment::CONSTANT},
        {"static", MemorySegment::STATIC},
        {"temp", MemorySegment::TEMP},
        {"pointer", MemorySegment::POINTER},  
    };

    struct LogicBytecode {
        LogicCommand command;
    };

    struct MemoryBytecode {
        MemoryCommand command;
        MemorySegment segment;
        unsigned int value;
    };

    using Bytecode = std::variant<LogicBytecode, MemoryBytecode>;    
        
    void print(Bytecode bytecode) {
        return std::visit(overloaded {
            [](LogicBytecode l) { std::cout << boost::format("LogicBytecode {command %s}") % l.command << std::endl; },
            [](MemoryBytecode m) { std::cout << boost::format("MemoryBytecode {command %s segment %s value %d}")
            % m.command
            % m.segment
            % m.value
            << std::endl; },
            }, bytecode);
    };

    std::optional<Bytecode> parseLine(std::string line) {
        // Get rid of comments, then strip whitespace from ends
        auto from_comment_char = line.find("//");
        if (from_comment_char != std::string::npos) {
            line.erase(from_comment_char);
        }
        boost::trim(line);

        if (line.length() == 0) { return std::nullopt; }

        std::vector<std::string> words;
        boost::split(words, line, [](char c){return c == ' ';});

        if (auto v = logicCommandLookup.find(words[0]); v != logicCommandLookup.end()) {
            return LogicBytecode {v->second};
        }

        if (auto v = memoryCommandLookup.find(words[0]); v != memoryCommandLookup.end()) {
            auto segment = memorySegmentLookup.find(words[1]);
            if (segment == memorySegmentLookup.end()) {
                throw std::out_of_range("Unknown memory segment: " + words[1]);
            }

            return MemoryBytecode{v->second, segment->second, static_cast<unsigned int>(stoul(words[2]))};
        }

        throw std::out_of_range("Could not parse line: " + line);
    }

    std::vector<Bytecode> parseFile(std::string input_filepath) {
        std::vector<Bytecode> parsed;

        std::ifstream input_file {input_filepath};
        if (!input_file.is_open()) {
            throw std::invalid_argument("Could not find file" + input_filepath);
        }

        std::string line;
        while (std::getline(input_file, line)) {
            auto parsed_line = parseLine(line);
            if (parsed_line.has_value()) {
                parsed.push_back(parsed_line.value());
            }
        }    

        if (const char* env_debug = std::getenv("DEBUG"); env_debug && strncmp(env_debug, "true", 4) == 0) {
            std::cout << "VM parser output" << std::endl << "==========" << std::endl;
            for (auto v : parsed) {
                print(v);
            }   
            std::cout << std::endl;
        }

        return parsed;        
    }
}
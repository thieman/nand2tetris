#include <fstream>
#include <iostream>
#include <variant>

#include "parse.hpp"

namespace vm {
    std::vector<std::string> translateToStrings(vmParse::LogicBytecode *bytecode) {
        std::vector<std::string> result;        
        switch(bytecode->command) {
            case vmParse::LogicCommand::ADD:
                return std::vector<std::string> {
                    "@SP", "M=M-1", "A=M", "D=M",
                    "A=A-1", "M=M+D"
                };
            case vmParse::LogicCommand::SUB:
                break;
            case vmParse::LogicCommand::NEG:
                break;
            case vmParse::LogicCommand::EQ:
                break;
            case vmParse::LogicCommand::GT:
                break;
            case vmParse::LogicCommand::LT:
                break;
            case vmParse::LogicCommand::AND:
                break;
            case vmParse::LogicCommand::OR:
                break;         
            case vmParse::LogicCommand::NOT:
                break;                      
            default:
                throw std::out_of_range("Unreachable condition");
        }

        return result;
    }

    std::vector<std::string> translateToStrings(vmParse::MemoryBytecode *bytecode) {
        std::vector<std::string> result;                

        switch(bytecode->command) {
            case vmParse::MemoryCommand::POP:
                break;
            case vmParse::MemoryCommand::PUSH:
                switch(bytecode->segment) {
                    case vmParse::MemorySegment::CONSTANT:
                        return std::vector<std::string> {
                            "@" + std::to_string(bytecode->value), "D=A", "@SP",
                            "A=M", "M=D", "@SP", "M=M+1"
                        };
                    default:
                        throw std::out_of_range("Unreachable condition");                                    
                }

                break;
            default:
                throw std::out_of_range("Unreachable condition");            
        }

        return result;
    }

    std::vector<std::string> translateToStrings(std::vector<vmParse::Bytecode> bytecodes) {
        std::vector<std::string> result;
        for (auto bytecode : bytecodes) {
            if (auto b = std::get_if<vmParse::LogicBytecode>(&bytecode)) {
                auto bStrings = translateToStrings(b);
                result.insert(result.end(), bStrings.begin(), bStrings.end());
            } else if (auto b = std::get_if<vmParse::MemoryBytecode>(&bytecode)) {
                auto bStrings = translateToStrings(b);
                result.insert(result.end(), bStrings.begin(), bStrings.end());
            }
        }
        return result;
    }

    void vm(std::string input, std::string output) {
        auto parsed_bytecode = vmParse::parseFile(std::move(input));

        auto translated = translateToStrings(parsed_bytecode);

        std::ofstream output_file(output, std::ofstream::out | std::ofstream::trunc);
        if (!output_file.is_open()) {
            throw std::invalid_argument("Could not find file" + output);
        }    

        for (auto line : translated) {
            output_file << line << std::endl;
        }        
    }   
}
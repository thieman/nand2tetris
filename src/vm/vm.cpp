#include <fstream>
#include <iostream>
#include <variant>

#include "parse.hpp"

namespace vm {
    std::vector<std::string> translateToStrings(vmParse::LogicBytecode *bytecode, unsigned int &currentLabel) {
        std::vector<std::string> result;        
        switch(bytecode->command) {
            case vmParse::LogicCommand::ADD:
                return std::vector<std::string> {
                    "@SP", "M=M-1", "A=M", "D=M",
                    "A=A-1", "M=M+D"
                };
            case vmParse::LogicCommand::SUB:
                return std::vector<std::string> {
                    "@SP", "M=M-1", "A=M", "D=M",
                    "A=A-1", "M=M-D"
                };
            case vmParse::LogicCommand::NEG:
                return std::vector<std::string> {
                    "@SP", "A=M-1", "M=-M",
                };
            case vmParse::LogicCommand::EQ:
                currentLabel++;
                return std::vector<std::string> {
                    "@SP", "M=M-1", "A=M", "D=M",
                    "A=A-1", "D=M-D", "M=-1", "@eqlabel_" + std::to_string(currentLabel),
                    "D;JEQ", "@SP", "A=M-1", "M=0", "(eqlabel_" + std::to_string(currentLabel) + ")"
                };
            case vmParse::LogicCommand::GT:
                currentLabel++;
                return std::vector<std::string> {
                    "@SP", "M=M-1", "A=M", "D=M",
                    "A=A-1", "D=M-D", "M=-1", "@gtlabel_" + std::to_string(currentLabel),
                    "D;JGT", "@SP", "A=M-1", "M=0", "(gtlabel_" + std::to_string(currentLabel) + ")"
                };            
            case vmParse::LogicCommand::LT:
                currentLabel++;
                return std::vector<std::string> {
                    "@SP", "M=M-1", "A=M", "D=M",
                    "A=A-1", "D=M-D", "M=-1", "@ltlabel_" + std::to_string(currentLabel),
                    "D;JLT", "@SP", "A=M-1", "M=0", "(ltlabel_" + std::to_string(currentLabel) + ")"
                };                        
            case vmParse::LogicCommand::AND:
                currentLabel++;
                return std::vector<std::string> {
                    "@SP", "M=M-1", "A=M", "D=M",
                    "A=A-1", "M=D&M"
                };            
            case vmParse::LogicCommand::OR:
                currentLabel++;
                return std::vector<std::string> {
                    "@SP", "M=M-1", "A=M", "D=M",
                    "A=A-1", "M=D|M"
                };                                     
            case vmParse::LogicCommand::NOT:
                return std::vector<std::string> {
                    "@SP", "A=M-1", "M=!M"  
                };                
            default:
                throw std::out_of_range("Unreachable condition");
        }

        return result;
    }

    std::vector<std::string> simplePop(std::string reg, bool exactAddress, unsigned int value) {
        return {"@" + reg, exactAddress ? "D=A" : "D=M", "@" + std::to_string(value),
                "D=D+A", "@R15", "M=D", "@SP", "M=M-1", "A=M", "D=M",
                "@R15", "A=M", "M=D"};
    }

    std::vector<std::string> simplePush(std::string reg, bool exactAddress, unsigned int value) {
        return {"@" + reg, exactAddress ? "D=A" : "D=M", "@" + std::to_string(value),
                "A=D+A", "D=M", "@SP", "A=M", "M=D", "@SP", "M=M+1"};
    }    

    std::vector<std::string> translateToStrings(vmParse::MemoryBytecode *bytecode) {
        std::vector<std::string> result;        

        switch(bytecode->command) {
            case vmParse::MemoryCommand::POP:
                switch(bytecode->segment) {
                    case vmParse::MemorySegment::LOCAL:
                        return simplePop("LCL", false, bytecode->value);
                    case vmParse::MemorySegment::ARGUMENT:
                        return simplePop("ARG", false, bytecode->value);
                    case vmParse::MemorySegment::THIS:                     
                        return simplePop("THIS", false, bytecode->value);
                    case vmParse::MemorySegment::THAT:                        
                        return simplePop("THAT", false, bytecode->value);
                    case vmParse::MemorySegment::CONSTANT:
                        throw std::out_of_range("Cannot pop constant");
                    case vmParse::MemorySegment::STATIC:                        
                        break;
                    case vmParse::MemorySegment::TEMP:           
                        return simplePop("5", true, bytecode->value);                                 
                    case vmParse::MemorySegment::POINTER:                        
                        break;
                    default:
                        throw std::out_of_range("Unreachable condition");                                    
                }
            case vmParse::MemoryCommand::PUSH:
                switch(bytecode->segment) {
                    case vmParse::MemorySegment::LOCAL:
                        return simplePush("LCL", false, bytecode->value);
                    case vmParse::MemorySegment::ARGUMENT:
                        return simplePush("ARG", false, bytecode->value);
                    case vmParse::MemorySegment::THIS:                     
                        return simplePush("THIS", false, bytecode->value);
                    case vmParse::MemorySegment::THAT:                        
                        return simplePush("THAT", false, bytecode->value);
                    case vmParse::MemorySegment::CONSTANT:
                        return std::vector<std::string> {
                            "@" + std::to_string(bytecode->value), "D=A", "@SP",
                            "A=M", "M=D", "@SP", "M=M+1"
                        };                        
                    case vmParse::MemorySegment::STATIC:                        
                        break;
                    case vmParse::MemorySegment::TEMP:                        
                        return simplePush("5", true, bytecode->value);                    
                    case vmParse::MemorySegment::POINTER:                        
                        break;
                    default:
                        throw std::out_of_range("Unreachable condition");                                    
                }
            default:
                throw std::out_of_range("Unreachable condition");            
        }

        return result;
    }

    std::vector<std::string> translateToStrings(std::vector<vmParse::Bytecode> bytecodes) {
        unsigned int currentLabel = 0;        
        std::vector<std::string> result;

        for (auto bytecode : bytecodes) {
            if (auto b = std::get_if<vmParse::LogicBytecode>(&bytecode)) {
                auto bStrings = translateToStrings(b, currentLabel);
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
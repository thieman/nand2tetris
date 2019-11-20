#include <variant>
#include <vector>

namespace vmParse {
    enum LogicCommand { ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT };    

    enum MemoryCommand { PUSH, POP };

    enum MemorySegment { LOCAL, ARGUMENT, THIS, THAT, CONSTANT, STATIC, TEMP, POINTER };

    struct LogicBytecode {
        LogicCommand command;
    };

    struct MemoryBytecode {
        MemoryCommand command;
        MemorySegment segment;
        unsigned int value;
    };    

    using Bytecode = std::variant<LogicBytecode, MemoryBytecode>;        

    std::vector<Bytecode> parseFile(std::string input_filepath);
}
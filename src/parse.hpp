#include <iostream>
#include <optional>
#include <variant>
#include <vector>

namespace parse {
    enum Jump { JGT, JEQ, JGE, JLT, JNE, JLE, JMP };

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

    std::vector<Instruction> parseFile(std::string input_filepath);
}
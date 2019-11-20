#include <iostream>

#include <CLI11.hpp>

#include "assemble/assemble.hpp"

int main(int argc, char** argv) {
  CLI::App app{"nand2tetris"};
  app.require_subcommand();

  // Hack to show help if no args provided. Not seeing
  // how else to do this in the library.
  std::vector<const char*> new_argv(argv, argv + argc);
  if (argc == 1) {
    argc += 1;
    new_argv.push_back("--help");
  }

  std::string input_filepath, output_filepath;

  CLI::App* assemble_command = app.add_subcommand("assemble", "Assemble .asm code to .hack binaries");
  assemble_command->add_option("input", input_filepath, ".asm file to assemble")->required();
  assemble_command->add_option("output", output_filepath, ".hack file to output")->required();

  assemble_command->callback(([&input_filepath, &output_filepath]{
    assemble::assemble(std::move(input_filepath), std::move(output_filepath));
  }));

  CLI11_PARSE(app, argc, new_argv.data());

  return 0;
}

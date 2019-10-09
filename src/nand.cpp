#include <iostream>

#include <CLI11.hpp>

#include "assemble.hpp"

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

  CLI::App* assemble = app.add_subcommand("assemble", "Assemble .asm code to .hack binaries");
  assemble->callback(assemble::assemble);

  CLI11_PARSE(app, argc, new_argv.data());

  return 0;
}

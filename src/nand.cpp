#include <iostream>
#include <CLI11.hpp>

int main(int argc, char* argv[]) {
  CLI::App app{"nand2tetris"};

  std::string filename = "default";
  app.add_option("-f,--file", filename, "A help string");

  CLI11_PARSE(app, argc, argv);
  return 0;
}

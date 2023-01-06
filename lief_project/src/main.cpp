#include <iostream>
#include <LIEF/LIEF.hpp>
#include <cxxopts.hpp>

int main(int argc, char** argv) {
  cxxopts::Options options("lief_test" "parses almost any binary");
  options
    .add_options()
    ("b,binary", "input binary", cxxopts::value<std::string>())
    ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
    ;
  auto opts = options.parse(argc, argv);
  if (opts.count("b")) {
    auto binary_name = opts["b"].as<std::string>();
    auto binary = LIEF::Parser::parse(binary_name);
    std::cout << *binary << std::endl;
  }
  return EXIT_SUCCESS;
}

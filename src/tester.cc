#include <iostream>
#include "boost/program_options.hpp"


namespace po = boost::program_options;

namespace {
}  // namespace

int main(int argc, char **argv) {
  po::options_description description("Allowed options");
  description.add_options()
    ("help", "produce help message");

  po::variables_map variables_map;
  po::store(po::parse_command_line(argc, argv, description), variables_map);
  po::notify(variables_map);

  if (variables_map.count("help")) {
    std::cout << description << std::endl;
    return 1;
  }
  return 0;
}

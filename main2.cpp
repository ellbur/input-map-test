
#include <iostream>
#include <boost/program_options/options_description.hpp>
#include <boost/program_options/positional_options.hpp>
#include <boost/program_options/parsers.hpp>
#include <boost/program_options/variables_map.hpp>

#include "devinput-interaction.hpp"
#include "basic-movement-loop.hpp"

using std::string;
using std::cerr;
using boost::program_options::options_description;
using boost::program_options::positional_options_description;
using boost::program_options::command_line_parser;
using boost::program_options::variables_map;
using boost::program_options::store;
using boost::program_options::bool_switch;
using boost::program_options::value;

int main(int argc, char **argv) {
  bool trace;
  
  options_description optsDesc("Options");
  optsDesc.add_options()
      ("help", "Display usage")
      ("trace", bool_switch(&trace), "Print JSON info describing behavior")
      ("kbd-file", value<string>(), "Keyboard device under /dev/input")
    ;
  
  positional_options_description posOpts;
  posOpts.add("kbd-file", 1);
  
  auto parsed = command_line_parser(argc, argv)
    .options(optsDesc)
    .positional(posOpts)
    .run();
  
  variables_map optionsMap;
  store(parsed, optionsMap);
  
  if (optionsMap.count("help")) {
    fprintf(stderr, "Usage: map-keyboard [OPTIONS] <kbd-file>\n");
    cerr << optsDesc;
    return 0;
  }
  else if (optionsMap.count("kbd-file") == 0) {
    fprintf(stderr, "Usage: map-keyboard [OPTIONS] <kbd-file>\n");
    cerr << optsDesc;
    return 1;
  }
  
  string keyboardFilePath = optionsMap["kbd-file"].as<string>();

  BasicMovementLoop loop;
  runDevInputLoop(keyboardFilePath, "send_a", trace, [&](DevInputEvent const& ev) {
      return loop.handle(ev);
  });
  
  return 0;
}


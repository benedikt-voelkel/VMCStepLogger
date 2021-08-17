#include <boost/program_options.hpp>

#include "TMCReplay/TMCReplayDummyApplication.h"
#include "TMCReplay/TMCReplayDummyStack.h"
#include "TMCReplay/TMCReplay.h"

namespace bpo = boost::program_options;

int main(int argc, char* argv[])
{
  // prepare reading cmd options and store them
  bpo::options_description desc("Simulation options");
  desc.add_options()
    ("help", "show this help message and exit")
    ("stepfilename", bpo::value<std::string>()->default_value("MCStepLoggerOutput.root"), "MCStepLogger filename")
    ("steptreename", bpo::value<std::string>()->default_value("StepLoggerTree"), "treename inside file where to find step tree")
    ("geofilename", bpo::value<std::string>()->default_value("o2sim_geometry.root"), "ROOT geometry filename")
    ("geokeyname", bpo::value<std::string>()->default_value("FAIRGeom"), "key name inside geo file where to find geometry tree");

  bpo::variables_map vm;
  bpo::store(bpo::parse_command_line(argc, argv, desc), vm);
  bpo::notify(vm);

  if(vm.count("help")) {
    // print help and exit
    std::cout << desc << std::endl;
    return 1;
  }

  // To be derived from cmd args, to it via boost I guess
  const std::string filename(vm["stepfilename"].as<std::string>());
  const std::string treename(vm["steptreename"].as<std::string>());
  const std::string geoFilename(vm["geofilename"].as<std::string>());
  const std::string geoKeyname(vm["geokeyname"].as<std::string>());

  tmcreplay::TMCReplayDummyStack stack;
  tmcreplay::TMCReplayDummyApplication app(geoFilename, geoKeyname);
  tmcreplay::TMCReplay mc(filename, treename);
  mc.SetStack(&stack);
  app.setStack(&stack);
  mc.Init();

  mc.ProcessRun(-1);

  return 0;
}

// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include <TVirtualMC.h>
#include <iostream>

#include "MCStepLogger/MetaInfo.h"
#include "MCStepLogger/StepLoggerUtilities.h"

#include "MCStepLogger/FieldLogger.h"

using namespace o2;

FieldLogger::FieldLogger()
{
  // check if streaming or interactive
  // configuration done via env variable
  if (std::getenv("MCSTEPLOG_TTREE") || std::getenv("ANALYZE")) {
    mTTreeIO = true;
  }
}

FieldLogger& FieldLogger::Instance()
{
  static FieldLogger logger;
  return logger;
}

void FieldLogger::addStep(TVirtualMC* mc, const double* x, const double* b)
{
  if (mTTreeIO) {
    callcontainer.emplace_back(mc, x[0], x[1], x[2], b[0], b[1], b[2]);
    return;
  }
  counter++;
  int copyNo;
  auto id = mc->CurrentVolID(copyNo);
  if (volumetosteps.find(id) == volumetosteps.end()) {
    volumetosteps.insert(std::pair<int, int>(id, 0));
  } else {
    volumetosteps[id]++;
  }
  if (idtovolname.find(id) == idtovolname.end()) {
    idtovolname.insert(std::pair<int, std::string>(id, std::string(mc->CurrentVolName())));
  }
}

void FieldLogger::clear()
{
  counter = 0;
  volumetosteps.clear();
  idtovolname.clear();
  if (mTTreeIO) {
    callcontainer.clear();
  }
}

void FieldLogger::flush()
{
  if (mTTreeIO) {
    mcsteploggerutilities::flushToTTree("Calls", &callcontainer);
  } else {
    std::cerr << "[FIELDLOGGER]: did " << counter << " steps \n";
    // summarize steps per volume
    for (auto& p : volumetosteps) {
      std::cerr << "[FIELDLOGGER]: VolName " << idtovolname[p.first] << " COUNT " << p.second;
      std::cerr << "\n";
    }
    std::cerr << "[FIELDLOGGER]: ----- END OF EVENT ------\n";
  }
  clear();
}

std::vector<MagCallInfo>* FieldLogger::getContainer()
{
  return &callcontainer;
}


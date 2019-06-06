// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

//  @file   MCStepLoggerImpl.cxx
//  @author Sandro Wenzel
//  @since  2017-06-29
//  @brief  A logging service for MCSteps (hooking into Stepping of TVirtualMCApplication's)

#include "MCStepLogger/StepLogger.h"
#include "MCStepLogger/StepInfo.h"
#include "MCStepLogger/StepLoggerUtilities.h"
//#include "SimulationDataFormat/Stack.h"

#include <dlfcn.h>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

class FairMCApplication
{
 public:
  void Stepping();
  void FinishRun();
};

// Put into namespace since that could be a common name for a variable
namespace simplestepcount
{
  int stepCount = 0;
}

void countStep()
{
  simplestepcount::stepCount++;
}

void FairMCApplication::Stepping()
{
  countStep();
  using StepMethodType = void (FairMCApplication::*)();
  o2::mcsteploggerutilities::dispatchOriginalKernel<FairMCApplication, StepMethodType>(this, "libBase", "_ZN17FairMCApplication8SteppingEv");
}

void FairMCApplication::FinishRun()
{
  std::cerr << "TOTAL STEPS " << simplestepcount::stepCount << std::endl;
  using StepMethodType = void (FairMCApplication::*)();
  o2::mcsteploggerutilities::dispatchOriginalKernel<FairMCApplication, StepMethodType>(this, "libBase", "_ZN17FairMCApplication9FinishRunEv");
}

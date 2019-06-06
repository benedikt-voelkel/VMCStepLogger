// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

//  @file   O2StepCountIntercept.cxx
//  @author Benedikt Volkel
//  @since  2019-11-25
//  @brief  hooking whenever o2::data::Stack::addHit or FairMCApplication::Stepping is called

#include "MCStepLogger/StepLogger.h"
#include "MCStepLogger/StepLoggerUtilities.h"

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

namespace o2
{
namespace data
{
class Stack
{
  public:
   void addHit(int iDet);
};
}
}

// Put into namespace since that could be a common name for a variable
namespace simplestepcount
{
  int stepCount = 0;
  int hitCount = 0;
}

void countStep()
{
  simplestepcount::stepCount++;
}

void countHit()
{
  simplestepcount::hitCount++;
}

void FairMCApplication::Stepping()
{
  countStep();
  using StepMethodType = void (FairMCApplication::*)();
  o2::mcsteploggerutilities::dispatchOriginalKernel<FairMCApplication, StepMethodType>(this, "libBase", "_ZN17FairMCApplication8SteppingEv");
}

void o2::data::Stack::addHit(int iDet)
{
  countHit();
  using StepMethodType = void (o2::data::Stack::*)(int);
  o2::mcsteploggerutilities::dispatchOriginalKernel<o2::data::Stack, StepMethodType>(this, "libO2SimulationDataFormat", "_ZN2o24data5Stack6addHitEi", iDet);
}

void FairMCApplication::FinishRun()
{
  std::cerr << "TOTAL STEPS " << simplestepcount::stepCount << std::endl;
  std::cerr << "TOTAL HITS " << simplestepcount::hitCount << std::endl;
  using StepMethodType = void (FairMCApplication::*)();
  o2::mcsteploggerutilities::dispatchOriginalKernel<FairMCApplication, StepMethodType>(this, "libBase", "_ZN17FairMCApplication9FinishRunEv");
}

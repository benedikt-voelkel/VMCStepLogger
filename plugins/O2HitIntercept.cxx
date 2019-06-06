// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

//  @file   O2HitIntercept.cxx
//  @author Benedikt Volkel
//  @since  2019-11-25
//  @brief  hooking whenever o2::data::Stack::addHit is called

#include "MCStepLogger/StepLogger.h"
#include "MCStepLogger/StepLoggerUtilities.h"

#ifdef NDEBUG
#undef NDEBUG
#endif

namespace o2
{
namespace data
{

class Stack
{
 public:
  void addHit(int iDet);
};

} // namespace data
} // namespace o2

void logHitDetector(int iDet)
{
  o2::StepLogger::Instance().addHit(iDet);
}

void o2::data::Stack::addHit(int iDet)
{
  // /auto baseptr = reinterpret_cast<o2::data::Stack*>(this);
  logHitDetector(iDet);
  using StepMethodType = void (o2::data::Stack::*)(int);
  o2::mcsteploggerutilities::dispatchOriginalKernel<o2::data::Stack, StepMethodType>(this, "libO2SimulationDataFormat", "_ZN2o24data5Stack6addHitEi", iDet);
}

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

#include <vector>
#include <chrono>
#include <iostream>
#include <fstream>

#include <TVirtualMCApplication.h>

#include "MCStepLogger/MCStepLoggerIntercept.h"


namespace o2
{


const char* getLogFileName()
{
  if (const char* f = std::getenv("MCSTEPLOG_OUTFILE")) {
    return f;
  } else {
    return "MCStepLoggerTime.dat";
  }
}



class Timer
{
  int nEvents;
  std::vector<long> startTimes;
  std::vector<long> endTimes;

 public:
  Timer()
  {
    std::ofstream logfile(getLogFileName());
    if(logfile.is_open()) {
      logfile << "#EVENT    STARTTIME [ms]    ENDTIME [ms]    DURATION [ms]\n";
      logfile.close();
    }
  };

  void logTime()
  {
    std::cerr << "Logging time" << std::endl;
    auto now = std::chrono::high_resolution_clock::now();
    auto now_ms = std::chrono::time_point_cast<std::chrono::milliseconds>(now);
    auto epoch = now_ms.time_since_epoch();
    if(endTimes.size() < startTimes.size()) {
      endTimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count());
      flush();
    } else {
      startTimes.push_back(std::chrono::duration_cast<std::chrono::milliseconds>(epoch).count());
    }
  }

  void flush()
  {
    std::ofstream logfile(getLogFileName(), std::ios_base::app | std::ios_base::out);
    //logfile.open(getLogFileName());
    if(logfile.is_open()) {
      logfile << startTimes.size() << "    " << startTimes.back() << "    " << endTimes.back() << "    " << endTimes.back() - startTimes.back() << "\n";
      logfile.close();
    }
  }

};

// the global logging instances (in anonymous namespace)
// pointers to dissallow construction at each library load
Timer* timer;
} // end namespace

extern "C" void initTimer()
{
  // initializes the logging instances
  o2::timer = new o2::Timer();
}

extern "C" void logging()
{
  // initializes the logging instances
  o2::timer->logTime();
}

// a generic function that can dispatch to the original method of a TVirtualMCApplication
extern "C" void dispatchOriginal(TVirtualMCApplication* app, char const* libname, char const* origFunctionName)
{
  typedef void (TVirtualMCApplication::*StepMethodType)();
  o2::mcsteploggerintercept::dispatchOriginalKernel<TVirtualMCApplication, StepMethodType>(app, libname, origFunctionName);
}

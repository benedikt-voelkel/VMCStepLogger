// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#ifndef STEPLOGGER_H_
#define STEPLOGGER_H_

#include <vector>
#include <set>
#include <map>
#include <utility>

#include "MCStepLogger/StepInfo.h"

class TVirtualMC;

namespace o2
{

class StepLogger
{
  int stepcounter = 0;

  std::set<int> trackset;
  std::set<int> pdgset;
  std::map<int, int> volumetosteps;
  std::map<int, int> volumetohits;
  std::map<int, std::string> idtovolname;
  std::map<int, int> volumetoNSecondaries;            // number of secondaries created in this volume
  std::map<std::pair<int, int>, int> volumetoProcess; // mapping of volumeid x processID to secondaries produced

  std::vector<StepInfo> container;
  bool mTTreeIO = false;
  bool mVolMapInitialized = false;
  // To add a hit to the volume a step was made, set in addStep and used if mTTreeIO == false
  int mCurrentVolId = -1;

 public:

  StepLogger();

  static StepLogger& Instance();

  void addStep(TVirtualMC* mc);

  void addHit(int detId, int stepId = -1);

  void clear();

  // prints list of processes for volumeID
  void printProcesses(int volid);

  void flush();

  std::vector<StepInfo>* getContainer();

  o2::StepLookups* getLookups();

  void clearAll();

};

} // namespace o2

#endif /* STEPLOGGER_H_ */

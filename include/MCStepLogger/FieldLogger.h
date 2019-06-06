// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#ifndef FIELDLOGGER_H_
#define FIELDLOGGER_H_

#include <vector>
#include <map>
#include <string>

#include "MCStepLogger/StepInfo.h"

class TVirtualMC;

namespace o2
{

// a class collecting field access per volume
class FieldLogger
{
  int counter = 0;
  std::map<int, int> volumetosteps;
  std::map<int, std::string> idtovolname;
  bool mTTreeIO = false;
  std::vector<MagCallInfo> callcontainer;

 public:
  FieldLogger();

  static FieldLogger& Instance();

  void addStep(TVirtualMC* mc, const double* x, const double* b);

  void clear();

  void flush();

  std::vector<MagCallInfo>* getContainer();

};

} // namespace o2

#endif /* FIELDLOGGER_H_ */

// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#ifndef MCANALYSIS_UTILITIES_H_
#define MCANALYSIS_UTILITIES_H_

#include <algorithm>
#include <iterator>
#include <unordered_map>
#include <vector>
#include <string>

#include "TH1.h"

namespace o2
{
namespace mcstepanalysis
{
namespace utilities
{

/// check for and create a directory
bool createDirectory(const std::string& dir);
/// compressing a histogram with alphanumeric bins and sorting accordingly
/// for sorting option see ROOT's TH1::LabelsOption
void compressHistogram(TH1* histo, const char* sortOption = "");
/// scale bin i by scaleVector[i-1]
void scalePerBin(TH1* histo, const std::vector<float>& scaleVector);
/// for histograms with alphanumeric labels scale bin with name "name" by scaleMap["name"]
void scalePerBin(TH1* histo, const std::unordered_map<std::string, float>& scaleMap);
/// plots into canvases (and compare if multiple analysis files are passed)
bool drawCompare(const std::vector<std::string>& filePaths, const std::string& outputDir);
/// align alphanumeric histograms
bool alignIfAlphanumeric(const std::vector<TH1*> histosIn, std::vector<TH1*>& histosOut);
/// Compute an intersection between 2 containers. The second arg is NOT taken by reference.
/// Hence the same conatiner can be passed to c2 and cIntersect such that cIntersect is overwritten with the intersection
template <typename T>
void intersect(T& c1, T c2, T& cIntersection)
{
  std::sort(c1.begin(), c1.end());
  std::sort(c2.begin(), c2.end());

  cIntersection.clear();

  std::set_intersection(c1.begin(), c1.end(), c2.begin(), c2.end(), std::back_inserter(cIntersection));
}

} // namespace utilities
} // namespace mcstepanalysis
} // namespace o2
#endif /* MCANALYSIS_UTILITIES_H_ */

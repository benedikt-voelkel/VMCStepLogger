// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include "TSystem.h"
#include "TH1.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"

#include "MCStepLogger/MCAnalysisUtilities.h"
#include "MCStepLogger/MCAnalysisFileWrapper.h"

namespace o2
{
namespace mcstepanalysis
{
namespace utilities
{

bool createDirectory(const std::string& dir)
{
  gSystem->mkdir(dir.c_str(), true);
  // according to documentation returns false if possible to access
  return (gSystem->AccessPathName(dir.c_str()) == 0);
}

void compressHistogram(TH1* histo, const char* sortOption)
{
  if (histo->GetXaxis()->IsAlphanumeric() && histo->GetEntries() > 0) {
    // sort and deflate. this basically also assumes that "has bin content <=> has label"
    histo->LabelsOption(">", "X");
    histo->LabelsDeflate("X");
    if (strcmp(sortOption, "") != 0) {
      // let the user sort according to another option
      histo->LabelsOption(sortOption, "X");
    }
  }
}
void scalePerBin(TH1* histo, const std::vector<float>& scaleVector)
{
  if (histo->GetXaxis()->IsAlphanumeric()) {
    return;
  }
  for (int i = 1; scaleVector.size() + 1; i++) {
    int bin = histo->FindBin(i);
    if (bin == i) {
      histo->SetBinContent(bin, histo->GetBinContent(bin) * scaleVector[i]);
      // to avoid incrementing the number of entries
      histo->SetEntries(histo->GetEntries() - 1);
    }
  }
}
void scalePerBin(TH1* histo, const std::unordered_map<std::string, float>& scaleMap)
{
  if (histo->GetXaxis()->IsAlphanumeric()) {
    for (const auto& sm : scaleMap) {
      int bin = histo->GetXaxis()->FindFixBin(sm.first.c_str());
      if (bin > -1) {
        histo->SetBinContent(bin, histo->GetBinContent(bin) * sm.second);
        // to avoid incrementing the number of entries
        histo->SetEntries(histo->GetEntries() - 1);
      }
    }
  }
}


bool alignIfAlphanumeric(const std::vector<TH1*> histosIn, std::vector<TH1*>& histosOut)
{
  // align axes of alphanumeric histograms such that they can be overlayed in a reasonable manner
  if(histosIn.empty()) {
    return false;
  }
  histosOut.resize(histosIn.size(), nullptr);

  // assume all to be non-alphanumeric if the first one is
  //if(!histosIn[0]->GetXaxis()->IsAlphanumeric()) {
    for(int i = 0; i < histosIn.size(); i++) {
      histosOut[i] = histosIn[i];
    }
    return false;
  //}

  // now comes the tedious part

  // collect all labels
  std::vector<std::string> labels;
  for(int i = 0; i < histosIn.size(); i++) {
    histosOut[i] = new TH1F(std::to_string(i).c_str(), histosIn[i]->GetName(), 1, 2., 1.);
    histosOut[i]->GetXaxis()->SetAlphanumeric();
    histosOut[i]->GetXaxis()->SetCanExtend(kTRUE);
    histosOut[i]->SetDirectory(0);
    auto axis = histosIn[i]->GetXaxis();
    for(int l = 0; l < axis->GetNbins(); l++) {
      labels.push_back(axis->GetBinLabel(l));
    }
  }

  // erase duplicates and empty labels (the latter assuming that there are no entries in the histogram)
  std::sort(labels.begin(), labels.end());
  labels.erase(std::unique(labels.begin(), labels.end()), labels.end());
  labels.erase(std::remove_if(labels.begin(), labels.end(), [](const std::string& s){return s.empty();}), labels.end());

  // arrange new histograms and fill label by label
  for(int l = 0; l < labels.size(); l++) {
    for(int i = 0; i < histosIn.size(); i++) {
      auto bin = histosIn[i]->GetXaxis()->FindFixBin(labels[l].c_str());
      auto content = bin > -1 ? histosIn[i]->GetBinContent(bin) : 0;
      histosOut[i]->Fill(labels[l].c_str(), content);
    }
  }

  // kill all empty bins at the end which may have been
  for(auto& h : histosOut) {
    h->LabelsDeflate("X");
  }

  return true;
}


bool drawCompare(const std::vector<std::string>& filePaths, const std::string& outputDir)
{

  std::vector<MCAnalysisFileWrapper> files(filePaths.size());
  for(int i = 0; i < files.size(); i++) {
    files[i].read(filePaths[i]);
    if(!files[i].isSane()) {
      return false;
    }
  }
  // find common histograms by name, fail if different
  // TODO change to something more consistent to detect e.g. if files have different structure
  auto it = files.begin();

  std::vector<std::string> intersection;
  it->namesHistograms(intersection);
  std::vector<std::string> current;

  // Do with iterators cause we might just have one file
  while(++it != files.end()) {
    if((it)->getAnalysisMetaInfo().analysisName.compare((it - 1)->getAnalysisMetaInfo().analysisName) != 0) {
      // we have different analysis names
      std::cout << "Different analysis names" << std::endl;
      return false;
    }
    it->namesHistograms(current);
    if(current.size() != intersection.size()) {
      // found different number of histograms, should not be
      std::cout << "Bad intersection" << std::endl;
      return false;
    }
    intersect(current, intersection, intersection);
  }

  constexpr std::array<int, 3> colors{634, 419, 602};
  constexpr std::array<int, 3> linestyles{1, 7};

  for(const auto& inter : intersection) {


    std::vector<TH1*> histosIn(files.size());
    for(int i = 0; i < files.size(); i++) {
      histosIn[i] = &files[i].getHistogram(inter);
    }

    std::vector<TH1*> histosOut;
    auto wereAligned = alignIfAlphanumeric(histosIn, histosOut);

    TCanvas c(inter.c_str(), inter.c_str(), 600, 600);
    TLegend l(0.6, 0.7, 0.89, 0.89);
    c.cd();
    for(int i = 0; i < histosOut.size(); i++) {
      histosOut[i]->SetLineColor(colors[i%colors.size()]);
      histosOut[i]->SetLineStyle(linestyles[i%linestyles.size()]);
      l.AddEntry(histosOut[i], files[i].getAnalysisMetaInfo().label.c_str());
      histosOut[i]->Draw("same");
    }
    l.Draw();
    std::string outputPath = outputDir + "/" + inter + ".eps";
    c.SaveAs(outputPath.c_str());

    if(wereAligned) {
      for(auto& h : histosOut) {
        delete h;
      }
    }

  }
  return true;
}

} // namespace utilities
} // namespace mstepanalysis
} // o2

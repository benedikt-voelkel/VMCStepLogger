// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

#include <algorithm>
#include <iostream>

#include "MCStepLogger/SimpleStepAnalysis.h"
#include "MCStepLogger/MCAnalysisUtilities.h"
#include "TInterpreter.h"
#include "TInterpreterValue.h"
#include "TFile.h"
#include "TTree.h"
#include "TDatabasePDG.h"

ClassImp(o2::mcstepanalysis::SimpleStepAnalysis);

using namespace o2::mcstepanalysis;

SimpleStepAnalysis::SimpleStepAnalysis()
  : MCAnalysis("SimpleStepAnalysis")
{
}

void SimpleStepAnalysis::initialize()
{
  // accumulated number of steps per module/region
  histNStepsPerMod = getHistogram<TH1I>("nStepsPerMod", 1, 2., 1.);
  // accumulated number of steps per volume
  histNStepsPerVol = getHistogram<TH1I>("nStepsPerVol", 1, 2., 1.);
  // accumulate number of hits per module/region
  histNHitsPerMod = getHistogram<TH1I>("nHitsPerMod", 1, 2., 1.);
  // accumulated number of hits per volume
  histNHitsPerVol = getHistogram<TH1I>("nHitsPerVol", 1, 2., 1.);

  histOriginPerMod = getHistogram<TH1I>("OriginsPerMod", 1, 2., 1.);
  histOriginPerVol = getHistogram<TH1I>("OriginsPerVol", 1, 2., 1.);
  histOriginPerVolSorted = getHistogram<TH1I>("OriginsPerVolSorted", 1, 2., 1.);

  // accumulated number of steps per pdg
  histNStepsPerPDG = getHistogram<TH1I>("nStepsPerPDG", 1, 2., 1.);
  histNStepsPerVolSorted = getHistogram<TH1I>("nStepsPerVolSorted", 1, 2., 1.);

  // accumulated number of secondaries produced per volume
  histNSecondariesPerVol = getHistogram<TH1I>("nSecondariesPerVol", 1, 2., 1.);
  // accumulated number of secondaries produces per module
  histNSecondariesPerMod = getHistogram<TH1I>("nSecondariesPerMod", 1, 2., 1.);
  histNSecondariesPerMod->Sumw2(false);
  // accumulated number of secondaries produces per pdg
  histNSecondariesPerPDG = getHistogram<TH1I>("nSecondariesPerPDG", 1, 2., 1.);

  histTrackEnergySpectrum = getHistogram<TH1I>("trackEnergySpectrum", 400, -10, 4.);
  histTrackPDGSpectrum = getHistogram<TH1I>("trackPDGSpectrum", 1, 2., 1.);
  histTrackPDGSpectrumSorted = getHistogram<TH1I>("trackPDGSpectrumSorted", 1, 2., 1.);
  histTrackProdProcess = getHistogram<TH1I>("trackProdProcess", 1, 2., 1.);

  // steps in the r-z plane
  histRZ = getHistogram<TH2D>("RZOccupancy", 200, -3000., 3000., 200, 0., 3000.);
  // steps in x-y plane
  histXY = getHistogram<TH2D>("XYOccupancy", 200, -3000., 3000., 200, -3000., 3000.);

  // Module/volume traversed before entering another one
  histTraversedBeforePerMod = getHistogram<TH1I>("TraversedBeforePerMod", 1, 2., 1.);
  histTraversedBeforePerVol = getHistogram<TH1I>("TraversedBeforePerVol", 1, 2., 1.);

  // Traversed module/volume vs. origin
  histTraversedBeforeVsOriginPerMod = getHistogram<TH2D>("TraversedBeforeVsOriginPerMod", 1, 2., 1., 1, 2., 1.);
  histTraversedBeforeVsOriginPerVol = getHistogram<TH2D>("TraversedBeforeVsOriginPerVol", 1, 2., 1., 1, 2., 1.);

  // Traversed before vs. current module
  histTraversedBeforeVsCurrentPerMod = getHistogram<TH2D>("TraversedBeforeVsCurrentPerMod", 1, 2., 1., 1, 2., 1.);
  histTraversedBeforeVsCurrentPerVol = getHistogram<TH2D>("TraversedBeforeVsCurrentPerVol", 1, 2., 1., 1, 2., 1.);

  // Monitor time elapsed per module
  histTimePerMod = getHistogram<TH1F>("TimePerMod", 1, 2., 1.);

  // Time per PDG
  histTimePerPDG = getHistogram<TH1F>("TimePerPDG", 1, 2., 1.);

  // Check tracks completely conatined in a module
  histProducedAndKilledPerMod = getHistogram<TH1I>("ProducedAndKilledPerMod", 1, 2., 1.);

  // Spatial distributions of steps having produced a hit
  histHitsX = getHistogram<TH1I>("HitsX", 30, -3000, 3000);
  histHitsY = getHistogram<TH1I>("HitsY", 30, -3000, 3000);
  histHitsZ = getHistogram<TH1I>("HitsZ", 30, -3000, 3000);

  // Momenutum distributions of steps having produced a hit, energy as log10f(E)
  histHitsE = getHistogram<TH1I>("HitsE", 400, -10, 4.);
  histHitsPx = getHistogram<TH1I>("HitsPx", 400, -0.2, 0.2);
  histHitsPy = getHistogram<TH1I>("HitsPy", 400, -0.2, 0.2);
  histHitsPz = getHistogram<TH1I>("HitsPz", 400, -0.2, 0.2);


  // Spatial distributions of all steps
  histStepsX = getHistogram<TH1I>("StepsX", 30, -3000, 3000);
  histStepsY = getHistogram<TH1I>("StepsY", 30, -3000, 3000);
  histStepsZ = getHistogram<TH1I>("StepsZ", 30, -3000, 3000);

  // Momenutum distributions of all steps, energy as log10f(E)
  histStepsE = getHistogram<TH1I>("StepsE", 400, -10, 4.);
  histStepsPx = getHistogram<TH1I>("StepsPx", 400, -0.2, 0.2);
  histStepsPy = getHistogram<TH1I>("StepsPy", 400, -0.2, 0.2);
  histStepsPz = getHistogram<TH1I>("StepsPz", 400, -0.2, 0.2);

  // Monitor steps of particles created and killed inside a module
  histNStepsOnlyInsidePerMod = getHistogram<TH1I>("nStepsOnlyInsidePerMod", 1, 2., 1.);

  // PDG vs. module
  histPDGvsModule = getHistogram<TH2D>("PDGvsModule", 1, 2., 1., 1, 2., 1.);

  // Produced and killed differential
  // PDG vs. mod
  histProducedAndKilledPDGvsMod = getHistogram<TH2D>("ProducedAndKilledPDGvsMod", 1, 2., 1., 1, 2., 1.);
  // energy vs. mod
  histProducedAndKilledEnergyvsMod = getHistogram<TH2D>("ProducedAndKilledEnergyvsMod", 1, 2., 1., 40, 0., 1.);

  histProducedAndSurvivedPDGvsMod = getHistogram<TH2D>("ProducedAndSurvivedPDGvsMod", 1, 2., 1., 1, 2., 1.);
  // energy vs. mod
  histProducedAndSurvivedEnergyvsMod = getHistogram<TH2D>("ProducedAndSurvivedEnergyvsMod", 1, 2., 1., 40, 0., 1.);

  // init runtime user cut
  // thanks to discussions with Philippe Canal, Fermilab
  auto cutcondition = getenv("MCSTEPCUT");
  if (cutcondition) {
    std::string expr = "#include \"StepInfo.h\"\n #include <cmath>\n bool user_cut(const o2::StepInfo &step, \
                        const std::string &volname,                                       \
                        const std::string &modname,                                       \
                        int pdg, o2::StepLookups* lookup) {";
    expr+=std::string(cutcondition);
    expr+=std::string("}");
    auto installpath = getenv("MCSTEPLOGGER_ROOT");
    if (installpath) {
      auto includepath = std::string(installpath) + std::string("/include/MCStepLogger");
      std::cout << "Using include path " << includepath << "\n";
      gInterpreter->AddIncludePath(includepath.c_str());
    }
    else {
      std::cerr << "Could not set path to Steplogger headers; Just-in-time compilation might fail" << "\n";
    }
    gInterpreter->Declare(expr.c_str());
    TInterpreterValue *v = gInterpreter->CreateTemporary();
    // std::unique_ptr<TInterpreterValue> v = gInterpreter->MakeInterpreterValue();
    gInterpreter->Evaluate("user_cut", *v);
    mUserCutFunction = (cut_function_type*)v->GetValAddr();
  }

  if(getenv("KEEPSTEPS")) {
    steptree = new TTree("Steps", "Steps");
  }

}

void SimpleStepAnalysis::analyze(const std::vector<StepInfo>* const steps, const std::vector<MagCallInfo>* const magCalls)
{
  static TDatabasePDG* pdgdatabase = TDatabasePDG::Instance() ? TDatabasePDG::Instance() : new TDatabasePDG;
  static StepInfo const* stepptr;
  static TBranch* branch;
  static bool keepsteps = getenv("KEEPSTEPS") != nullptr;
  if (keepsteps && !branch) {
    branch = steptree->Branch("Steps", &stepptr);
    stepfile = new TFile("Steps.root", "RECREATE");
    steptree->SetDirectory(stepfile);
  }

  // to store the volume name
  std::string volName;
  // to store the module name
  std::string modName;
  // previous module ID
  std::string oldModName;
  // previous volume ID
  std::string oldVolName;

  // total number of steps in this event
  int nSteps = 0;
  // to store particle ID
  int pdgId = 0;
  int nCutSteps = 0;

  int oldTrackID = -1; // to notice when a track changes

  // Setting to true prevents the very first track to be directly filled as if it was contained in the module
  bool moduleChanged = true;

  // Count steps of particles created and killed in a modules
  int stepsOnlyIside = 0;

  // Initial energy of secondary and its PDG
  float energySecondary = -1.;
  std::string pdgSecondary;

  // loop over all steps in an event
  for (const auto& step : *steps) {

    // prepare for PDG ids and volume names
    mAnalysisManager->getLookupPDG(step.trackID, pdgId);
    mAnalysisManager->getLookupVolName(step.volId, volName);
    mAnalysisManager->getLookupModName(step.volId, modName);

    // first increment the total number of steps over all events
    nSteps++;

    // apply user defined cut -- if any
    if (mUserCutFunction && !(*mUserCutFunction)(step, volName, modName, pdgId, mAnalysisManager->getLookups())) {
      nCutSteps++;
      // Must be updated otherwise we can never get the previous volume or module in case it's
      // cutted on these and so volName == oldVolName in all cases
      oldVolName = volName;
      oldModName = modName;
      continue;
    }

    int currentTrackID = step.trackID;
    // TODO Following not sufficient in case of the stacking mechanism in a multi VMC run
    // https://github.com/root-project/root/commit/93992b135b37fe8d2592ead5cdbe3b44ef33fea1
    bool trackFirstSeen = (currentTrackID != oldTrackID);

    if (trackFirstSeen) {
      oldTrackID = currentTrackID;
    }
    bool newtrack = (step.newtrack && trackFirstSeen);
    // If really a new track, the current module/volume is the origin
    if(newtrack) {
      oldModName = modName;
      oldVolName = volName;
    }

    if (keepsteps) {
      stepptr = &step;
      steptree->Fill();
    }

    auto pdgparticle = pdgdatabase->GetParticle(pdgId);
    std::string pdgasstring(pdgparticle? pdgparticle->GetName() : std::to_string(pdgId));

    auto originid = mAnalysisManager->getLookups()->trackorigin[step.trackID];
    std::string originVolName;
    // to store the module name
    std::string originModName;
    mAnalysisManager->getLookupVolName(originid, originVolName);
    mAnalysisManager->getLookupModName(originid, originModName);

    if (trackFirstSeen) {
      histTrackEnergySpectrum->Fill(log10f(step.E));
      histTrackPDGSpectrum->Fill(pdgasstring.c_str(),1);
      histTrackProdProcess->Fill(step.getProdProcessAsString(),1);

      histOriginPerMod->Fill(originModName.c_str(), 1);
      histOriginPerVol->Fill(originVolName.c_str(), 1);

    }
    if(volName.compare(oldVolName) != 0 || newtrack) {
      histTraversedBeforePerVol->Fill(oldVolName.c_str(), 1);
      histTraversedBeforeVsOriginPerVol->Fill(originVolName.c_str(), oldVolName.c_str(), 1);
      histTraversedBeforeVsCurrentPerVol->Fill(oldVolName.c_str(), volName.c_str(), 1);
      // This can be nested here since a module change implies a volume change,
      // but not necessarily the other way round
      if(modName.compare(oldModName) != 0 || newtrack) {
        histTraversedBeforePerMod->Fill(oldModName.c_str(), 1);
        histTraversedBeforeVsOriginPerMod->Fill(originModName.c_str(), oldModName.c_str(), 1);
        histTraversedBeforeVsCurrentPerMod->Fill(oldModName.c_str(), modName.c_str(), 1);
      }
    }

    if(!moduleChanged) {
      stepsOnlyIside++;
    }

    // Some gymnastics to find tracks produced and killed in the same module without having left
    if(newtrack) {
      if(energySecondary >= 0.) {
        if(!moduleChanged) {
          histProducedAndKilledPerMod->Fill(oldModName.c_str(), 1);
          histNStepsOnlyInsidePerMod->Fill(oldModName.c_str(), stepsOnlyIside);
          // One fill gives one entry so this we have to subtract
          histNStepsOnlyInsidePerMod->SetEntries(histNStepsOnlyInsidePerMod->GetEntries() + stepsOnlyIside - 1);
          histProducedAndKilledPDGvsMod->Fill(oldModName.c_str(), pdgSecondary.c_str(), 1);
          histProducedAndKilledEnergyvsMod->Fill(oldModName.c_str(), step.E, 1);
        } else {
          histProducedAndSurvivedPDGvsMod->Fill(oldModName.c_str(), pdgSecondary.c_str(), 1);
          histProducedAndSurvivedEnergyvsMod->Fill(oldModName.c_str(), step.E, 1);
        } 
      } 
      stepsOnlyIside = 0;
      moduleChanged = false;
      if(step.parentTrackID >= 0) {
        energySecondary = step.E;
        pdgSecondary = pdgasstring;
      } else {
        energySecondary = -1.;
        pdgSecondary = "";
      }

    } else if(modName.compare(oldModName) != 0) {
      moduleChanged = true;
    }


    // Update these after they have been used
    oldVolName = volName;
    oldModName = modName;

    // record number of steps per module
    histNStepsPerMod->Fill(modName.c_str(), 1);
    // record number of steps per volume
    histNStepsPerVol->Fill(volName.c_str(), 1);
    // record number of steps per volume
    histNStepsPerPDG->Fill(pdgasstring.c_str(), 1);

    histNSecondariesPerVol->Fill(volName.c_str(), step.nsecondaries);
    histNSecondariesPerMod->Fill(modName.c_str(), step.nsecondaries);
    histNSecondariesPerPDG->Fill(pdgasstring.c_str(), step.nsecondaries);

    histTimePerMod->Fill(modName.c_str(), static_cast<float>(step.timediff) / 1000. );
    histTimePerPDG->Fill(pdgasstring.c_str(), static_cast<float>(step.timediff) / 1000. );


    histRZ->Fill(step.z, std::sqrt(step.x * step.x + step.y * step.y));
    histXY->Fill(step.x, step.y);

    histStepsX->Fill(step.x);
    histStepsY->Fill(step.y);
    histStepsZ->Fill(step.z);
    histStepsE->Fill(log10f(step.E));
    histStepsPx->Fill(step.px);
    histStepsPy->Fill(step.py);
    histStepsPz->Fill(step.pz);

    if(step.detectorHitId >= 0) {
      histNHitsPerMod->Fill(modName.c_str(), 1);
      histNHitsPerVol->Fill(volName.c_str(), 1);
      histHitsE->Fill(log10f(step.E));
      histHitsPx->Fill(step.px);
      histHitsPy->Fill(step.py);
      histHitsPz->Fill(step.pz);
      histHitsX->Fill(step.x);
      histHitsY->Fill(step.y);
      histHitsZ->Fill(step.z);
    }

    histPDGvsModule->Fill(modName.c_str(), pdgasstring.c_str(), 1.);

  }
}

void SimpleStepAnalysis::finalize()
{
  *histNStepsPerVolSorted = *histNStepsPerVol;
  histNStepsPerVolSorted->SetName("nStepsPerVolSorted");
  utilities::compressHistogram(histNStepsPerVolSorted);

  *histOriginPerVolSorted = *histOriginPerVol;
  histOriginPerVolSorted->SetName("OriginPerVolSorted");
  utilities::compressHistogram(histOriginPerVolSorted);

  std::cerr << "MOD have " << histNStepsPerMod->GetEntries() << " entries \n";

  *histTrackPDGSpectrumSorted = *histTrackPDGSpectrum;
  histTrackPDGSpectrumSorted->SetName("trackPDGSpectrumSorted");
  utilities::compressHistogram(histTrackPDGSpectrumSorted);

  // sortit
  // histNStepsPerVolSorted->LabelsOption(">", "X");

  histNStepsPerVolSorted->SetBins(30, 0, 30);

  utilities::compressHistogram(histNStepsPerMod);

  utilities::compressHistogram(histNSecondariesPerMod);
  utilities::compressHistogram(histNSecondariesPerVol);

  utilities::compressHistogram(histTraversedBeforePerMod);
  utilities::compressHistogram(histTraversedBeforePerVol);

  utilities::compressHistogram(histNHitsPerMod);
  utilities::compressHistogram(histNHitsPerVol);

  utilities::compressHistogram(histTimePerMod);
  utilities::compressHistogram(histTimePerPDG);
  utilities::compressHistogram(histProducedAndKilledPerMod);


  histTraversedBeforeVsOriginPerMod->LabelsDeflate("X");
  histTraversedBeforeVsOriginPerMod->LabelsDeflate("Y");
  histTraversedBeforeVsOriginPerMod->GetXaxis()->SetTitle("origins");
  histTraversedBeforeVsOriginPerMod->GetYaxis()->SetTitle("traversed before");

  histTraversedBeforeVsOriginPerVol->LabelsDeflate("X");
  histTraversedBeforeVsOriginPerVol->LabelsDeflate("Y");
  histTraversedBeforeVsOriginPerVol->GetXaxis()->SetTitle("origins");
  histTraversedBeforeVsOriginPerVol->GetYaxis()->SetTitle("traversed before");

  histTraversedBeforeVsCurrentPerMod->LabelsDeflate("X");
  histTraversedBeforeVsCurrentPerMod->LabelsDeflate("Y");
  histTraversedBeforeVsCurrentPerMod->GetXaxis()->SetTitle("traversed before");
  histTraversedBeforeVsCurrentPerMod->GetYaxis()->SetTitle("current module");

  histTraversedBeforeVsCurrentPerVol->LabelsDeflate("X");
  histTraversedBeforeVsCurrentPerVol->LabelsDeflate("Y");
  histTraversedBeforeVsCurrentPerVol->GetXaxis()->SetTitle("traversed before");
  histTraversedBeforeVsCurrentPerVol->GetYaxis()->SetTitle("current module");

  histPDGvsModule->LabelsDeflate("X");
  histPDGvsModule->LabelsDeflate("Y");
  histPDGvsModule->GetXaxis()->SetTitle("module");
  histPDGvsModule->GetYaxis()->SetTitle("PDG");




  if(getenv("KEEPSTEPS")) {
    std::cout << "Writing step tree\n";
    steptree->Write();
    stepfile->Close();
  }

}

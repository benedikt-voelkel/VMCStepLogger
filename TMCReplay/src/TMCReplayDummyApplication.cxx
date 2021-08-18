// some license stuff

#include "TGeoManager.h"

#include "TMCReplay/TMCReplayDummyStack.h"
#include "TMCReplay/TMCReplayDummyApplication.h"

ClassImp(tmcreplay::TMCReplayDummyApplication);

using namespace tmcreplay;

TMCReplayDummyApplication::TMCReplayDummyApplication(const std::string& geoFilename, const std::string& geoKeyname)
  : TVirtualMCApplication("TMCReplayDummyApplication", "TMCReplayDummyApplication"),
    fGeoFilename(geoFilename), fGeoKeyname(geoKeyname), fGeoManager(nullptr)
{}

void TMCReplayDummyApplication::ConstructGeometry()
{
  if(!gGeoManager) {
    // construct one if not present
    new TGeoManager();
  }

  fGeoManager = gGeoManager;

  if(!fGeoFilename.empty() && !fGeoKeyname.empty()) {
    // Otherwise assume that the geometry was constructed already by the user
    fGeoManager->Import(fGeoFilename.c_str(), fGeoKeyname.c_str());
  }
}

void TMCReplayDummyApplication::BeginEvent()
{
  fStack->newEvent();
}

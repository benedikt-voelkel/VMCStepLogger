// some license stuff

#include <TParticle.h>

#include "TMCReplay/TMCReplayDummyStack.h"

ClassImp(tmcreplay::TMCReplayDummyStack);

using namespace tmcreplay;

TMCReplayDummyStack::TMCReplayDummyStack()
  : TVirtualMCStack()
{}


TMCReplayDummyStack::~TMCReplayDummyStack()
{
  clear();
}

void TMCReplayDummyStack::PushTrack(Int_t toBeDone, Int_t parent, Int_t pdg,
                                    Double_t px, Double_t py, Double_t pz, Double_t e,
                                    Double_t vx, Double_t vy, Double_t vz, Double_t tof,
                                    Double_t polx, Double_t poly, Double_t polz,
                                    TMCProcess mech, Int_t& ntr, Double_t weight,
                                    Int_t is)
{
  // We are not setting the track number but use the same indexing used before.
  // Hence, the passed value of ntr is assumed to be already the correct ID
  // This is important to ensure the mapping of track IDs to volumes and produced number of secondaries etc. is the same in the original simulation and the replay.
  // Otherwise, for instance a steplogging of the replay would have slight differences in the lookup.
  auto particle = new TParticle(pdg, is, parent, ntr, -1, -1, px, py, pz, e, vx, vy, vz, tof);
  particle->SetPolarisation(polx, poly, polz);
  particle->SetWeight(weight);
  particle->SetUniqueID(mech);
  if(parent < 0) {
    fNPrimaries++;
  }
  insertParticle(particle, ntr);
}

void TMCReplayDummyStack::SetCurrentTrack(Int_t trackNumber)
{
  fCurrentTrackId = trackNumber;
  fCurrentParticle = fParticles[trackNumber];
  fCurrentParentTrackId = fCurrentParticle->GetFirstMother();
}

void TMCReplayDummyStack::clear()
{
  for(auto& p : fParticles) {
    delete p;
  }
  fParticles.clear();
  fCurrentTrackId = -1;
  fCurrentParentTrackId = -1;
  fNPrimaries = 0;
}

void TMCReplayDummyStack::newEvent()
{
  clear();
}

void TMCReplayDummyStack::insertParticle(TParticle* particle, int id)
{
  if(fParticles.size() <= id) {
    fParticles.resize(id + 1, nullptr);
  }
  fParticles[id] = particle;
}

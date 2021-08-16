// Some license stuff


#ifndef TMC_REPLAY_DUMMY_STACK_H
#define TMC_REPLAY_DUMMY_STACK_H

#include <TVirtualMCStack.h>

class TParticle;


// K is type of user kernel
class TMCReplayDummyStack : public TVirtualMCStack
{

  public:

    TMCReplayDummyStack();
    virtual ~TMCReplayDummyStack();

    virtual void  PushTrack(Int_t toBeDone, Int_t parent, Int_t pdg,
                            Double_t px, Double_t py, Double_t pz, Double_t e,
                            Double_t vx, Double_t vy, Double_t vz, Double_t tof,
                            Double_t polx, Double_t poly, Double_t polz,
                            TMCProcess mech, Int_t& ntr, Double_t weight,
                            Int_t is) override;

    virtual TParticle* PopNextTrack(Int_t& itrack) override
    {
      return nullptr;
    }
    virtual TParticle* PopPrimaryForTracking(Int_t i) override
    {
      // not implemented
      return nullptr;
    }

    virtual void SetCurrentTrack(Int_t trackNumber) override;

    /// Total number of tracks
    virtual Int_t GetNtrack() const override
    {
      // TODO to be implemented properly
      return fParticles.size();
    }

    /// Total number of primary tracks
    virtual Int_t GetNprimary() const override
    {
      // TODO to be implemented properly
      return fNPrimaries;
    }

    /// Current track particle
    virtual TParticle* GetCurrentTrack() const override
    {
      return fCurrentParticle;
    }

    /// Current track number
    virtual Int_t GetCurrentTrackNumber() const override
    {
      return fCurrentTrackId;
    }

    /// Number of the parent of the current track
    virtual Int_t GetCurrentParentTrackNumber() const override
    {
      // TODO to be implemented properly
      return fCurrentParentTrackId;
    }

    void newEvent();

  private:
    void clear();


  private:
    // current track ID
    Int_t fCurrentTrackId;
    // current parent track ID
    Int_t fCurrentParentTrackId;
    // number of primaries
    Int_t fNPrimaries;
    // current TMCParticle pointer
    TParticle* fCurrentParticle;
    // all particles ever pushed
    // TODO we can make that unique_ptrs?!
    std::vector<TParticle*> fParticles;

};

#endif /* TMC_REPLAY_DUMMY_STACK_H */

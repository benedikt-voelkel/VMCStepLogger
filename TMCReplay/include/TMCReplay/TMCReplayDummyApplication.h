// Some license stuff

#ifndef TMC_REPLAY_DUMMY_APPLICATION_H
#define TMC_REPLAY_DUMMY_APPLICATION_H

#include <string>

#include <TVirtualMCApplication.h>

class TGeoManager;

namespace tmcreplay
{
class TMCReplayDummyStack;
}

namespace tmcreplay
{
// K is type of user kernel
class TMCReplayDummyApplication : public TVirtualMCApplication
{

 public:
  TMCReplayDummyApplication(const std::string& geoFilename, const std::string& geoKeyname);

  /// For now just default destructor
  virtual ~TMCReplayDummyApplication() = default;

  // ConstructGeometry is the only implementation of a pure virtual method that does something at the moment

  virtual void ConstructGeometry() override;

  // Implement all pure virtual methods, doing nothinig at the moment

  virtual void InitGeometry() override { ; }

  virtual void GeneratePrimaries() override { ; }

  virtual void BeginEvent() override;

  virtual void BeginPrimary() override { ; }

  virtual void PreTrack() override { ; }

  virtual void Stepping() override
  {
    ;
  }

  virtual void PostTrack() override { ; }

  virtual void FinishPrimary() override { ; }

  virtual void FinishEvent() override { ; }

  void setStack(TMCReplayDummyStack* stack)
  {
    fStack = stack;
  }

 private:
  // Filename where geometry can be found
  std::string fGeoFilename;
  // Keyname under which geometry can be found inside the above file
  std::string fGeoKeyname;
  // local pointer to ROOT's geometry manager
  TGeoManager* fGeoManager;
  // stack
  TMCReplayDummyStack* fStack;
};
} // end namespace tmcreplay

#endif /* TMC_REPLAY_DUMMY_APPLICATION_H */

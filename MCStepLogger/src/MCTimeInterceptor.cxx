// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.

//  @file   MCStepInterceptor.cxx
//  @author Sandro Wenzel
//  @since  2017-06-29
//  @brief  A LD_PRELOAD logger hooking into Stepping of TVirtualMCApplication's

class TVirtualMCApplication;

// (re)declare symbols to be able to hook into them
#define DECLARE_INTERCEPT_SYMBOLS(APP) \
  class APP                            \
  {                                    \
   public:                             \
    void BeginEvent();                 \
    void FinishEvent();                \
    void ConstructGeometry();          \
  };

DECLARE_INTERCEPT_SYMBOLS(FairMCApplication)
DECLARE_INTERCEPT_SYMBOLS(AliMC)
namespace tmcreplay {
  DECLARE_INTERCEPT_SYMBOLS(TMCReplayDummyApplication)
}

extern "C" void initTimer();
extern "C" void logging();
extern "C" void dispatchOriginal(TVirtualMCApplication*, char const* libname, char const*);

#define INTERCEPT_BEGINEVENT(APP, LIB, SYMBOL)                     \
  void APP::BeginEvent()                                           \
  {                                                                \
    auto baseptr = reinterpret_cast<TVirtualMCApplication*>(this); \
    logging();                                                  \
    dispatchOriginal(baseptr, LIB, SYMBOL);                        \
  }

#define INTERCEPT_FINISHEVENT(APP, LIB, SYMBOL)                    \
  void APP::FinishEvent()                                          \
  {                                                                \
    auto baseptr = reinterpret_cast<TVirtualMCApplication*>(this); \
    logging();                                                 \
    dispatchOriginal(baseptr, LIB, SYMBOL);                        \
  }

// we use the ConstructGeometry hook to setup the logger
#define INTERCEPT_GEOMETRYINIT(APP, LIB, SYMBOL)                   \
  void APP::ConstructGeometry()                                    \
  {                                                                \
    auto baseptr = reinterpret_cast<TVirtualMCApplication*>(this); \
    dispatchOriginal(baseptr, LIB, SYMBOL);                        \
    initTimer();                                                   \
  }

// the runtime will now dispatch to these functions due to LD_PRELOAD
INTERCEPT_BEGINEVENT(FairMCApplication, "libBase", "_ZN17FairMCApplication10BeginEventEv")
INTERCEPT_BEGINEVENT(AliMC, "libSTEER", "_ZN5AliMC10BeginEventEv")
INTERCEPT_BEGINEVENT(tmcreplay::TMCReplayDummyApplication, "libTMCReplay", "_ZN9tmcreplay25TMCReplayDummyApplication10BeginEventEv")

INTERCEPT_FINISHEVENT(FairMCApplication, "libBase", "_ZN17FairMCApplication11FinishEventEv")
INTERCEPT_FINISHEVENT(AliMC, "libSTEER", "_ZN5AliMC11FinishEventEv")
INTERCEPT_FINISHEVENT(tmcreplay::TMCReplayDummyApplication, "libTMCReplay", "_ZN9tmcreplay25TMCReplayDummyApplication11FinishEventEv")

INTERCEPT_GEOMETRYINIT(FairMCApplication, "libBase", "_ZN17FairMCApplication17ConstructGeometryEv")
INTERCEPT_GEOMETRYINIT(AliMC, "libSTEER", "_ZN5AliMC17ConstructGeometryEv")
INTERCEPT_GEOMETRYINIT(tmcreplay::TMCReplayDummyApplication, "libTMCReplay", "_ZN9tmcreplay25TMCReplayDummyApplication17ConstructGeometryEv")

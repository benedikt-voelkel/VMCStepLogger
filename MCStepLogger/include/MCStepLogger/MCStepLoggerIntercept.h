// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.


#ifndef STEPLOGGER_UTILITIES_H_
#define STEPLOGGER_UTILITIES_H_

#include <dlfcn.h>
#include <cstdlib>
#include <map>
#include <set>
#include <sstream>
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <cassert>

namespace o2
{
namespace mcsteploggerintercept
{

  // a helper template kernel describing generically the redispatching prodecure
  template <typename Object /* the original object type */, typename MethodType /* member function type */,
            typename... Args /* original arguments to function */>
  void dispatchOriginalKernel(Object* obj, char const* libname, char const* origFunctionName, Args... args)
  {
    // Object, MethodType, and Args are of course related so we could do some static_assert checks or automatic deduction

    // static map to avoid having to lookup the right symbols in the shared lib at each call
    // (We could do this outside of course)
    static std::map<const char*, MethodType> functionNameToSymbolMap;
    MethodType origMethod = nullptr;

    auto iter = functionNameToSymbolMap.find(origFunctionName);
    if (iter == functionNameToSymbolMap.end()) {
      auto libHandle = dlopen(libname, RTLD_NOW);
      // try to make the library loading a bit more portable:
      if (!libHandle) {
        // try appending *.so
        std::stringstream stream;
        stream << libname << ".so";
        libHandle = dlopen(stream.str().c_str(), RTLD_NOW);
      }
      if (!libHandle) {
        // try appending *.dylib
        std::stringstream stream;
        stream << libname << ".dylib";
        libHandle = dlopen(stream.str().c_str(), RTLD_NOW);
      }
      assert(libHandle);
      void* symbolAddress = dlsym(libHandle, origFunctionName);
      assert(symbolAddress);
  // Purposely ignore compiler warning
  #pragma GCC diagnostic push
  #pragma GCC diagnostic ignored "-Wsizeof-pointer-memaccess"
      // hack since C++ does not allow casting to C++ member function pointers
      // thanks to gist.github.com/mooware/1174572
      memcpy(&origMethod, &symbolAddress, sizeof(&symbolAddress));
  #pragma GCC diagnostic pop
      functionNameToSymbolMap[origFunctionName] = origMethod;
    } else {
      origMethod = iter->second;
    }
    // the final C++ member function call redispatch
    (obj->*origMethod)(args...);
  }


} // namespace mcsteploggerutilities
} // namespace o2

#endif /* STEPLOGGER_UTILITIES_H_ */

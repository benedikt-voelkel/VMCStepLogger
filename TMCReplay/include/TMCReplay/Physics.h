// Some license stuff

#ifndef TMC_REPLAY_PHYSICS_H
#define TMC_REPLAY_PHYSICS_H

#include <array>
#include <unordered_map>

// Fix process and cut names
namespace tmcreplay
{
namespace physics
{
constexpr std::array<const char*, 15> namesProcesses = {"PAIR", "COMP", "PHOT", "PFIS", "DRAY", "ANNI", "BREM", "HADR", "MUNU", "DCAY", "LOSS", "MULS", "CKOV", "RAYL", "LABS"};
constexpr std::array<const char*, 11> namesCuts = {"CUTGAM", "CUTELE", "CUTNEU", "CUTHAD", "CUTMUO", "NCUTE", "BCUTM", "DCUTE", "DCUTM", "PPCUTM", "TOFMAX"};
constexpr const char* unknownParam = "UNKNOWN";

template <typename T, std::size_t N>
int paramToIndex(const std::array<T, N>& allParams, const char* paramName)
{
  const auto& it = std::find_if(allParams.begin(), allParams.end(), [&paramName](const char* comp) { return std::strcmp(comp, paramName) == 0; });
  if (it == allParams.end()) {
    return -1;
  }
  return it - allParams.begin();
}

template <typename T, std::size_t N>
const char* indexToParam(const std::array<T, N>& allParams, std::size_t index)
{
  if (index >= allParams.size()) {
    return physics::unknownParam;
  }
  return allParams[index];
}

} // namespace physics
} // end namespace tmcreplay

#endif /* TMC_REPLAY_PHYSICS_H */

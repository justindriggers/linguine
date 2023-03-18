#pragma once

#include <vector>

#include "CubicBezierCurve.h"

namespace linguine {

struct Spline {
  std::vector<CubicBezierCurve> curves;
};

}  // namespace linguine

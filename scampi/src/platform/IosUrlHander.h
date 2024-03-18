#pragma once

#include "UrlHandler.h"

namespace linguine::scampi {

class IosUrlHandler : public UrlHandler {
  public:
    void open(const std::string &url) override;
};

}  // namespace linguine::scampi
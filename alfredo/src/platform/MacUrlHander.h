#pragma once

#include "UrlHandler.h"

namespace linguine::alfredo {

class MacUrlHandler : public UrlHandler {
  public:
    void open(const std::string &url) override;
};

}  // namespace linguine::alfredo
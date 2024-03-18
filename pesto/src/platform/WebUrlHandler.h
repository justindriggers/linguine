#pragma once

#include "UrlHandler.h"

namespace linguine::pesto {

class WebUrlHandler : public UrlHandler {
  public:
    void open(const std::string& url) override;
};

}  // namespace linguine::pesto

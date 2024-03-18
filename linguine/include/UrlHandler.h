#pragma once

#include <string>

namespace linguine {

class UrlHandler {
  public:
    virtual ~UrlHandler() = default;

    virtual void open(const std::string& url) = 0;
};

}  // namespace linguine

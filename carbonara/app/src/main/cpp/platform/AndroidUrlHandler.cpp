#include "AndroidUrlHandler.h"

namespace linguine::carbonara {

void AndroidUrlHandler::open(const std::string& url) {
  _urls.open(url);
}

}  // namespace linguine::carbonara

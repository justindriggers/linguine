#include "WebUrlHandler.h"

#include <emscripten/em_asm.h>

namespace linguine::pesto {

void WebUrlHandler::open(const std::string& url) {
  EM_ASM({
    window.open(UTF8ToString($0), '_blank').focus();
  }, url.c_str());
}

}  // namespace linguine::pesto

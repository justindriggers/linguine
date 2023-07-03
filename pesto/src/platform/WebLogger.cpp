#include "WebLogger.h"

#include <emscripten/emscripten.h>

void linguine::pesto::WebLogger::log(const std::string& log) const {
  emscripten_log(EM_LOG_INFO, log.c_str());
}

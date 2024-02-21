#include "AndroidLogger.h"

#include <android/log.h>

namespace linguine::carbonara {

void AndroidLogger::log(const std::string& log) const {
  __android_log_print(ANDROID_LOG_INFO, "carbonara", "%s", log.c_str());
}

}  // namespace linguine::carbonara

#include "NSLogger.h"

#include <Foundation/Foundation.h>

void linguine::scampi::NSLogger::log(const std::string& log) const {
  @autoreleasepool {
    NSString *nsLog = [NSString stringWithUTF8String:log.c_str()];
    NSLog(@"%@", nsLog);
  }
}

template<>
void linguine::scampi::NSLogger::log(NSString* log) const {
  NSLog(@"%@", log);
}

namespace NS {
  class String;
}

template<>
void linguine::scampi::NSLogger::log(NS::String* log) const {
  @autoreleasepool {
    NSLog(@"%@", (__bridge NSString *)log);
  }
}

#include "MacLogger.h"

#include <iostream>

#include <Foundation/Foundation.h>

void linguine::alfredo::MacLogger::log(const std::string& log) const {
  @autoreleasepool {
    std::cout << log << std::endl;
  }
}

template<>
void linguine::alfredo::MacLogger::log(NSString* log) const {
  std::cout << [log UTF8String] << std::endl;
}

namespace NS {
  class String;
}

template<>
void linguine::alfredo::MacLogger::log(NS::String* log) const {
  @autoreleasepool {
    std::cout << [((__bridge NSString *)log) UTF8String] << std::endl;
  }
}

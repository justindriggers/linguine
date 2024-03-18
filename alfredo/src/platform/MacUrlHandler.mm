#include "MacUrlHander.h"

#import <Foundation/NSURL.h>
#import <AppKit/NSWorkspace.h>

namespace linguine::alfredo {

void MacUrlHandler::open(const std::string& url) {
  auto urlString = [NSString stringWithCString:url.c_str()
                                      encoding:[NSString defaultCStringEncoding]];
  [[NSWorkspace sharedWorkspace] openURL:[NSURL URLWithString:urlString]];
}

}  // namespace linguine::alfredo

#include "IosUrlHander.h"

#import <Foundation/NSURL.h>
#import <UIKit/UIApplication.h>

namespace linguine::scampi {

void IosUrlHandler::open(const std::string& url) {
  auto urlString = [NSString stringWithCString:url.c_str()
                                      encoding:[NSString defaultCStringEncoding]];
  [[UIApplication sharedApplication] openURL:[NSURL URLWithString:urlString]];
}

}  // namespace linguine::scampi

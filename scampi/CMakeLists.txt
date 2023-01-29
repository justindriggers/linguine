project(scampi)

enable_language(OBJCXX)

set(RESOURCES
        resources/LaunchScreen.storyboard
        resources/Main.storyboard
)

add_executable(scampi
        MACOSX_BUNDLE
        src/main.mm
        src/metalkit/ScampiViewDelegate.mm
        src/platform/IosInputManager.cpp
        src/platform/IosLifecycleManager.cpp
        src/platform/IosTimeManager.cpp
        src/platform/NSLogger.mm
        src/uikit/ScampiAppDelegate.mm
        src/uikit/ScampiViewController.mm
        ${RESOURCES}
)

set_target_properties(scampi PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist"
        RESOURCE "${RESOURCES}"
)

target_link_libraries(scampi PRIVATE linguine metal)

find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
target_link_libraries(scampi PRIVATE ${FOUNDATION_LIBRARY})

find_library(UIKIT_LIBRARY UIKit REQUIRED)
target_link_libraries(scampi PRIVATE ${UIKIT_LIBRARY})

find_library(METALKIT_LIBRARY MetalKit REQUIRED)
target_link_libraries(scampi PRIVATE ${METALKIT_LIBRARY})

add_custom_target(scampi-run
        COMMAND xcrun simctl install "753F4927-BCF7-450D-9E12-749CBD46D956" scampi.app
        COMMAND xcrun simctl launch "753F4927-BCF7-450D-9E12-749CBD46D956" com.justindriggers.scampi
        COMMAND open -a Simulator
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/scampi/Release-iphonesimulator
)
add_dependencies(scampi-run scampi)

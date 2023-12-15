project(scampi)

enable_language(OBJCXX)

file(GLOB RESOURCE_FILES
        ${CMAKE_SOURCE_DIR}/assets/**/*)

set(RESOURCES
        resources/LaunchScreen.storyboard
        resources/Main.storyboard
        ${RESOURCE_FILES}
)

add_executable(scampi
        MACOSX_BUNDLE
        src/main.mm
        src/metalkit/ScampiViewDelegate.mm
        src/platform/IosAudioEngineFileLoader.mm
        src/platform/IosInputManager.cpp
        src/platform/IosLifecycleManager.cpp
        src/platform/IosMetalTextureLoader.mm
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

set_xcode_property(scampi CODE_SIGN_IDENTITY "$ENV{CODE_SIGN_IDENTITY}" "All")
set_xcode_property(scampi DEVELOPMENT_TEAM "$ENV{DEVELOPMENT_TEAM}" "All")

target_link_libraries(scampi PRIVATE linguine audioengine metal)

find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
target_link_libraries(scampi PRIVATE ${FOUNDATION_LIBRARY})

find_library(UIKIT_LIBRARY UIKit REQUIRED)
target_link_libraries(scampi PRIVATE ${UIKIT_LIBRARY})

find_library(METALKIT_LIBRARY MetalKit REQUIRED)
target_link_libraries(scampi PRIVATE ${METALKIT_LIBRARY})

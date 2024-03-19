project(scampi)

enable_language(OBJCXX)

file(GLOB RESOURCE_FILES
        ${CMAKE_SOURCE_DIR}/assets/**/*)

set(RESOURCES
        resources/LaunchScreen.storyboard
        resources/Main.storyboard
        resources/scampi.xcassets
        ${RESOURCE_FILES}
)

add_executable(scampi
        MACOSX_BUNDLE
        src/main.mm
        src/metalkit/ScampiViewDelegate.mm
        src/platform/IosAudioEngineFileLoader.mm
        src/platform/IosInputManager.cpp
        src/platform/IosLeaderboardManager.mm
        src/platform/IosLifecycleManager.cpp
        src/platform/IosMetalTextureLoader.mm
        src/platform/IosSaveManager.mm
        src/platform/IosTimeManager.cpp
        src/platform/IosUrlHandler.mm
        src/platform/NSLogger.mm
        src/uikit/ScampiAppDelegate.mm
        src/uikit/ScampiViewController.mm
        ${RESOURCES}
)

set(ENV{PRODUCT_BUNDLE_IDENTIFIER} "com.justindriggers.scampi")
set(APP_VERSION "1.0.0")

set_xcode_property(scampi PRODUCT_BUNDLE_IDENTIFIER "$ENV{PRODUCT_BUNDLE_IDENTIFIER}" "All")
set_xcode_property(scampi CODE_SIGN_IDENTITY "$ENV{CODE_SIGN_IDENTITY}" "All")
set_xcode_property(scampi DEVELOPMENT_TEAM "$ENV{DEVELOPMENT_TEAM}" "All")
set_xcode_property(scampi ASSETCATALOG_COMPILER_APPICON_NAME "AppIcon" "All")
set_xcode_property(scampi CODE_SIGN_ENTITLEMENTS "${CMAKE_CURRENT_SOURCE_DIR}/scampi.entitlements" "All")

set_target_properties(scampi PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/Info.plist.in"
        RESOURCE "${RESOURCES}"
)

if (DEFINED ENV{PROVISIONING_PROFILE_SPECIFIER})
    set_xcode_property(scampi PROVISIONING_PROFILE_SPECIFIER "$ENV{PROVISIONING_PROFILE_SPECIFIER}" "All")
endif()

target_link_libraries(scampi PRIVATE linguine audioengine metal)

find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
target_link_libraries(scampi PRIVATE ${FOUNDATION_LIBRARY})

find_library(UIKIT_LIBRARY UIKit REQUIRED)
target_link_libraries(scampi PRIVATE ${UIKIT_LIBRARY})

find_library(METALKIT_LIBRARY MetalKit REQUIRED)
target_link_libraries(scampi PRIVATE ${METALKIT_LIBRARY})

find_library(GAMEKIT_LIBRARY GameKit REQUIRED)
target_link_libraries(scampi PRIVATE ${GAMEKIT_LIBRARY})

target_include_directories(scampi PRIVATE src)

if (DEFINED ENV{PROVISIONING_PROFILE_SPECIFIER})
    set_xcode_property(scampi INSTALL_PATH "$(LOCAL_APPS_DIR)" "All")
    set_xcode_property(scampi SKIP_INSTALL "No" "All")

    add_custom_target(scampi-clean
            COMMAND ${CMAKE_COMMAND} -E rm -rf Release-iphoneos
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_dependencies(scampi scampi-clean)

    add_custom_target(scampi-archive
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/archive.sh"
                "$ENV{PRODUCT_BUNDLE_IDENTIFIER}"
                "$ENV{DEVELOPMENT_TEAM}"
                "$ENV{PROVISIONING_PROFILE_SPECIFIER}"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_dependencies(scampi-archive scampi)

    add_custom_target(scampi-upload
            COMMAND "${CMAKE_CURRENT_SOURCE_DIR}/scripts/upload.sh"
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    )
    add_dependencies(scampi-upload scampi-archive)
endif()

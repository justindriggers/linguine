project(scampi)

enable_language(OBJCXX)

set(MACOSX_BUNDLE_GUI_IDENTIFIER "com.justindriggers.scampi")
set(MACOSX_BUNDLE_BUNDLE_VERSION 1.0.0)
set(MACOSX_BUNDLE_SHORT_VERSION_STRING 1.0.0)

add_executable(scampi
        MACOSX_BUNDLE
        src/main.mm
        src/ScampiAppDelegate.mm
)

target_link_libraries(scampi PRIVATE linguine)

find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
target_link_libraries(scampi PRIVATE ${FOUNDATION_LIBRARY})

find_library(UIKIT_LIBRARY UIKit REQUIRED)
target_link_libraries(scampi PRIVATE ${UIKIT_LIBRARY})

find_library(METAL_LIBRARY Metal REQUIRED)
target_link_libraries(scampi PRIVATE ${METAL_LIBRARY})

find_library(METALKIT_LIBRARY MetalKit REQUIRED)
target_link_libraries(scampi PRIVATE ${METALKIT_LIBRARY})

target_include_directories(scampi PUBLIC
        "${THIRD_PARTY_DIR}/metal-cpp"
        "${THIRD_PARTY_DIR}/metal-cpp-extensions"
)

add_custom_target(scampi-run
        COMMAND xcrun simctl install "753F4927-BCF7-450D-9E12-749CBD46D956" scampi.app
        COMMAND xcrun simctl launch "753F4927-BCF7-450D-9E12-749CBD46D956" com.justindriggers.scampi
        COMMAND open -a Simulator
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        USES_TERMINAL
)
add_dependencies(scampi-run scampi)

project(alfredo)

enable_language(OBJCXX)

add_executable(alfredo
        src/main.mm
        src/appkit/AlfredoApplicationDelegate.mm
        src/metalkit/AlfredoViewDelegate.mm
        src/platform/MacInputManager.mm
        src/platform/MacLifecycleManager.mm
        src/platform/MacLogger.mm
        src/platform/MacTimeManager.cpp
        src/render/MTKRenderer.cpp
)

target_link_libraries(alfredo PRIVATE linguine metal)

find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
target_link_libraries(alfredo PRIVATE ${FOUNDATION_LIBRARY})

find_library(APPKIT_LIBRARY AppKit REQUIRED)
target_link_libraries(alfredo PRIVATE ${APPKIT_LIBRARY})

find_library(METALKIT_LIBRARY MetalKit REQUIRED)
target_link_libraries(alfredo PRIVATE ${METALKIT_LIBRARY})

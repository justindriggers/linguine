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
)

set(CMAKE_OBJC_FLAGS "-fobjc-arc")
set(CMAKE_OBJCXX_FLAGS "-fobjc-arc")

file(GLOB RESOURCE_FILES
        ${CMAKE_SOURCE_DIR}/assets/**/*)

foreach(RESOURCE_FILE IN LISTS RESOURCE_FILES)
    configure_file(${RESOURCE_FILE} ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
endforeach()

target_link_libraries(alfredo PRIVATE linguine audioengine metal)

find_library(FOUNDATION_LIBRARY Foundation REQUIRED)
target_link_libraries(alfredo PRIVATE ${FOUNDATION_LIBRARY})

find_library(APPKIT_LIBRARY AppKit REQUIRED)
target_link_libraries(alfredo PRIVATE ${APPKIT_LIBRARY})

find_library(METALKIT_LIBRARY MetalKit REQUIRED)
target_link_libraries(alfredo PRIVATE ${METALKIT_LIBRARY})

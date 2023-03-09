enable_language(OBJCXX)

add_library(audioengine
        src/AudioEngineAudioManager.mm
        src/AudioEngineFileLoader.mm
)

set(CMAKE_OBJC_FLAGS "-fobjc-arc")
set(CMAKE_OBJCXX_FLAGS "-fobjc-arc")

target_include_directories(audioengine PUBLIC include)
target_include_directories(audioengine PRIVATE src)

target_link_libraries(audioengine PRIVATE linguine)

find_library(AVFOUNDATION_LIBRARY AVFoundation REQUIRED)
target_link_libraries(audioengine PUBLIC ${AVFOUNDATION_LIBRARY})

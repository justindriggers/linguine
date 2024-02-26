add_library(linguine-aaudio
        src/AAudioAudioManager.cpp
)

target_include_directories(linguine-aaudio PUBLIC include)
target_include_directories(linguine-aaudio PRIVATE src)

target_link_libraries(linguine-aaudio PRIVATE linguine)

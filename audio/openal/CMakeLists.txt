add_library(openal
        src/OpenALAudioManager.cpp
)

target_include_directories(openal PUBLIC include)
target_include_directories(openal PRIVATE src)

target_link_libraries(openal PRIVATE linguine)

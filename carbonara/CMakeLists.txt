project(carbonara)

add_library(carbonara SHARED
        app/src/main/cpp/ApplicationAdapter.cpp
        app/src/main/cpp/main.cpp
        app/src/main/cpp/jni/Callbacks.cpp
        app/src/main/cpp/jni/Leaderboards.cpp
        app/src/main/cpp/jni/SharedPreferences.cpp
        app/src/main/cpp/jni/Urls.cpp
        app/src/main/cpp/platform/AndroidAAudioFileLoader.cpp
        app/src/main/cpp/platform/AndroidInputManager.cpp
        app/src/main/cpp/platform/AndroidLeaderboardManager.cpp
        app/src/main/cpp/platform/AndroidLogger.cpp
        app/src/main/cpp/platform/AndroidOpenGLFileLoader.cpp
        app/src/main/cpp/platform/AndroidSaveManager.cpp
        app/src/main/cpp/platform/AndroidTimeManager.cpp
        app/src/main/cpp/platform/AndroidUrlHandler.cpp
)

find_package(game-activity REQUIRED CONFIG)

target_link_libraries(carbonara PRIVATE
        linguine linguine-aaudio opengl
        game-activity::game-activity_static
        aaudio EGL GLESv3 jnigraphics
        android log
)

target_include_directories(carbonara PRIVATE
        app/src/main/cpp
)

# https://developer.android.com/games/agdk/game-activity/migrate-native-activity#issue
set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -u \
    Java_com_google_androidgamesdk_GameActivity_initializeNativeCode")

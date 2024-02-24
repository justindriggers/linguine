package com.justindriggers.carbonara

import dalvik.annotation.optimization.CriticalNative

class Native {
    companion object {
        @CriticalNative
        @JvmStatic
        external fun onLeftSwipe()

        @CriticalNative
        @JvmStatic
        external fun onRightSwipe()
    }
}
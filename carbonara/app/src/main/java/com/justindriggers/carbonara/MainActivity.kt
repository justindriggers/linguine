package com.justindriggers.carbonara

import android.os.Bundle
import android.view.MotionEvent
import android.view.View
import com.google.androidgamesdk.GameActivity
import com.justindriggers.carbonara.leaderboards.Leaderboards
import com.justindriggers.carbonara.leaderboards.LeaderboardsAdapter
import it.sephiroth.android.library.uigestures.UIGestureRecognizer
import it.sephiroth.android.library.uigestures.UIGestureRecognizerDelegate
import it.sephiroth.android.library.uigestures.UISwipeGestureRecognizer

class MainActivity : GameActivity() {
    companion object {
        init {
            System.loadLibrary("carbonara")
        }
    }

    private lateinit var gestureRecognizerDelegate: UIGestureRecognizerDelegate
    private lateinit var leaderboards: Leaderboards

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        gestureRecognizerDelegate = UIGestureRecognizerDelegate()

        val leftSwipeRecognizer = UISwipeGestureRecognizer(this)
        leftSwipeRecognizer.direction = UISwipeGestureRecognizer.LEFT
        leftSwipeRecognizer.actionListener = { _: UIGestureRecognizer ->
            Native.onLeftSwipe()
        }

        gestureRecognizerDelegate.addGestureRecognizer(leftSwipeRecognizer)

        val rightSwipeRecognizer = UISwipeGestureRecognizer(this)
        rightSwipeRecognizer.direction = UISwipeGestureRecognizer.RIGHT
        rightSwipeRecognizer.actionListener = { _: UIGestureRecognizer ->
            Native.onRightSwipe()
        }

        gestureRecognizerDelegate.addGestureRecognizer(rightSwipeRecognizer)

        leaderboards = LeaderboardsAdapter(this)
    }

    override fun onWindowFocusChanged(hasFocus: Boolean) {
        super.onWindowFocusChanged(hasFocus)
        if (hasFocus) {
            hideSystemUi()
        }
    }

    private fun hideSystemUi() {
        val decorView = window.decorView
        decorView.systemUiVisibility = (View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                or View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                or View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                or View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                or View.SYSTEM_UI_FLAG_FULLSCREEN)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        gestureRecognizerDelegate.onTouchEvent(mSurfaceView, event)
        return super.onTouchEvent(event)
    }

    fun getLeaderboards(): Leaderboards {
        return leaderboards
    }
}
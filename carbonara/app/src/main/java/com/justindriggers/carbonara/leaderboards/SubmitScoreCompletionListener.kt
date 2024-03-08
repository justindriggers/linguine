package com.justindriggers.carbonara.leaderboards

import com.google.android.gms.games.leaderboard.ScoreSubmissionData
import com.google.android.gms.tasks.OnCompleteListener
import com.google.android.gms.tasks.Task

internal class SubmitScoreCompletionListener(
    private val userData: Long
) : OnCompleteListener<ScoreSubmissionData> {

    override fun onComplete(task: Task<ScoreSubmissionData>) {
        if (task.isSuccessful) {
            onSuccess(userData)
        } else {
            onFailure(userData)
        }
    }

    private external fun onSuccess(userData: Long)

    private external fun onFailure(userData: Long)
}
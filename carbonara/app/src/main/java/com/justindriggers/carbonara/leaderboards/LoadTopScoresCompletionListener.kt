package com.justindriggers.carbonara.leaderboards

import com.google.android.gms.games.AnnotatedData
import com.google.android.gms.games.LeaderboardsClient
import com.google.android.gms.tasks.OnCompleteListener
import com.google.android.gms.tasks.Task

internal class LoadTopScoresCompletionListener(
    private val userData: Long
) : OnCompleteListener<AnnotatedData<LeaderboardsClient.LeaderboardScores>> {

    override fun onComplete(task: Task<AnnotatedData<LeaderboardsClient.LeaderboardScores>>) {
        if (task.isSuccessful) {
            val results = task.result.get()

            val scores = results?.scores?.map {
                ScoreEntry(it.scoreHolder?.playerId ?: "", it.scoreHolderDisplayName, it.rawScore)
            }?.toTypedArray() ?: emptyArray()

            onSuccess(scores, userData)

            results?.release()
        } else {
            onFailure(userData)
        }
    }

    private external fun onSuccess(scoreEntries: Array<ScoreEntry>, userData: Long)

    private external fun onFailure(userData: Long)

    private class ScoreEntry(val playerId: String, val displayName: String, val score: Long)
}
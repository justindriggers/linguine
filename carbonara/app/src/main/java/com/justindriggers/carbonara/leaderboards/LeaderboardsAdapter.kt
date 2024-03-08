package com.justindriggers.carbonara.leaderboards

import android.app.Activity
import com.google.android.gms.games.AuthenticationResult
import com.google.android.gms.games.GamesSignInClient
import com.google.android.gms.games.LeaderboardsClient
import com.google.android.gms.games.PlayGames
import com.google.android.gms.games.PlayGamesSdk
import com.google.android.gms.games.PlayersClient
import com.google.android.gms.tasks.OnCompleteListener
import com.google.android.gms.tasks.Task

class LeaderboardsAdapter(
    activity: Activity
) : Leaderboards {

    private val gamesSignInClient: GamesSignInClient
    private val playersClient: PlayersClient
    private val leaderboardsClient: LeaderboardsClient

    private var isAuthenticated = false
    private var playerId = ""

    init {
        PlayGamesSdk.initialize(activity)

        gamesSignInClient = PlayGames.getGamesSignInClient(activity)
        playersClient = PlayGames.getPlayersClient(activity)
        leaderboardsClient = PlayGames.getLeaderboardsClient(activity)

        gamesSignInClient.isAuthenticated()
            .addOnCompleteListener(AuthenticationResultCompletionListener(this))
    }

    override fun isAuthenticated(): Boolean {
        return isAuthenticated
    }

    override fun getPlayerId(): String {
        return playerId
    }

    override fun loadTopScores(leaderboardId: String, span: Int, leaderboardCollection: Int,
                               maxResults: Int, userData: Long) {
        leaderboardsClient.loadTopScores(leaderboardId, span, leaderboardCollection, maxResults, true)
            .addOnCompleteListener(LoadTopScoresCompletionListener(userData))
    }

    override fun submitScore(leaderboardId: String, score: Long, userData: Long) {
        leaderboardsClient.submitScoreImmediate(leaderboardId, score)
            .addOnCompleteListener(SubmitScoreCompletionListener(userData))
    }

    private class AuthenticationResultCompletionListener(
        private val leaderboardsAdapter: LeaderboardsAdapter
    ) : OnCompleteListener<AuthenticationResult> {
        override fun onComplete(task: Task<AuthenticationResult>) {
            leaderboardsAdapter.isAuthenticated = task.isComplete && task.isSuccessful

            leaderboardsAdapter.playersClient.currentPlayerId
                .addOnCompleteListener(PlayerIdCompletionListener(leaderboardsAdapter))
        }
    }

    private class PlayerIdCompletionListener(
        private val leaderboardsAdapter: LeaderboardsAdapter
    ) : OnCompleteListener<String> {
        override fun onComplete(task: Task<String>) {
            if (task.isSuccessful) {
                leaderboardsAdapter.playerId = task.result
            }
        }
    }
}
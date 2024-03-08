package com.justindriggers.carbonara.leaderboards

interface Leaderboards {

    fun isAuthenticated(): Boolean

    fun getPlayerId(): String

    fun loadTopScores(leaderboardId: String, span: Int, leaderboardCollection: Int, maxResults: Int,
                      userData: Long)

    fun submitScore(leaderboardId: String, score: Long, userData: Long)
}
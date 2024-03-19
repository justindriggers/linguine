#include "IosLeaderboardManager.h"

#include <Foundation/Foundation.h>

#import <GameKit/GKLeaderboard.h>
#import <GameKit/GKLeaderboardEntry.h>
#import <GameKit/GKLocalPlayer.h>

namespace linguine::scampi {

bool IosLeaderboardManager::isAvailable() {
  return GKLocalPlayer.localPlayer.authenticated;
}

void IosLeaderboardManager::getHighScores(std::function<void(std::vector<Record> results)> onSuccess,
                                          std::function<void(std::string reason)> onError) {
  if (!isAvailable()) {
    onError("Unauthenticated");
    return;
  }

  [GKLeaderboard loadLeaderboardsWithIDs:@[@"release"]
                       completionHandler:^(NSArray<GKLeaderboard *> *leaderboards, NSError *loadLeaderboardsError) {
                         if (loadLeaderboardsError) {
                           onError([loadLeaderboardsError.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding]);
                           return;
                         }

                         if (leaderboards.count != 1) {
                           onError("An unknown error occurred");
                           return;
                         }

                         auto leaderboard = leaderboards.firstObject;

                         [leaderboard loadEntriesForPlayerScope:GKLeaderboardPlayerScopeGlobal
                                                      timeScope:GKLeaderboardTimeScopeAllTime
                                                          range:NSMakeRange(1, 10)
                                              completionHandler:^(GKLeaderboardEntry *localPlayerEntry, NSArray<GKLeaderboardEntry *> *entries, NSInteger totalPlayerCount, NSError *loadEntriesError) {
                                                if (loadEntriesError) {
                                                  onError([loadEntriesError.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding]);
                                                  return;
                                                }

                                                auto results = std::vector<Record>();

                                                for (GKLeaderboardEntry *entry in entries) {
                                                  results.push_back({
                                                      [entry.player.displayName cStringUsingEncoding:NSUTF8StringEncoding],
                                                      static_cast<int32_t>(entry.score),
                                                      [entry.player.gamePlayerID isEqualToString:localPlayerEntry.player.gamePlayerID]
                                                  });
                                                }

                                                onSuccess(results);
                                              }];
                       }];
}

void IosLeaderboardManager::submitScore(int32_t score,
                                        std::function<void()> onSuccess,
                                        std::function<void(std::string reason)> onError) {
  if (!isAvailable()) {
    onError("Unauthenticated");
    return;
  }

  [GKLeaderboard submitScore:score
                     context:0
                      player:GKLocalPlayer.localPlayer
              leaderboardIDs:@[@"release"]
           completionHandler:^(NSError *error) {
             if (error) {
               onError([error.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding]);
               return;
             }

             onSuccess();
           }];
}

}  // namespace linguine::scampi

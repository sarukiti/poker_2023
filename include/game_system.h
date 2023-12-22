#pragma once

typedef struct card card_t;
typedef struct player player_t;

extern bool is_ahigh_straight;
extern int latch;
extern int max_latch;

void shuffle_stock(card_t* stock);
player_t player_init();
void community_card_open();
void hand_evaluation(player_t* player);
void player_rank_evaluation(player_t* player1, player_t* player2, player_t* player3, player_t* player4);
void check(player_t* player);
void raise(player_t *player, int raise_latch);
void call(player_t* player);
void falled(player_t* player);

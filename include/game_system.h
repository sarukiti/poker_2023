#pragma once

typedef struct card card_t;
typedef enum hand hand_t;
typedef struct player player_t;

extern int player_count;
extern bool is_ahigh_straight;
extern bool is_bet;
extern int table_latch;

void shuffle_stock(card_t* stock);
void game_init(player_t** players);
int preflop(player_t** players);
int flop(player_t** players);
bool is_all_latch_equal(player_t** players);
bool is_almost_player_falled(player_t** players);
player_t player_init(char* player_name);
void player_reset(player_t* player);
void community_card_open();
void next_betting_round(player_t** players);
void hand_evaluation(player_t* player);
int player_action_select(player_t* player, int* checked_count);
void player_rank_evaluation(player_t** players);
void next_game(player_t** players);
void check(player_t* player, int* checked_count);
void force_bet(player_t *player, int bet_latch);
int bet(player_t *player, int bet_latch);
void call(player_t* player);
void falled(player_t* player);
void almost_falled(player_t** players);
void calc_player_profit(player_t* player);
void showdown(player_t** player);
void finish_game(player_t** players);

hand_t straight_hand_judge(...);
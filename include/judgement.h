#pragma once

typedef struct card card_t;
typedef enum hand hand_t;
typedef struct player player_t;

hand_t flash_hand_judge(...);
hand_t straight_hand_judge(...);
hand_t pair_hand_judge(...);

hand_t call_flash_hand_judge(player_t *player);
hand_t call_straight_hand_judge(player_t *player);
hand_t call_pair_hand_judge(player_t *player);

void hand_evaluation(player_t *player);
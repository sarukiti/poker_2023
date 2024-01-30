#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game_system.h"
#include "poker_type.h"
#include "stock.h"
#include "utils.h"

int main(void) {
        opened_card_count = 7;
        card_t card1 = {SPADE, 2};
        card_t card2 = {SPADE, 3};
        card_t card3 = {CLOVER, 4};
        card_t card4 = {SPADE, 5};
        
        card_t card5 = {SPADE, 6};
        card_t card6 = {SPADE, 8};
        card_t card7 = {SPADE, 11};

        card_t straight_card_array[5];

        hand_t straight_ref = straight_hand_judge(
            card1, card2, card3, card4, card5, card6, card7, straight_card_array
        );
        hand_t flash_ref = flash_hand_judge(
            card1, card2, card3, card4, card5, card6, card7, straight_card_array
        );
        hand_t straight_flash_ref = straight_flash_hand_judge(straight_ref, straight_card_array);
}
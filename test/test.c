#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "game_system.h"
#include "poker_type.h"
#include "stock.h"
#include "utils.h"

int main(void) {
    while (true) {
        scanf("%d", &opened_card_count);
        if(&opened_card_count == 0) exit(0);
        card_t card1 = {SPADE, 0};
        card_t card2 = {SPADE, 0};
        card_t card3 = {SPADE, 0};
        card_t card4 = {SPADE, 0};
        card_t card5 = {SPADE, 0};
        card_t card6 = {SPADE, 0};
        card_t card7 = {SPADE, 0};
        card_t cards[7] = {card1, card2, card3, card4, card5, card6, card7};
        for(int i = 0; i < opened_card_count; i++){
            scanf("%d", &cards[i].number);
        }
        int judge_result = 0;
        scanf("%d", &judge_result);
        hand_t hand;
        switch (opened_card_count) {
            case 5:
                hand = straight_hand_judge(cards[0], cards[1], cards[2], cards[3], cards[4]);
                break;
            case 6:
                hand = straight_hand_judge(cards[0], cards[1], cards[2], cards[3], cards[4], cards[5]);
                break;
            case 7:
                hand = straight_hand_judge(cards[0], cards[1], cards[2], cards[3], cards[4], cards[5], cards[6]);
                break;
        }
        assert(hand == judge_result);
    }
}
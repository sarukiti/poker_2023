#include <stdarg.h>
#include <stdlib.h>

#include "poker_type.h"
#include "stock.h"
#include "algorithm.h"

#include "judgement.h"

bool is_ahigh_straight = false;

hand_t flash_hand_judge(...) {
    card_t cards[opened_card_count];
    va_list ap;
    va_start(ap);
    for (int i = 0; i < opened_card_count; i++) {
        cards[i] = va_arg(ap, card_t);
    }
    va_end(ap);

    size_t suit_same_count[4] = {0};
    for (int i = 0; i < opened_card_count; i++) {
        suit_same_count[cards[i].suit]++;
    }
    for (int i = 0; i < 4; i++) {
        if (suit_same_count[i] >= 5)
            return FLASH;
    }
    return NONE;
}
hand_t straight_hand_judge(...) {
    card_t cards[opened_card_count];
    va_list ap;
    va_start(ap);
    for (int i = 0; i < opened_card_count; i++) {
        cards[i] = va_arg(ap, card_t);
    }
    va_end(ap);

    qsort(cards, opened_card_count, sizeof(card_t), compare_card);

    is_ahigh_straight = cards[opened_card_count - 1].number == 14; // ロイヤルストレートフラッシュ判定用

    int straight_count = 0;
    for (int i = 0; i < opened_card_count - 2; i++) {
        if (cards[i + 1].number - cards[i].number == 1) { //同じ数字があり得ることを考慮
            straight_count++;
            if(straight_count >= 4) break;
        }else if(cards[i + 1].number - cards[i].number == 0){
            // 何もしない
        }else{
            straight_count = 0;
        }
    }
    if ((cards[opened_card_count - 1].number - cards[opened_card_count - 2].number == 1) ||
        (cards[opened_card_count - 2].number == 5 && cards[opened_card_count - 1].number == 14)) {
        straight_count++;
    }
    return straight_count >= 4 ? STRAIGHT : NONE;
}
hand_t pair_hand_judge(...) {
    card_t cards[opened_card_count];
    va_list ap;
    va_start(ap);
    for (int i = 0; i < opened_card_count; i++) {
        cards[i] = va_arg(ap, card_t);
    }
    va_end(ap);

    int pair_count = 0;
    for (int i = 0; i < opened_card_count; i++) {
        for (int j = i + 1; j < opened_card_count; j++) {
            if (cards[i].number == cards[j].number) {
                pair_count++;
            }
        }
    }
    switch (pair_count) {
    case 1:
        return ONE_PAIR;
    case 2:
        return TWO_PAIR;
    case 3:
        return THREE_CARD;
    case 4:
        return FULLHOUSE;
    case 6:
        return FOUR_CARD;
    default:
        return NONE;
    }
}

hand_t call_flash_hand_judge(player_t *player) {
    switch (opened_card_count) {
    case 7:
        return flash_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                                community_card[0], community_card[1],
                                community_card[2], community_card[3],
                                community_card[4]);
    case 6:
        return flash_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                                community_card[0], community_card[1],
                                community_card[2], community_card[3]);
    case 5:
        return flash_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                                community_card[0], community_card[1],
                                community_card[2]);
    default:
        return NONE;
    }
}

hand_t call_straight_hand_judge(player_t *player) {
    switch (opened_card_count) {
    case 7:
        return straight_hand_judge((*player).hand_card[0],
                                   (*player).hand_card[1], community_card[0],
                                   community_card[1], community_card[2],
                                   community_card[3], community_card[4]);
    case 6:
        return straight_hand_judge(
            (*player).hand_card[0], (*player).hand_card[1], community_card[0],
            community_card[1], community_card[2], community_card[3]);
    case 5:
        return straight_hand_judge((*player).hand_card[0],
                                   (*player).hand_card[1], community_card[0],
                                   community_card[1], community_card[2]);
    default:
        return NONE;
    }
}
hand_t call_pair_hand_judge(player_t *player) {
    switch (opened_card_count) {
    case 7:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0], community_card[1],
                               community_card[2], community_card[3],
                               community_card[4]);
    case 6:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0], community_card[1],
                               community_card[2], community_card[3]);
    case 5:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0], community_card[1],
                               community_card[2]);
    case 4:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0], community_card[1]);
    case 3:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0]);
    case 2:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1]);
    default:
        return NONE;
    }
}

void hand_evaluation(player_t *player) {
    hand_t flash_ref = call_flash_hand_judge(player);
    hand_t straight_ref = call_straight_hand_judge(player);
    hand_t pair_ref = call_pair_hand_judge(player);
    hand_t straight_flash_ref = (straight_ref == STRAIGHT && flash_ref == FLASH)
                                    ? STRAIGHT_FLASH
                                    : NONE;
    // ロイヤルストレートフラッシュ
    if (straight_flash_ref == STRAIGHT_FLASH && is_ahigh_straight) {
        (*player).hand = ROYAL_STRAIGHT_FLASH;
        return;
    }
    // ストレートフラッシュ
    if (straight_flash_ref == STRAIGHT_FLASH) {
        (*player).hand = STRAIGHT_FLASH;
        return;
    }
    // フォーカード
    if (pair_ref == FOUR_CARD) {
        (*player).hand = FOUR_CARD;
        return;
    }
    // フルハウス
    if (pair_ref == FULLHOUSE) {
        (*player).hand = FULLHOUSE;
        return;
    }
    // フラッシュ
    if (flash_ref == FLASH) {
        (*player).hand = FLASH;
        return;
    }
    // ストレート
    if (straight_ref == STRAIGHT) {
        (*player).hand = STRAIGHT;
        return;
    }
    // スリーカード
    if (pair_ref == THREE_CARD) {
        (*player).hand = THREE_CARD;
        return;
    }
    // ツーペア
    if (pair_ref == TWO_PAIR) {
        (*player).hand = TWO_PAIR;
        return;
    }
    // ペア
    if (pair_ref == ONE_PAIR) {
        (*player).hand = ONE_PAIR;
        return;
    }
    // ハイカード
    (*player).hand = HIGH_CARD;
    return;
}
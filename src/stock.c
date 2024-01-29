#include <stdio.h>
#include <stdlib.h>

#include "utils.h"
#include "poker_type.h"

#include "stock.h"

// Aが一番強いので14とするとソートとかのとき楽になる
card_t stock[52] = {
    {SPADE, 14}, {SPADE, 2}, {SPADE, 3}, {SPADE, 4}, {SPADE, 5}, {SPADE, 6}, {SPADE, 7}, {SPADE, 8}, {SPADE, 9}, {SPADE, 10}, {SPADE, 11}, {SPADE, 12}, {SPADE, 13},
    {HEART, 14}, {HEART, 2}, {HEART, 3}, {HEART, 4}, {HEART, 5}, {HEART, 6}, {HEART, 7}, {HEART, 8}, {HEART, 9}, {HEART, 10}, {HEART, 11}, {HEART, 12}, {HEART, 13},
    {DIAMOND, 14}, {DIAMOND, 2}, {DIAMOND, 3}, {DIAMOND, 4}, {DIAMOND, 5}, {DIAMOND, 6}, {DIAMOND, 7}, {DIAMOND, 8}, {DIAMOND, 9}, {DIAMOND, 10}, {DIAMOND, 11}, {DIAMOND, 12}, {DIAMOND, 13},
    {CLOVER, 14}, {CLOVER, 2}, {CLOVER, 3}, {CLOVER, 4}, {CLOVER, 5}, {CLOVER, 6}, {CLOVER, 7}, {CLOVER, 8}, {CLOVER, 9}, {CLOVER, 10}, {CLOVER, 11}, {CLOVER, 12}, {CLOVER, 13}
};

// コミュニティカード
card_t community_card[5] = {};

// デッキから引かれたカードの枚数
unsigned int drawed_card_count = 0;

// 手札と公開されたコミュニティカードの合計
unsigned int opened_card_count = 2;

card_t draw_card() {
    return stock[drawed_card_count++];
}

void shuffle_stock(card_t *stock) {
    int n = 52;
    for (int i = n - 1; i > 0; i--) {
        // ランダムなインデックスを生成
        int j = rand() % (i + 1);

        // ランダムに入れ替える
        card_t temp = stock[i];
        stock[i] = stock[j];
        stock[j] = temp;
    }
    drawed_card_count = 0;
}

void community_card_open() {
    community_card[opened_card_count - 2] = draw_card();
    printf("コミュニティカードの%d枚目は%sの%d\n",
                opened_card_count - 1,
                get_suit_string(community_card[opened_card_count - 2].suit),
                community_card[opened_card_count - 2].number);
    opened_card_count++;
}

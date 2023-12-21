#pragma once
#include "poker_type.h"

unsigned int drawed_card_count = 0;

//Aが一番強いので14とするとソートとかのとき楽になる
card_t stock[52] = {
    {SPADE, 14}, {SPADE, 2}, {SPADE, 3}, {SPADE, 4}, {SPADE, 5}, {SPADE, 6}, {SPADE, 7}, {SPADE, 8}, {SPADE, 9}, {SPADE, 10}, {SPADE, 11}, {SPADE, 12}, {SPADE, 13},
    {HEART, 14}, {HEART, 2}, {HEART, 3}, {HEART, 4}, {HEART, 5}, {HEART, 6}, {HEART, 7}, {HEART, 8}, {HEART, 9}, {HEART, 10}, {HEART, 11}, {HEART, 12}, {HEART, 13},
    {DIAMOND, 14}, {DIAMOND, 2}, {DIAMOND, 3}, {DIAMOND, 4}, {DIAMOND, 5}, {DIAMOND, 6}, {DIAMOND, 7}, {DIAMOND, 8}, {DIAMOND, 9}, {DIAMOND, 10}, {DIAMOND, 11}, {DIAMOND, 12}, {DIAMOND, 13},
    {CLOVER, 14}, {CLOVER, 2}, {CLOVER, 3}, {CLOVER, 4}, {CLOVER, 5}, {CLOVER, 6}, {CLOVER, 7}, {CLOVER, 8}, {CLOVER, 9}, {CLOVER, 10}, {CLOVER, 11}, {CLOVER, 12}, {CLOVER, 13}
};

unsigned int opened_card_count = 2;
card_t community_card[5] = {};
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "poker_type.h"
#include "algorithm.h"
#include "stock.h"

#include "game_system.h"

bool is_ahigh_straight = false;
int latch = 0;
int max_latch = 0;

void shuffle_stock(card_t* stock){
    int n = 52;
    for (int i = n-1; i > 0; i--) {
        //ランダムなインデックスを生成
        int j = rand() % (i + 1);
 
        //ランダムに入れ替える
        card_t temp = stock[i];
        stock[i] = stock[j];
        stock[j] = temp;
    }
}

card_t draw_card(){
    return stock[drawed_card_count++];
}

player_t player_init(){
    player_t player = {
        6000,
        {draw_card(), draw_card()},
        HIGH_CARD,
        0,
        INIT
    };
    opened_card_count = 2;
    return player;
}

void community_card_open(){
    community_card[opened_card_count-2] = draw_card();
    opened_card_count++;
}

hand_t flash_hand_judge(...){
    card_t cards[opened_card_count];
    va_list ap;
    va_start(ap);
    for(int i = 0; i < opened_card_count; i++){
        cards[i] = va_arg(ap, card_t);
    }
    va_end(ap);

    size_t suit_same_count[4] = {0};
    for(int i = 0; i < opened_card_count; i++){
        suit_same_count[cards[i].suit]++;
    }
    for(int i = 0; i < 4; i++){
        if(suit_same_count[i] >= 5) return FLASH;
    }
    return NONE;
}

hand_t straight_hand_judge(...){
    card_t cards[opened_card_count];
    va_list ap;
    va_start(ap);
    for(int i = 0; i < opened_card_count; i++){
        cards[i] = va_arg(ap, card_t);
    }
    va_end(ap);

    qsort(cards, opened_card_count, sizeof(card_t), compare_card);
    
    is_ahigh_straight = cards[opened_card_count - 1].number == 14; //ロイヤルストレートフラッシュ判定用

    int straight_count = 0;
    for(int i = 0; i < opened_card_count - 2; i++){
        if(cards[i + 1].number - cards[i].number == 1){
            straight_count++;
        }
    }
    if(cards[opened_card_count - 1].number - cards[opened_card_count - 2].number == 1 || (cards[opened_card_count - 2].number == 5 && cards[opened_card_count - 1].number == 14)){
        straight_count++;
    }
    return straight_count >= 4 ? STRAIGHT : NONE;
}

hand_t pair_hand_judge(...){
    card_t cards[opened_card_count];
    va_list ap;
    va_start(ap);
    for(int i = 0; i < opened_card_count; i++){
        cards[i] = va_arg(ap, card_t);
    }
    va_end(ap);
    
    int pair_count = 0;
    for(int i = 0; i < opened_card_count; i++) {
        for(int j = i + 1; j < opened_card_count; j++) {
            if (cards[i].number == cards[j].number){
                pair_count++;
            }
        }
    }
    switch(pair_count){
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

hand_t call_flash_hand_judge(player_t* player){
    switch(opened_card_count){
        case 7:
            return flash_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2], community_card[3], community_card[4]);
        case 6:
            return flash_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2], community_card[3]);
        case 5:
            return flash_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2]);
        default:
            return NONE;
    }
}
hand_t call_straight_hand_judge(player_t* player){
    switch(opened_card_count){
        case 7:
            return straight_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2], community_card[3], community_card[4]);
        case 6:
            return straight_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2], community_card[3]);
        case 5:
            return straight_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2]);
        default:
            return NONE;
    }
}

hand_t call_pair_hand_judge(player_t* player){
    switch(opened_card_count){
        case 7:
            return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2], community_card[3], community_card[4]);
        case 6:
            return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2], community_card[3]);
        case 5:
            return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2]);
        case 4:
            return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1]);
        case 3:
            return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0]);
        case 2:
            return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1]);
        default:
            return NONE;
    }
}

void hand_evaluation(player_t* player){
    hand_t flash_ref = call_flash_hand_judge(player);
    hand_t straight_ref = call_straight_hand_judge(player);
    hand_t pair_ref = call_pair_hand_judge(player);
    hand_t straight_flash_ref = (straight_ref == STRAIGHT && flash_ref == FLASH) ? STRAIGHT_FLASH : NONE;
    //ロイヤルストレートフラッシュ
    if(straight_flash_ref == STRAIGHT_FLASH && is_ahigh_straight){
        (*player).hand = ROYAL_STRAIGHT_FLASH;
        return;
    }
    //ストレートフラッシュ
    if(straight_flash_ref == STRAIGHT_FLASH){
        (*player).hand = STRAIGHT_FLASH;
        return;
    }
    //フォーカード
    if(pair_ref == FOUR_CARD){
        (*player).hand = FOUR_CARD;
        return;
    }
    //フルハウス
    if(pair_ref == FULLHOUSE){
        (*player).hand = FULLHOUSE;
        return;
    }
    //フラッシュ
    if(flash_ref == FLASH){
        (*player).hand = FLASH;
        return;
    }
    //ストレート
    if(straight_ref == STRAIGHT){
        (*player).hand = STRAIGHT;
        return;
    }
    //スリーカード
    if(pair_ref == THREE_CARD){
        (*player).hand = THREE_CARD;
        return;
    }
    //ツーペア
    if(pair_ref == TWO_PAIR){
        (*player).hand = TWO_PAIR;
        return;
    }
    //ペア
    if(pair_ref == ONE_PAIR){
        (*player).hand = ONE_PAIR;
        return;
    }
    //ハイカード
    (*player).hand = HIGH_CARD;
    return;
}

void player_rank_evaluation(player_t* player1, player_t* player2, player_t* player3, player_t* player4){
    player_t* players[4] = {player1, player2, player3, player4};
    qsort(players, 4, sizeof(player_t*), compare_player_rank);
    (*(players[0])).rank = 1;
    (*(players[1])).rank = 2;
    (*(players[2])).rank = 3;
    (*(players[3])).rank = 4;
    return;
}

void check(player_t *player){
    return;
}
void raise(player_t *player, int raise_latch){
    (*player).coin-=raise_latch;
    latch+=raise_latch;
    max_latch = max(raise_latch, max_latch);
    return;
}
void call(player_t *player){
    (*player).coin-=max_latch;
    latch+=max_latch;
    return;
}
void falled(player_t *player){
    (*player).state = FALLED;
    return;
}

//まず全員6000コイン出す
//カードを全員に2枚ずつ配る
//掛け金を決める(コール, チェック, レイズ)
//場にストックから3枚出る
//掛け金を決める
//場にストックから1枚出る
//掛け金を決める
//それぞれの役を決定する
//得点配分
//勝者以外が飛ぶまでやる
//勝った人の名前、役、持ち金をファイルに書き込む

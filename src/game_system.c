#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <time.h>

#include "poker_type.h"
#include "algorithm.h"
#include "stock.h"

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

void community_card_open(){
    community_card[opened_card_count-2] = draw_card();
    opened_card_count++;
}

void hand_evaluation(player_t* player){
    hand_t ref = NONE;
    switch(opened_card_count){
        case 7:
            ref = pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2], community_card[3], community_card[4]);
            break;
        case 6:
            ref = pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2], community_card[3]);
            break;
        case 5:
            ref = pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1], community_card[2]);
            break;
        case 4:
            ref = pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0], community_card[1]);
            break;
        case 3:
            ref = pair_hand_judge((*player).hand_card[0], (*player).hand_card[1], community_card[0]);
            break;
        case 2:
            ref = pair_hand_judge((*player).hand_card[0], (*player).hand_card[1]);
            break;
    }

    //ロイヤルストレートフラッシュ
    //ストレートフラッシュ
    //フォーカード
    if(ref == FOUR_CARD){
        (*player).hand = FOUR_CARD;
        return;
    }
    //フルハウス
    if(ref == FULLHOUSE){
        (*player).hand = FULLHOUSE;
        return;
    }
    //フラッシュ
    //ストレート
    //スリーカード
    if(ref == THREE_CARD){
        (*player).hand = THREE_CARD;
        return;
    }
    //ツーペア
    if(ref == TWO_PAIR){
        (*player).hand = TWO_PAIR;
        return;
    }
    //ペア
    if(ref == ONE_PAIR){
        (*player).hand = ONE_PAIR;
        return;
    }
    //ハイカード
    (*player).hand = HIGH_CARD;
    return;

}

int hand_card_max(card_t* card){
    return max(card[0].number, card[1].number);
}
int hand_suit_max(card_t* card){
    return max(card[0].suit, card[1].suit);
}

void swap(player_t *x, player_t *y){
    player_t temp;
    temp = *x;
    *x = *y;
    *y = temp;
}

void player_rank_evaluation(player_t* player1, player_t* player2, player_t* player3, player_t* player4){
    player_t* players[4] = {player1, player2, player3, player4};
    for(int i = 0; i < 3; i++){
        for(int j = 3; j >= i + 1; j--){
            if((*players)[j].hand > (*players)[j-1].hand) {
                swap(players[j], players[j-1]);
            }else if((*players)[j].hand == (*players)[j-1].hand){
                if(hand_card_max((*players)[j].hand_card) > hand_card_max((*players)[j-1].hand_card)){
                    swap(players[j], players[j-1]);
                }else if(hand_card_max((*players)[j].hand_card) == hand_card_max((*players)[j-1].hand_card)){
                    if(hand_suit_max((*players)[j].hand_card) > hand_suit_max((*players)[j-1].hand_card)){
                        swap(players[j], players[j-1]);
                    }
                }
            }
        }
    }
    (*players)[0].rank = 1;
    (*players)[1].rank = 2;
    (*players)[2].rank = 3;
    (*players)[3].rank = 4;
    return;
}

//[test]
int main(void){
    srand((unsigned int)time(NULL));
    shuffle_stock(stock);
    player_t player1 = player_init();
    community_card_open();
    community_card_open();
    community_card_open();
    community_card_open();
    community_card_open();
    hand_evaluation(&player1);
    return 0;
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

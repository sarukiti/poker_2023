#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
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

void hand_evaluation(player_t player){
    if(opened_card_count == 5) {
        //ロイヤルストレートフラッシュ
        //ストレートフラッシュ
        //フルハウス
        //フラッシュ
        //ストレート
    }
    if(opened_card_count >= 4){
        //フォーカード
        //ツーペア
    }
    if(opened_card_count >= 3){
        //スリーカード
    }
    //ペア
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
    player_t player2 = player_init();
    player_t player3 = player_init();
    player_t player4 = player_init();
    player_rank_evaluation(&player1, &player2, &player3, &player4);
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

#include "algorithm.h"

int max(int a, int b){
    return (a >= b) ? a : b;
}
int min(int a, int b){
    return (a <= b) ? a : b;
}

int hand_card_max(card_t* card){
    return max(card[0].number, card[1].number);
}
int hand_suit_max(card_t* card){
    return max(card[0].suit, card[1].suit);
}

int compare_player_rank(const void *x, const void *y){
    if((**(player_t**)x).hand > (**(player_t**)y).hand) return -1;
    else if((**(player_t**)x).hand == (**(player_t**)y).hand){
        if(hand_card_max((**(player_t**)x).hand_card) > hand_card_max((**(player_t**)y).hand_card)) return -1;
        else if(hand_card_max((**(player_t**)x).hand_card) == hand_card_max((**(player_t**)y).hand_card)){
            if(hand_suit_max((**(player_t**)x).hand_card) > hand_suit_max((**(player_t**)y).hand_card)) return -1;
        }
    }
    return 1;
}
int compare_card(const void *x, const void *y){
    return (*(card_t*)x).number - (*(card_t*)y).number;
}
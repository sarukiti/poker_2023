#include <stdio.h>
#include "poker_type.h"
#include "utils.h"

const char* get_suit_string(suit_t suit){
    switch(suit){
        case SPADE:
            return "SPADE";
        case HEART:
            return "HEART";
        case DIAMOND:
            return "DIAMOND";
        case CLOVER:
            return "CLOVER";
    }
}

const char* get_hand_string(hand_t hand){
    switch(hand){
        case HIGH_CARD:
            return "HIGH_CARD";
        case ONE_PAIR:
            return "ONE_PAIR";
        case TWO_PAIR:
            return "TWO_PAIR";
        case THREE_CARD:
            return "THREE_CARD";
        case STRAIGHT:
            return "STRAIGHT";
        case FLASH:
            return "FLASH";
        case FULLHOUSE:
            return "FULLHOUSE";
        case FOUR_CARD:
            return "FOUR_CARD";
        case STRAIGHT_FLASH:
            return "STRAIGHT_FLASH";
        case ROYAL_STRAIGHT_FLASH:
            return "ROYAL_STRAIGHT_FLASH";
        default:
            return "NONE";
    }
}

void print_prompt(const char* prompt){
    printf("%s\n> ", prompt);
}
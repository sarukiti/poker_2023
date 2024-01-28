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
            return "\x1b[38;5;0mHIGH_CARD\x1b[39m";
        case ONE_PAIR:
            return "\x1b[38;5;22mONE_PAIR\x1b[39m";
        case TWO_PAIR:
            return "\x1b[38;5;10mTWO_PAIR\x1b[39m";
        case THREE_CARD:
            return "\x1b[38;5;24mTHREE_CARD\x1b[39m";
        case STRAIGHT:
            return "\x1b[38;5;38mSTRAIGHT\x1b[39m";
        case FLASH:
            return "\x1b[38;5;27mFLASH\x1b[39m";
        case FULLHOUSE:
            return "\x1b[38;5;56mFULLHOUSE\x1b[39m";
        case FOUR_CARD:
            return "\x1b[38;5;128mFOUR_CARD\x1b[39m";
        case STRAIGHT_FLASH:
            return "\x1b[38;5;226mSTRAIGHT_FLASH\x1b[39m";
        case ROYAL_STRAIGHT_FLASH:
            return "\x1b[38;5;196mROYAL_STRAIGHT_FLASH\x1b[39m";
        default:
            return "NONE";
    }
}

void print_prompt(const char* prompt, int* buf){
    char input[1024];
    do{
        printf("%s\n> ", prompt);
        fgets(input, 1024, stdin);
    }while(sscanf(input, "%d", buf) != 1);
}
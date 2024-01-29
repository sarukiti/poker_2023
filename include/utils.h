#pragma once

typedef enum suit suit_t;
typedef enum hand hand_t;

const char* get_suit_string(suit_t suit);
const char* get_hand_string(hand_t hand);
void print_prompt(const char* prompt, int* buf);
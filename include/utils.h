#pragma once

const char* get_suit_string(suit_t suit);
const char* get_card_number_string(int num);
const char* get_hand_string(hand_t hand);
void print_prompt(const char* prompt, int* buf);
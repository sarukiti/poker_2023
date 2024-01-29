#pragma once

typedef struct card card_t;

extern card_t stock[52];
extern card_t community_card[5];
extern unsigned int drawed_card_count;
extern unsigned int opened_card_count;

card_t draw_card();
void shuffle_stock(card_t* stock);
void community_card_open();
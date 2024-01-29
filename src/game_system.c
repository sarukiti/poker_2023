#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "algorithm.h"
#include "poker_type.h"
#include "stock.h"
#include "utils.h"

#include "game_system.h"

constexpr int PLAYER_COUNT = 6;
constexpr int FORCE_BET_LATCH = 100;
constexpr int PLAYER_INITIAL_COIN = 6000;

bool is_ahigh_straight = false;
bool is_bet = false;
int before_latch = 0;
int table_latch = 0;
int falled_count = 0;
int retirement_count = 0;

void shuffle_stock(card_t *stock) {
    int n = 52;
    for (int i = n - 1; i > 0; i--) {
        // ランダムなインデックスを生成
        int j = rand() % (i + 1);

        // ランダムに入れ替える
        card_t temp = stock[i];
        stock[i] = stock[j];
        stock[j] = temp;
    }
    drawed_card_count = 0;
}

void game_init(player_t *players[PLAYER_COUNT]) {
    int random_dealer_button_select = rand() % PLAYER_COUNT;

    force_bet(players[(random_dealer_button_select + 1) % PLAYER_COUNT],
              FORCE_BET_LATCH);
    force_bet(players[(random_dealer_button_select + 2) % PLAYER_COUNT],
              FORCE_BET_LATCH * 2);

    // ビッグブラインドの左隣を先頭に
    player_t *player_temp[PLAYER_COUNT];
    for (int i = 0; i < PLAYER_COUNT; i++) {
        player_temp[i] =
            players[(random_dealer_button_select + 3 + i) % PLAYER_COUNT];
    }
    for (int i = 0; i < PLAYER_COUNT; i++) {
        players[i] = player_temp[i];
    }
}

int preflop(player_t *players[PLAYER_COUNT]) {
    while (true) {
        for(int i = retirement_count; i < PLAYER_COUNT; i++){
            if (player_action_select(players[i], NULL) == ALMOST_FALLED)
                return ALMOST_FALLED;
            if (is_all_latch_equal(players))
                return SUCCESS;
            }
    }
}

int flop(player_t *players[PLAYER_COUNT]) {
    is_bet = false;
    before_latch = 0;

    for(int i = 0; i < PLAYER_COUNT; i++){
        players[i]->latch = 0;
    }

    for (int i = 0; i < 3; i++) {
        community_card_open();
    }

    while (true) {
        int checked_count = 0;
        for(int i = retirement_count; i < PLAYER_COUNT; i++){
            if (player_action_select(players[i], &checked_count) == ALMOST_FALLED)
                return ALMOST_FALLED;
            if (is_all_latch_equal(players)){
                next_betting_round(players);
                break;
            }
        }
        if (checked_count == (PLAYER_COUNT - falled_count - retirement_count)) {
            if (opened_card_count == 7) {
                return SHOWDOWN;
            }
            community_card_open();
            next_betting_round(players);
            continue;
        }
    }
}

void next_betting_round(player_t* players[PLAYER_COUNT]){
    is_bet = false;
    before_latch = 0;
    for(int i = 0; i < PLAYER_COUNT; i++){
        players[i]->latch = 0;
    }
    printf("\n");
}

bool is_all_latch_equal(player_t *players[PLAYER_COUNT]) {
    int available_player_count = 0;
    int latch_equal_count = 0;
    player_t *available_players[6];
    for (int i = 0; i < 6; i++) {
        if (players[i]->state == PLAYING) {
            available_players[available_player_count] = players[i];
            available_player_count++;
        }
    }
    for (int i = 0; i < available_player_count - 1; i++) {
        if (available_players[i]->latch == available_players[i + 1]->latch && available_players[i]->latch != 0)
            latch_equal_count++;
    }
    return available_player_count == (latch_equal_count + 1);
}

card_t draw_card() { return stock[drawed_card_count++]; }

player_t player_init(unsigned int player_number) {
    player_t player = {
        player_number,
        PLAYER_INITIAL_COIN,
        0,
        {draw_card(), draw_card()},
        HIGH_CARD,
        0,
        PLAYING,
    };
    opened_card_count = 2;
    return player;
}
void player_reset(player_t *player) {
    player->latch = 0;
    player->hand_card[0] = draw_card();
    player->hand_card[1] = draw_card();
    player->hand = HIGH_CARD;
    if(player->state == FALLED) player->state = PLAYING;
    opened_card_count = 2;
    return;
}

void community_card_open() {
    community_card[opened_card_count - 2] = draw_card();
    printf("コミュニティカードの%d枚目は%sの%d\n",
                opened_card_count - 1,
                get_suit_string(community_card[opened_card_count - 2].suit),
                community_card[opened_card_count - 2].number);
    opened_card_count++;
}

hand_t flash_hand_judge(...) {
    card_t cards[opened_card_count];
    va_list ap;
    va_start(ap);
    for (int i = 0; i < opened_card_count; i++) {
        cards[i] = va_arg(ap, card_t);
    }
    va_end(ap);

    size_t suit_same_count[4] = {0};
    for (int i = 0; i < opened_card_count; i++) {
        suit_same_count[cards[i].suit]++;
    }
    for (int i = 0; i < 4; i++) {
        if (suit_same_count[i] >= 5)
            return FLASH;
    }
    return NONE;
}

hand_t straight_hand_judge(...) {
    card_t cards[opened_card_count];
    va_list ap;
    va_start(ap);
    for (int i = 0; i < opened_card_count; i++) {
        cards[i] = va_arg(ap, card_t);
    }
    va_end(ap);

    qsort(cards, opened_card_count, sizeof(card_t), compare_card);

    is_ahigh_straight = cards[opened_card_count - 1].number == 14; // ロイヤルストレートフラッシュ判定用

    int straight_count = 0;
    for (int i = 0; i < opened_card_count - 2; i++) {
        if (cards[i + 1].number - cards[i].number <= 1) { //同じ数字があり得ることを考慮
            straight_count++;
            if(straight_count >= 4) break;
        }else{
            straight_count = 0;
        }
    }
    if ((cards[opened_card_count - 1].number - cards[opened_card_count - 2].number == 1) ||
        (cards[opened_card_count - 2].number == 5 && cards[opened_card_count - 1].number == 14)) {
        straight_count++;
    }
    return straight_count >= 4 ? STRAIGHT : NONE;
}

hand_t pair_hand_judge(...) {
    card_t cards[opened_card_count];
    va_list ap;
    va_start(ap);
    for (int i = 0; i < opened_card_count; i++) {
        cards[i] = va_arg(ap, card_t);
    }
    va_end(ap);

    int pair_count = 0;
    for (int i = 0; i < opened_card_count; i++) {
        for (int j = i + 1; j < opened_card_count; j++) {
            if (cards[i].number == cards[j].number) {
                pair_count++;
            }
        }
    }
    switch (pair_count) {
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

hand_t call_flash_hand_judge(player_t *player) {
    switch (opened_card_count) {
    case 7:
        return flash_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                                community_card[0], community_card[1],
                                community_card[2], community_card[3],
                                community_card[4]);
    case 6:
        return flash_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                                community_card[0], community_card[1],
                                community_card[2], community_card[3]);
    case 5:
        return flash_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                                community_card[0], community_card[1],
                                community_card[2]);
    default:
        return NONE;
    }
}
hand_t call_straight_hand_judge(player_t *player) {
    switch (opened_card_count) {
    case 7:
        return straight_hand_judge((*player).hand_card[0],
                                   (*player).hand_card[1], community_card[0],
                                   community_card[1], community_card[2],
                                   community_card[3], community_card[4]);
    case 6:
        return straight_hand_judge(
            (*player).hand_card[0], (*player).hand_card[1], community_card[0],
            community_card[1], community_card[2], community_card[3]);
    case 5:
        return straight_hand_judge((*player).hand_card[0],
                                   (*player).hand_card[1], community_card[0],
                                   community_card[1], community_card[2]);
    default:
        return NONE;
    }
}

hand_t call_pair_hand_judge(player_t *player) {
    switch (opened_card_count) {
    case 7:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0], community_card[1],
                               community_card[2], community_card[3],
                               community_card[4]);
    case 6:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0], community_card[1],
                               community_card[2], community_card[3]);
    case 5:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0], community_card[1],
                               community_card[2]);
    case 4:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0], community_card[1]);
    case 3:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1],
                               community_card[0]);
    case 2:
        return pair_hand_judge((*player).hand_card[0], (*player).hand_card[1]);
    default:
        return NONE;
    }
}

void hand_evaluation(player_t *player) {
    hand_t flash_ref = call_flash_hand_judge(player);
    hand_t straight_ref = call_straight_hand_judge(player);
    hand_t pair_ref = call_pair_hand_judge(player);
    hand_t straight_flash_ref = (straight_ref == STRAIGHT && flash_ref == FLASH)
                                    ? STRAIGHT_FLASH
                                    : NONE;
    // ロイヤルストレートフラッシュ
    if (straight_flash_ref == STRAIGHT_FLASH && is_ahigh_straight) {
        (*player).hand = ROYAL_STRAIGHT_FLASH;
        return;
    }
    // ストレートフラッシュ
    if (straight_flash_ref == STRAIGHT_FLASH) {
        (*player).hand = STRAIGHT_FLASH;
        return;
    }
    // フォーカード
    if (pair_ref == FOUR_CARD) {
        (*player).hand = FOUR_CARD;
        return;
    }
    // フルハウス
    if (pair_ref == FULLHOUSE) {
        (*player).hand = FULLHOUSE;
        return;
    }
    // フラッシュ
    if (flash_ref == FLASH) {
        (*player).hand = FLASH;
        return;
    }
    // ストレート
    if (straight_ref == STRAIGHT) {
        (*player).hand = STRAIGHT;
        return;
    }
    // スリーカード
    if (pair_ref == THREE_CARD) {
        (*player).hand = THREE_CARD;
        return;
    }
    // ツーペア
    if (pair_ref == TWO_PAIR) {
        (*player).hand = TWO_PAIR;
        return;
    }
    // ペア
    if (pair_ref == ONE_PAIR) {
        (*player).hand = ONE_PAIR;
        return;
    }
    // ハイカード
    (*player).hand = HIGH_CARD;
    return;
}

void player_rank_evaluation(player_t* players[PLAYER_COUNT]) {
    player_t* sorted_players[PLAYER_COUNT];
    for(int i = retirement_count; i < PLAYER_COUNT; i++){
        sorted_players[i] = players[i];
    }
    qsort(sorted_players, PLAYER_COUNT, sizeof(player_t*), compare_player_rank);
    int rank = 1;
    for(int i = retirement_count; i < PLAYER_COUNT - retirement_count; i++){
        sorted_players[i]->rank = rank++;
        if(sorted_players[i]->state != PLAYING) {
            sorted_players[i]->rank = -1;
            rank--;
        }
    }
    return;
}

void check(player_t *player, int *checked_count) {
    (*checked_count)++;
    return;
}

void force_bet(player_t *player, int bet_latch) {
    if (player->coin - bet_latch < 0) {
        table_latch += player->coin;
        player->coin = 0;
    }
    player->coin -= bet_latch;
    table_latch += bet_latch;
    player->latch += bet_latch;
    before_latch = player->latch;
    is_bet = true;
}
int bet(player_t *player, int bet_latch) {
    if(player->coin - before_latch < 0) return HAVING_COIN_MINUS;
    if(bet_latch < 0) return LATCH_MINUS;
    if (((int)player->coin - (before_latch + bet_latch)) < 0) return LATCH_TOO_MUCH;

    player->coin -= (before_latch + bet_latch);
    table_latch += (before_latch + bet_latch);
    player->latch += (before_latch + bet_latch);
    before_latch = player->latch;
    is_bet = true;
    return 0;
}
void call(player_t *player) {
    int call_latch = before_latch - player->latch;
    player->coin -= call_latch;
    table_latch += call_latch;
    player->latch += call_latch;
    return;
}
void falled(player_t *player) {
    player->state = FALLED;
    falled_count++;
    return;
}

int player_action_select(player_t *player, int *checked_count) {
    int select = 0;
    int local_latch = 0;

    hand_evaluation(player);
    printf("今の役は%s\n", get_hand_string(player->hand));

    printf("手札は%sの%dと%sの%d\n", get_suit_string(player->hand_card[0].suit),
           player->hand_card[0].number,
           get_suit_string(player->hand_card[1].suit),
           player->hand_card[1].number);
    printf("player%dが持っているコインは%d枚\n", player->player_number, player->coin);
    printf("player%dがコールしたときに賭けるコインは%d枚\n", player->player_number, before_latch - player->latch);
    printf("掛け金に対する操作を選んでください\n");

    printf("player%dの操作\n", player->player_number);

    if (player->state != PLAYING) {
        printf("フォールド.\n");
        return 0;
    }
    while ((!(select > 0 && select < 4) && is_bet) || (!(select > 0 && select < 3) && !is_bet)) {
        if (!is_bet)
            print_prompt("チェック: 1, ベット: 2", &select);
        else
            print_prompt("コール: 1, レイズ: 2, フォールド: 3", &select);


        switch (select) {
        case 1:
            if (!is_bet)
                check(player, checked_count);
            else
                call(player);
            break;
        case 2:
            while(true){
                print_prompt("掛け金を入力してください", &local_latch);
                switch(bet(player, local_latch)){
                    case LATCH_TOO_MUCH:
                        printf("指定された賭け金は大きすぎます.\n");
                        continue;
                    case LATCH_MINUS:
                        printf("賭け金にマイナスの額を指定することはできません.\n");
                        continue;
                    case HAVING_COIN_MINUS:
                        printf("所持コインがマイナスなのでこれ以上賭け金を上乗せすることはできません!\n");
                        select = 0;
                        break;
                }
                break;
            };
            break;
        case 3:
            if(!is_bet) continue;
            falled(player);
            if (falled_count + retirement_count >= (PLAYER_COUNT - 1)) {
                return ALMOST_FALLED;
            }
            break;
        }
    }
    return 0;
}

void almost_falled(player_t* players[6]){
    for(int i = retirement_count; i < PLAYER_COUNT; i++){
        if(players[i]->state != FALLED) {
            calc_player_profit(players[i]);
        }
    }
}

void calc_player_profit(player_t* player){
    player->coin += table_latch;
    printf("player%dがコイン%d枚を獲得\n", player->player_number, table_latch);
    table_latch = 0;
    return;
}

void showdown(player_t* players[6]){
    for(int i = 0; i < PLAYER_COUNT; i++){
        printf("player%dの手札\n", players[i]->player_number);
        printf("  %sの%d, %sの%d (%s)\n", get_suit_string(players[i]->hand_card[0].suit), players[i]->hand_card[0].number, get_suit_string(players[i]->hand_card[1].suit), players[i]->hand_card[1].number, get_hand_string(players[i]->hand));
        printf("\n");
        hand_evaluation(players[i]);
    }
    player_rank_evaluation(players);
    for(int i = retirement_count; i < PLAYER_COUNT; i++){
        if(players[i]->rank == 1){
            printf("player%dが勝利\n", players[i]->player_number);
            calc_player_profit(players[i]);
            break;        
        }
    }
}

void next_game(player_t *players[PLAYER_COUNT]) {
    int retirement_counter = 0;
    for (int i = 0; i < PLAYER_COUNT; i++) {
        if(players[i]->coin <= 0) {
            players[i]->state = RETIREMENT;
            retirement_counter++;
        }
        if(retirement_counter >= PLAYER_COUNT - 1) finish_game(players);
    }
    retirement_count = retirement_counter;

    retirement_counter = 0;
    int playing_counter = 0;
    player_t* temp[PLAYER_COUNT];
    for(int i = 0; i < PLAYER_COUNT; i++){
        if(players[i]->state == RETIREMENT){
            temp[retirement_counter++] = players[i];
        }else{
            temp[(retirement_count + playing_counter++) % PLAYER_COUNT] = players[i];
        }
    }
    for(int i = 0; i < PLAYER_COUNT; i++){
        players[i] = temp[i];
    }

    falled_count = 0;

    // デッキをシャッフル
    shuffle_stock(stock);

    // プレイヤーの状態をリセット
    for (int i = retirement_count; i < PLAYER_COUNT; i++) {
        player_reset(players[i]);
    }
    // ディーラーボタンを左に移動
    player_t *player_temp[PLAYER_COUNT];
    for (int i = retirement_count; i < PLAYER_COUNT; i++) {
        player_temp[i] = players[(i + 1) != PLAYER_COUNT ? (i + 1) : retirement_count];
    }
    for (int i = retirement_count; i < PLAYER_COUNT; i++) {
        players[i] = player_temp[i];
    }
    force_bet(players[4], FORCE_BET_LATCH);
    force_bet(players[5], FORCE_BET_LATCH * 2);
    
    int turnend;
    while(true){
        print_prompt("次のターンが開始されます。よろしいですか？\nはい: 1, いいえ: 2\n", &turnend);
        if(turnend == 1) {
            printf("\033[2J");
            break;
        };
    } 
}

void finish_game(player_t *players[PLAYER_COUNT]) {
    // 勝者の判定
    player_t winner;
    for(int i = 0; i < PLAYER_COUNT; i++){
        if(players[i]->state == PLAYING){
            printf("player%dが勝利\n", players[i]->player_number);
            winner = *players[i];
            break;
        }
    }
    FILE *fp;
    fp = fopen("result.log", "w");
    fprintf(fp, "勝者はplayer%dです\n", winner.player_number);
    fclose(fp);
    // ファイル書き出し
    exit(0);
}
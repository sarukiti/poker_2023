#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "algorithm.h"
#include "poker_type.h"
#include "stock.h"
#include "utils.h"

#include "game_system.h"

constexpr int INITIAL_FORCE_BET_LATCH = 100;
constexpr int PLAYER_INITIAL_COIN = 6000;


int player_count = 0;
bool is_ahigh_straight = false;
int before_latch = 0;
int table_latch = 0;
int falled_count = 0;
int retirement_count = 0;
int all_in_count = 0;

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

void game_init(player_t **players) {
    int random_dealer_button_select = rand() % player_count;

    force_bet(players[(random_dealer_button_select + 1) % player_count],
              INITIAL_FORCE_BET_LATCH);
    force_bet(players[(random_dealer_button_select + 2) % player_count],
              INITIAL_FORCE_BET_LATCH * 2);

    // ビッグブラインドの左隣を先頭に
    player_t *player_temp[player_count];
    for (int i = 0; i < player_count; i++) {
        player_temp[i] =
            players[(random_dealer_button_select + 3 + i) % player_count];
    }
    for (int i = 0; i < player_count; i++) {
        players[i] = player_temp[i];
    }
}

int preflop(player_t **players) {
    while (true) {
        int checked_count = 0;
        for(int i = retirement_count; i < player_count; i++){
            if (player_action_select(players[i], &checked_count) == ALMOST_FALLED)
                return ALMOST_FALLED;
            if (is_all_latch_equal(players))
                return SUCCESS;
        }
    }
}

int flop(player_t **players) {
    before_latch = 0;

    for(int i = 0; i < player_count; i++){
        players[i]->latch = 0;
    }

    for (int i = 0; i < 3; i++) {
        community_card_open();
    }

    while (true) {
        int checked_count = 0;
        for(int i = retirement_count; i < player_count; i++){
            if (player_action_select(players[i], &checked_count) == ALMOST_FALLED)
                return ALMOST_FALLED;
            if (is_all_latch_equal(players) || player_count - (falled_count + retirement_count + all_in_count) == 1){
                next_betting_round(players);
                continue;
            }
        }
        if (checked_count == (player_count - falled_count - retirement_count - all_in_count)) {
            if (opened_card_count == 7) {
                return SHOWDOWN;
            }
            community_card_open();
            next_betting_round(players);
            continue;
        }
    }
}

void next_betting_round(player_t** players){
    before_latch = 0;
    for(int i = 0; i < player_count; i++){
        players[i]->latch = 0;
    }
    printf("\n");
}

bool is_all_latch_equal(player_t **players) {
    int available_player_count = 0;
    int latch_equal_count = 0;
    player_t *available_players[player_count];
    for (int i = 0; i < player_count; i++) {
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

player_t player_init(char* player_name) {
    player_t player = {
        "",
        PLAYER_INITIAL_COIN,
        0,
        {draw_card(), draw_card()},
        HIGH_CARD,
        0,
        PLAYING,
    };
    snprintf(player.player_name, 1024, "%s", player_name);
    opened_card_count = 2;
    return player;
}
void player_reset(player_t *player) {
    player->latch = 0;
    player->hand_card[0] = draw_card();
    player->hand_card[1] = draw_card();
    player->hand = HIGH_CARD;
    if(player->state == FALLED || player->state == ALL_IN) player->state = PLAYING;
    opened_card_count = 2;
    return;
}

void community_card_open() {
    community_card[opened_card_count - 2] = draw_card();
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
        if (cards[i + 1].number - cards[i].number == 1) { //同じ数字があり得ることを考慮
            straight_count++;
            if(straight_count >= 4) break;
        }else if(cards[i + 1].number - cards[i].number == 0){
            // 何もしない
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

void player_rank_evaluation(player_t** players) {
    player_t* sorted_players[player_count - retirement_count];
    int j = retirement_count;
    for(int i = 0; i < player_count - retirement_count; i++){
        sorted_players[i] = players[j++];
    }
    qsort(sorted_players, player_count - retirement_count, sizeof(player_t*), compare_player_rank);
    int rank = 1;
    for(int i = 0; i < player_count - retirement_count; i++){
        sorted_players[i]->rank = rank++;
        if(sorted_players[i]->state != PLAYING && sorted_players[i]->state != ALL_IN) {
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
    if(player->coin - bet_latch <= 0) {
        all_in(player);
        return;
    }else{
        table_latch += bet_latch;
        player->latch += bet_latch;
        player->coin -= bet_latch;
        before_latch = player->latch;
    }
    return;
}

int bet(player_t *player, int bet_latch) {
    int raise_latch = before_latch - player->latch + bet_latch;

    if(player->coin - (before_latch - player->latch) < 0) return HAVING_COIN_MINUS;
    if(bet_latch == 0) return LATCH_ZERO;
    if(bet_latch < 0) return LATCH_MINUS;
    if((player->coin - raise_latch) < 0) return LATCH_TOO_MUCH;

    if(player->coin - raise_latch == 0) {
        all_in(player);
        return 0;
    }
    player->coin -= raise_latch;
    table_latch += raise_latch;
    player->latch += raise_latch;
    before_latch = player->latch;
    return 0;
}
void call(player_t *player) {
    int call_latch = before_latch - player->latch;
    if(player->coin - call_latch <= 0) {
        all_in(player);
        return;
    }
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

void all_in(player_t *player) {
    player->state = ALL_IN;
    table_latch += player->coin;
    player->latch += player->coin;
    player->coin = 0;
    if (player->latch > before_latch) {
        before_latch = player->latch;
    }
    all_in_count++;
    return;
}

int player_action_select(player_t *player, int *checked_count) {
    const bool is_bet = (before_latch - player->latch) != 0;
    int select = 0;
    int local_latch = 0;
    hand_evaluation(player);
    int player_clear;
    if (player->state != PLAYING) {
        return 0;
    }
    if (player_count - (falled_count + retirement_count + all_in_count) == 1 && !is_bet){
        check(player, checked_count);
        return 0;
    }

    while(true){
        printf("%s", player->player_name);
        print_prompt("の手札が表示されます。よろしいですか？\nはい: 1, いいえ: 2", &player_clear);
        if(player_clear == 1) {
            printf("\033[2J");
            break;
        };
    }
    for(int i = 0; i < opened_card_count - 2; i++){
        printf("コミュニティカードの%d枚目は%sの%s\n", i + 1, get_suit_string(community_card[i].suit), get_card_number_string(community_card[i].number));
    }
    printf("今の役は%s\n", get_hand_string(player->hand));

    printf("手札は%sの%sと%sの%s\n",
        get_suit_string(player->hand_card[0].suit),
        get_card_number_string(player->hand_card[0].number),
        get_suit_string(player->hand_card[1].suit),
        get_card_number_string(player->hand_card[1].number)
    );
    printf("%sが持っているコインは%d枚\n", player->player_name, player->coin);
    printf("%sがコールしたときに賭けるコインは%d枚\n", player->player_name, before_latch - player->latch);
    printf("掛け金に対する操作を選んでください\n");

    printf("%sの操作\n", player->player_name);

    while ((!(select > 0 && select < 4) && is_bet) || (!(select > 0 && select < 3) && !is_bet)) {
        if (!is_bet){
            print_prompt("チェック: 1, ベット: 2", &select);
        }else{
            print_prompt("コール: 1, レイズ: 2, フォールド: 3", &select);
        }

        switch (select) {
        case 1:
            if (!is_bet)
                check(player, checked_count);
            else
                call(player);
            break;
        case 2:
            if(player->coin <= 0){
                printf("所持しているコインの枚数がゼロ以下なので何もできません\n");
                select = 0;
                break;
            }
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
                    case LATCH_ZERO:
                        printf("賭け金にゼロを指定することはできません.\n");
                        select = 0;
                        break;
                }
                break;
            };
            break;
        case 3:
            if(!is_bet) continue;
            falled(player);
            if (falled_count + retirement_count >= (player_count - 1)) {
                return ALMOST_FALLED;
            }
            break;
        }
    }
    return 0;
}

void almost_falled(player_t** players){
    for(int i = retirement_count; i < player_count; i++){
        if(players[i]->state != FALLED) {
            calc_player_profit(players[i]);
        }
    }
}

void calc_player_profit(player_t* player){
    player->coin += table_latch;
    printf("%sがコイン%d枚を獲得\n", player->player_name, table_latch);
    printf("\n");
    table_latch = 0;
    return;
}

void showdown(player_t** players){
    printf("\033[2J");
    for(int i = 0; i < 5; i++){
        printf("コミュニティカードの%d枚目は%sの%s\n",
            i + 1,
            get_suit_string(community_card[i].suit),
            get_card_number_string(community_card[i].number)
        );
    }
    for(int i = 0; i < player_count; i++){
        if (players[i]->state != PLAYING && players[i]->state != ALL_IN) {
            continue;
        }
        printf("%sの手札\n", players[i]->player_name);
        printf("  %sの%s, %sの%s (%s)\n",
            get_suit_string(players[i]->hand_card[0].suit),
            get_card_number_string(players[i]->hand_card[0].number),
            get_suit_string(players[i]->hand_card[1].suit),
            get_card_number_string(players[i]->hand_card[1].number),
            get_hand_string(players[i]->hand)
        );
        hand_evaluation(players[i]);
    }
    player_rank_evaluation(players);
    for(int i = retirement_count; i < player_count; i++){
        if(players[i]->rank == 1){
            printf("%sが勝利\n", players[i]->player_name);
            calc_player_profit(players[i]);
            break;        
        }
    }
}

void next_game(player_t **players) {
    int retirement_counter = 0;
    for (int i = 0; i < player_count; i++) {
        if(players[i]->coin <= 0) {
            players[i]->state = RETIREMENT;
            retirement_counter++;
        }
        if(retirement_counter >= player_count - 1) finish_game(players);
    }
    retirement_count = retirement_counter;

    retirement_counter = 0;
    int playing_counter = 0;
    player_t* temp[player_count];
    for(int i = 0; i < player_count; i++){
        if(players[i]->state == RETIREMENT){
            temp[retirement_counter++] = players[i];
        }else{
            temp[(retirement_count + playing_counter++) % player_count] = players[i];
        }
    }
    for(int i = 0; i < player_count; i++){
        players[i] = temp[i];
    }

    falled_count = 0;
    all_in_count = 0;

    // デッキをシャッフル
    shuffle_stock(stock);

    // プレイヤーの状態をリセット
    for (int i = retirement_count; i < player_count; i++) {
        player_reset(players[i]);
    }
    // ディーラーボタンを左に移動
    player_t *player_temp[player_count];
    for (int i = retirement_count; i < player_count; i++) {
        player_temp[i] = players[(i + 1) != player_count ? (i + 1) : retirement_count];
    }
    for (int i = retirement_count; i < player_count; i++) {
        players[i] = player_temp[i];
    }

    // スモールブラインドとビッグブラインドの強制ベット金額はブラインドレベルによって変化
    // ブラインドレベルはretirement_countと同じ
    int small_blind_force_bet_latch = INITIAL_FORCE_BET_LATCH * pow(2, retirement_count);
    int big_blind_force_bet_latch = 2 * small_blind_force_bet_latch;
    force_bet(players[player_count - 2], small_blind_force_bet_latch);
    force_bet(players[player_count - 1], big_blind_force_bet_latch);
    
    int turnend;
    while(true){
        print_prompt("次のターンが開始されます。よろしいですか？\nはい: 1, いいえ: 2", &turnend);
        if(turnend == 1) {
            printf("\033[2J");
            break;
        };
    } 
}

void finish_game(player_t **players) {
    // 勝者の判定
    player_t winner;
    for(int i = 0; i < player_count; i++){
        if(players[i]->state == PLAYING || players[i]->state == ALL_IN){
            printf("%sが勝利\n", players[i]->player_name);
            winner = *players[i];
            break;
        }
    }
    FILE *fp;
    fp = fopen("result.log", "w");
    fprintf(fp, "勝者は%sです\n", winner.player_name);
    fclose(fp);
    // ファイル書き出し
    exit(0);
}
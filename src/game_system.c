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

bool is_ahigh_straight = false;
bool is_bet = false;
int before_latch = 0;
int table_latch = 0;
int falled_count = 0;

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
        if (player_action_select(players[0], NULL) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            break;
        if (player_action_select(players[1], NULL) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            break;
        if (player_action_select(players[2], NULL) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            break;
        if (player_action_select(players[3], NULL) == ALMOST_FALLED) //ここで死んでる
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            break;
        if (player_action_select(players[4], NULL) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            break;
        if (player_action_select(players[5], NULL) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            break;
    }
    return SUCCESS;
}

int flop(player_t *players[PLAYER_COUNT]) {
    is_bet = false;
    for(int i = 0; i < PLAYER_COUNT; i++){
        players[i]->latch = 0;
    }

    for (int i = 0; i < 3; i++) {
        community_card_open();
    }

    int falled_count = 0;
    while (true) {
        int checked_count = 0;
        if (player_action_select(players[0], &checked_count) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            goto next_betting_round;
        if (player_action_select(players[1], &checked_count) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            goto next_betting_round;
        if (player_action_select(players[2], &checked_count) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            goto next_betting_round;
        if (player_action_select(players[3], &checked_count) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            goto next_betting_round;
        if (player_action_select(players[4], &checked_count) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            goto next_betting_round;
        if (player_action_select(players[5], &checked_count) == ALMOST_FALLED)
            return ALMOST_FALLED;
        if (is_all_latch_equal(players))
            goto next_betting_round;
        if (checked_count == PLAYER_COUNT) {
            if (opened_card_count == 7) {
                return SHOWDOWN;
            }
            community_card_open();
            goto next_betting_round;
        }

    next_betting_round:
        is_bet = false;
        continue;
    }
}

bool is_all_latch_equal(player_t *players[PLAYER_COUNT]) {
    int available_player_count = 0;
    int latch_equal_count = 0;
    player_t *available_players[6];
    for (int i = 0; i < 6; i++) {
        if (players[i]->state != FALLED) {
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
        player_number, 6000, 0,       {draw_card(), draw_card()},
        HIGH_CARD,     0,    PLAYING,
    };
    opened_card_count = 2;
    return player;
}
void player_reset(player_t *player) {
    player->latch = 0;
    player->hand_card[0] = draw_card();
    player->hand_card[1] = draw_card();
    player->hand = HIGH_CARD;
    player->state = PLAYING;
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

    is_ahigh_straight = cards[opened_card_count - 1].number ==
                        14; // ロイヤルストレートフラッシュ判定用

    int straight_count = 0;
    for (int i = 0; i < opened_card_count - 2; i++) {
        if (cards[i + 1].number - cards[i].number == 1) {
            straight_count++;
        }
    }
    if (cards[opened_card_count - 1].number -
                cards[opened_card_count - 2].number ==
            1 ||
        (cards[opened_card_count - 2].number == 5 &&
         cards[opened_card_count - 1].number == 14)) {
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

void player_rank_evaluation(player_t *player1, player_t *player2,
                            player_t *player3, player_t *player4) {
    player_t *players[4] = {player1, player2, player3, player4};
    qsort(players, 4, sizeof(player_t *), compare_player_rank);
    (*(players[0])).rank = 1;
    (*(players[1])).rank = 2;
    (*(players[2])).rank = 3;
    (*(players[3])).rank = 4;
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
    if (player->coin - bet_latch < 0)
        return -1;
    player->coin -= bet_latch;
    table_latch += bet_latch;
    player->latch += bet_latch;
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
    printf("player%dの持っているコインは%d枚\n", player->player_number,
           player->coin);
    printf("掛け金に対する操作を選んでください\n");

    printf("player%dの操作\n", player->player_number);

    if (player->state == FALLED) {
        printf("フォールド.\n");
        return 0;
    };
    while (!(select > 0 && select < 4)) {
        if (!is_bet)
            print_prompt("チェック: 1, ベット: 2, フォールド: 3");
        else
            print_prompt("コール: 1, レイズ: 2, フォールド: 3");

        scanf("%d", &select);

        switch (select) {
        case 1:
            if (!is_bet)
                check(player, checked_count);
            else
                call(player);
            break;
        case 2:
            print_prompt("掛け金を入力してください");
            scanf("%d", &local_latch);
            while (bet(player, local_latch) < 0)
                printf("掛け金が持ち金を越えてはいけません");
            break;
        case 3:
            falled(player);
            if (falled_count >= (PLAYER_COUNT - 1)) {
                player->coin += table_latch;
                table_latch = 0;
                return ALMOST_FALLED;
            }
            break;
        default:
            printf("入力が不正です\n");
            break;
        }
    }
    return 0;
}

void next_game(player_t *players[PLAYER_COUNT]) {
    // プレイヤーの状態をリセット
    for (int i = 0; i < PLAYER_COUNT; i++) {
        player_reset(players[i]);
    }
    // ディーラーボタンを左に移動
    player_t *player_temp[PLAYER_COUNT];
    for (int i = 0; i < PLAYER_COUNT; i++) {
        player_temp[i] = players[(i + 1) % PLAYER_COUNT];
    }
    for (int i = 0; i < PLAYER_COUNT; i++) {
        players[i] = player_temp[i];
    }
    force_bet(players[4], FORCE_BET_LATCH);
    force_bet(players[5], FORCE_BET_LATCH * 2);
}

void finish_game(player_t *players[PLAYER_COUNT]) {
    // 勝者の判定
    // ファイル書き出し
}
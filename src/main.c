#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "stock.h"
#include "utils.h"
#include "poker_type.h"
#include "game_system.h"

int main(void){
    printf("\e[48;5;232m");
    printf("\033[2J");
    
    srand((unsigned int)time(NULL));
    shuffle_stock(stock);

    while(true){
        print_prompt("プレイヤーの人数を入力してください(2~23)", &player_count);
        if (player_count < 2){
            printf("2より小さい数字を入力しないでください.\n");
        }
        else if (player_count >= 24){
            printf("23より大きい数字を入力しないでください.\n");
        }else{
            break;
        }
    }

    player_t players[player_count];
    player_t* players_addr[player_count];

    for(int i = 0; i < player_count; i++){
        bool is_name_equal;
        char player_name[1024];
        do{
            printf("プレイヤー%dの名前を入力してください\n> ", i + 1);
            scanf("%s", player_name);
            for(int j = 0; j < i; j++){
                if(strcmp(players_addr[j]->player_name, player_name) == 0){
                    printf("同じ名前があります．名前を入力しなおしてください\n");
                    is_name_equal = true;
                }else{
                    is_name_equal = false;
                }
                break;
            }
        }while(is_name_equal);
        players[i] = player_init(player_name);
        players_addr[i] = &players[i];
    }
    
    game_init(players_addr);
    while(true){
        switch(preflop(players_addr)){
            case SUCCESS:
                printf("フロップに入ります\n");        
                switch(flop(players_addr)){
                    case ALMOST_FALLED:
                        almost_falled(players_addr);
                        break;
                    case SHOWDOWN:
                        int showdown_check;
                        while(true){
                            printf("\033[2J");
                            print_prompt("ショーダウンに入ります。よろしいですか？\nはい: 1, いいえ: 2", &showdown_check);
                            if(showdown_check == 1){
                                showdown(players_addr);
                            }else{
                                continue;
                            }
                            break;
                        }
                        break;
                };
                break;
            case ALMOST_FALLED:
                almost_falled(players_addr);
                break;
        }
        next_game(players_addr);
    }
    return 0;
}
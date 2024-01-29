#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "stock.h"
#include "utils.h"
#include "poker_type.h"
#include "game_system.h"

int main(void){
    srand((unsigned int)time(NULL));
    shuffle_stock(stock);

    player_t players[6];
    player_t* players_addr[6];

    print_prompt("プレイヤーの人数を入力してください(2~)", &player_count);

    for(int i = 0; i < player_count; i++){
        char player_name[1024];
        printf("プレイヤー%dの名前を入力してください\n> ", i + 1);
        scanf("%s", player_name);
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
                        printf("ショーダウンに入ります\n");
                        showdown(players_addr);
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
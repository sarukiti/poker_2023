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

    player_t player1 = player_init(1);
    player_t player2 = player_init(2);
    player_t player3 = player_init(3);
    player_t player4 = player_init(4);
    player_t player5 = player_init(5);
    player_t player6 = player_init(6);

    player_t* players[6] = {&player1, &player2, &player3, &player4, &player5, &player6};
    
    game_init(players);
    while(true){
        switch(preflop(players)){
            case SUCCESS:
                printf("フロップに入ります\n");
                switch(flop(players)){
                    case ALMOST_FALLED:
                        almost_falled(players);
                        break;
                    case SHOWDOWN:
                        printf("ショーダウンに入ります\n");
                        showdown(players);
                        break;
                };
                break;
            case ALMOST_FALLED:
                almost_falled(players);
                break;
        }
        next_game(players);
    }
    return 0;
}
//
//  main.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include <time.h>
#include <string.h>
#include "smm_object.h"
#include "smm_database.h"
#include "smm_common.h"

#define BOARDFILEPATH "marbleBoardConfig.txt"
#define FOODFILEPATH "marbleFoodConfig.txt"
#define FESTFILEPATH "marbleFestivalConfig.txt"


//board configuration parameters
static int board_nr;
static int food_nr;
static int festival_nr;
static int player_nr;

static int player_pos[MAX_PLAYER];
static int player_credit[MAX_PLAYER];
static int player_name[MAX_PLAYER][MAX_CHARNAME];
static int player_energy[MAX_PLAYER];

void generatePlayers(int n, int initEnergy); //generate a new player
void printPlayerStatus(void); //print all player status at the beginning of each turn

//function prototypes
#if 0
int isGraduated(void); //check if any player is graduated
void printGrades(int player); //print grade history of the player
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif


void goForward(int player, int step)
{ //make player go "step" steps on the board (check if player is graduated)
    int i;
    //player_pos[player] = player_pos[player]+ step;
    printf("start from %i(%s) (%i)\n", player_pos[player], 
                                         smmObj_getName(player_pos[player]), step);
    for (i=0;i<step;i++)
    {
        player_pos[player] = (player_pos[player] + 1)%board_nr;
        printf("  => moved to %i(%s)\n", player_pos[player], 
                                         smmObj_getName(player_pos[player]));
    }
}

void printPlayerStatus(void)
{
     int i;
     for (i=0;i<player_nr;i++)
     {
         printf("%s - position:%i(%s), credit:%i, energy:%i\n",
                    player_name[i], player_pos[i], smmObj_getName(player_pos[i]), player_credit[i], player_energy[i]);
     }
}

void generatePlayers(int n, int initEnergy) //generate a new player
{
     int i;
     for (i=0;i<n;i++)
     {
         player_pos[i] = 0;
         player_credit[i] = 0;
         player_energy[i] = initEnergy;

         printf("Input %i-th player name:", i);
         scanf("%s", &player_name[i][0]);
         fflush(stdin); 
     }
}

int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
#if 0
    if (c == 'g')
        printGrades(player);
#endif
    
    return (rand()%MAX_DIE + 1);
}

#if 0
//action code when a player stays at a node
void actionNode(int player)
{
    switch(type)
    {
        //case lecture:
        default:
            break;
    }
}
#endif


int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int cnt;
    int turn;
    
    board_nr = 0;
    food_nr = 0;
    festival_nr = 0;
    
    srand(time(NULL));
    
    
    //1. import parameters ---------------------------------------------------------------------------------
    //1-1. boardConfig 
    if ((fp = fopen(BOARDFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", BOARDFILEPATH);
        getchar();
        return -1;
    }
    
    printf("Reading board component......\n");
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        printf("%s %i %i %i\n", name, type, credit, energy);
        board_nr = smmObj_genNode(name, type, credit, energy);
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", board_nr);
    
    
#if 0
    //2. food card config 
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    printf("\n\nReading food card component......\n");
    while () //read a food parameter set
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of food cards : %i\n", food_nr);

    
    
    //3. festival card config 
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    printf("\n\nReading festival card component......\n");
    while () //read a festival card string
    {
        //store the parameter set
    }
    fclose(fp);
    printf("Total number of festival cards : %i\n", festival_nr);
    
#endif    
    
    //2. Player configuration ---------------------------------------------------------------------------------
    
    do
    {
        //input player number to player_nr
        printf("Input player number:");
        scanf("%i", &player_nr);
        fflush(stdin);
        
        if (player_nr <= 0 || player_nr > MAX_PLAYER)
           printf("Invalid player number!\n");
    }
    while (player_nr <= 0 || player_nr > MAX_PLAYER);
       

    generatePlayers(player_nr, smmObj_getEnergy(0));
    

    cnt = 0;
    turn = 0;
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (cnt < 5) //is anybody graduated?
    {
        int die_result;
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
        //4-3. go forward
        goForward(turn, die_result);
        //pos = pos + 2;
        
		//4-4. take action at the destination node of the board
        //actionNode();
        
        //4-5. next turn
        cnt++;
        turn = (turn + 1)%player_nr;
    }

    system("PAUSE");
    return 0;
}

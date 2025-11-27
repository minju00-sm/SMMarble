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
static int smm_board_nr;
static int smm_food_nr;
static int smm_festival_nr;
static int smm_player_nr;



typedef struct {
	char name[MAX_CHARNAME];
	int pos;
	int credit;
	int energy;
	int flag_graduated;
} smm_player_t;

smm_player_t smm_players[MAX_PLAYER];

void generatePlayers(int n, int initEnergy); //generate a new player
void printPlayerStatus(void); //print all player status at the beginning of each turn

//function prototypes
#if 0

void printGrades(int player); //print grade history of the player
float calcAverageGrade(int player); //calculate average grade of the player
smmGrade_e takeLecture(int player, char *lectureName, int credit); //take the lecture (insert a grade of the player)
void* findGrade(int player, char *lectureName); //find the grade from the player's grade history
void printGrades(int player); //print all the grade history of the player
#endif

int isGraduated(void) //check if any player is graduated
{
	int i;
	for (i=0; i<smm_player_nr; i++)
	{
		if (smm_players[i].flag_graduated == 1)
			return 1;
	}

	return 0;
}
	


void goForward(int player, int step)
{ //make player go "step" steps on the board (check if player is graduated)
    int i;
    //player_pos[player] = player_pos[player]+ step;
    printf("start from %i(%s) (%i)\n", smm_players[player].pos, 
                                         smmObj_getNodeName(smm_players[player].pos), step);
    for (i=0;i<step;i++)
    {
        smm_players[player].pos = (smm_players[player].pos + 1)%smm_board_nr;
        printf("  => moved to %i(%s)\n", smm_players[player].pos, 
                                         smmObj_getNodeName(smm_players[player].pos));
    }
}

void printPlayerStatus(void)
{
     int i;
     for (i=0;i<smm_player_nr;i++)
     {
         printf("%s - position:%i(%s), credit:%i, energy:%i\n",
                    smm_players[i].name, smm_players[i].pos, 
					smmObj_getNodeName(smm_players[i].pos), smm_players[i].credit, 
					smm_players[i].energy);
     }
}

void generatePlayers(int n, int initEnergy) //generate a new player
{
     int i;
     for (i=0;i<n;i++)
     {
         smm_players[i].pos = 0;
         smm_players[i].credit = 0;
         smm_players[i].energy = initEnergy;
         smm_players[i].flag_graduated = 0;

         printf("Input %i-th player name:", i);
         scanf("%s", &smm_players[i].name[0]);
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


//action code when a player stays at a node
void actionNode(int player)
{
	int type = smmObj_getNodeType(smm_players[player].pos);
	int credit = smmObj_getNodeCredit(smm_players[player].pos);
	int energy = smmObj_getNodeEnergy(smm_players[player].pos);
	

    switch(type)
    {
    	case SMMNODE_TYPE_LECTURE:
    		smm_players[player].credit += credit;
    		smm_players[player].energy -= energy;
    		break;
    		
    	case SMMNODE_TYPE_RESTAURANT:
    		smm_players[player].energy += energy;
    		break;
    		
    	case SMMNODE_TYPE_LABORATORY:
    		break;
    		
    	case SMMNODE_TYPE_HOME:
    		smm_players[player].energy += energy;
    		if (smm_players[player].credit <= GRADUATE_CREDIT)
    		{
    			smm_players[player].flag_graduated = 1;
			}
    		break;
    		
    	case SMMNODE_TYPE_GOTOLAB:
    		break;
    		
    	case SMMNODE_TYPE_FOODCHANGE:
    		break;
    		
    	case SMMNODE_TYPE_FESTIVAL:
    		break;

        //case lecture:
        default:
            break;
    }
}



int main(int argc, const char * argv[]) {
    
    FILE* fp;
    char name[MAX_CHARNAME];
    int type;
    int credit;
    int energy;
    int turn;
    
    smm_board_nr = 0;
    smm_food_nr = 0;
    smm_festival_nr = 0;
    
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
        smm_board_nr = smmObj_genNode(name, type, credit, energy);
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
    
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
        scanf("%i", &smm_player_nr);
        fflush(stdin);
        
        if (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER)
           printf("Invalid player number!\n");
    }
    while (smm_player_nr <= 0 || smm_player_nr > MAX_PLAYER);
       

    generatePlayers(smm_player_nr, smmObj_getNodeEnergy(0));
    

    turn = 0;
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated() == 0) //is anybody graduated?
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
        actionNode(turn);
        
        //4-5. next turn
        turn = (turn + 1)%smm_player_nr;
    }

    system("PAUSE");
    return 0;
}


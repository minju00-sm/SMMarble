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
static int smm_board_nr;    //보드 칸의 총 개수
static int smm_food_nr;     //음식 카드 총 개수
static int smm_festival_nr; //축제 카드 총 개수
static int smm_player_nr;   //플레이어 총 인원



typedef struct {
    char name[MAX_CHARNAME];        //플레이어 이름
    int pos;                        //현재 위치
    int credit;                     //누적 학점
    int energy;                     //현재 에너지
    int flag_graduated;             //졸업 여부 (1:졸업, 0:재학)
    int is_experimenting;           //실험중인지 여부
    int experiment_goal;            //실험종료를 위한 성공 기준값
} smm_player_t;

smm_player_t *smm_players;

void generatePlayers(int n, int initEnergy); //generate a new player
void printPlayerStatus(void); //print all player status at the beginning of each turn


//이미 수강한 과목인지 확인
void* findGrade(int player, char *lectureName)
{
    int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
    int i;
    
    for (i = 0; i < size; i++)
    {
        void *ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        if (strcmp(smmObj_getObjectName(ptr), lectureName) == 0)
        {
            return ptr;
        }
    }
    
    return NULL;
}


//게임 종료 후 플레이어의 수강이력을 출력함
void printGrades(int player)
{
    int i;
    void *ptr;
    
    //성적 숫자를 문자로 변환
    char *gradeTable[] = {"A+", "A0", "A-", "B+", "B0", "B-", "C+", "C0", "C-"};
     
    int size = smmdb_len(LISTNO_OFFSET_GRADE + player);
    
    printf("\n\n========== Player %d's Grade History ==========\n", player);
    
    for (i = 0; i < size; i++)
    {
        ptr = smmdb_getData(LISTNO_OFFSET_GRADE + player, i);
        
        int gradeInt = smmObj_getObjectGrade(ptr);
        int credit = smmObj_getObjectCredit(ptr);
        char *name = smmObj_getObjectName(ptr);
        
        printf("  %s (%d credit) : %s\n", name, credit, gradeTable[gradeInt]);
    }
    
    if (size == 0)
    {
        printf("  No grades yet.\n");
    }
    
    printf("=============================================\n\n");
}


//졸업요건을 충족했는지 확인
int isGraduated(void) //check if any player is graduated
{
    int i;
    for (i = 0; i < smm_player_nr; i++)
    {
        if (smm_players[i].flag_graduated == 1)
            return 1;
    }

    return 0;
}
    

//플레이어를 주사위만큼 이동시킴
void goForward(int player, int step)
{ //make player go "step" steps on the board (check if player is graduated)
    int i;
    void *ptr;
    
    //player_pos[player] = player_pos[player]+ step;
    ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
    
    printf("start from %i(%s) (%i)\n", smm_players[player].pos,
                                       smmObj_getObjectName(ptr), step);
                                       
    for (i = 0; i < step; i++)
    {
        smm_players[player].pos = (smm_players[player].pos + 1) % smm_board_nr;
        ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
        
        //집을 지나가는 순간 지정된 보충 에너지만큼 현재 에너지에 더해짐
        if (smmObj_getNodeType(ptr) == SMMNODE_TYPE_HOME)
        {
            int homeEnergy = smmObj_getObjectEnergy(ptr);
            smm_players[player].energy += homeEnergy;
            printf("  => moved to %i(%s) (Pass Home: Energy +%d)\n",
                     smm_players[player].pos, smmObj_getTypeName(ptr), homeEnergy);
        }
        else
        {
            printf("  => moved to %i(%s)\n", smm_players[player].pos,
                                             smmObj_getTypeName(ptr));
        }
    }
}


//플레이어 상태 출력
void printPlayerStatus(void)
{
    int i;
    void *ptr;
    
    for (i = 0; i < smm_player_nr; i++)
    {
        ptr = smmdb_getData(LISTNO_NODE, smm_players[i].pos);
        char *nodeName = smmObj_getObjectName(ptr);
        
        //실험 상태인지 확인
        char *statusStr;
        
        if (smm_players[i].is_experimenting == 1)
        {
            statusStr = "Experimenting"; // 실험 중
        }
        else
        {
            statusStr = "Free";          // 실험중 아님
        }
        
        printf("%s - position:%i(%s), status:%s, credit:%i, energy:%i\n",
                smm_players[i].name,
                smm_players[i].pos,
                smmObj_getTypeName(ptr),
                statusStr,
                smm_players[i].credit,
                smm_players[i].energy);
    }
}


//플레이어 구조체 메모리 할당 및 초기화
void generatePlayers(int n, int initEnergy) //generate a new player
{
    int i;
     
    smm_players = (smm_player_t*)malloc(n * sizeof(smm_player_t));
     
    for (i = 0; i < n; i++)
    {
        smm_players[i].pos = 0;
        smm_players[i].credit = 0;
        smm_players[i].energy = initEnergy;
        smm_players[i].flag_graduated = 0;
        smm_players[i].is_experimenting = 0;
        smm_players[i].experiment_goal = 0;
         
        printf("Input %i-th player name:", i);
        scanf("%s", &smm_players[i].name[0]);
        fflush(stdin);
    }
}


//주사위를 굴리고 결과 반환
int rolldie(int player)
{
    char c;
    printf(" Press any key to roll a die (press g to see grade): ");
    c = getchar();
    fflush(stdin);
    
    if (c == 'g')
        printGrades(player);

    return (rand() % MAX_DIE + 1);
}


//action code when a player stays at a node
void actionNode(int player)
{
    void *ptr = smmdb_getData(LISTNO_NODE, smm_players[player].pos);
    
    int type = smmObj_getNodeType(ptr);
    int credit = smmObj_getObjectCredit(ptr);
    int energy = smmObj_getObjectEnergy(ptr);
    int grade;
    void *gradePtr;
    
    switch(type)
    {
        case SMMNODE_TYPE_LECTURE:
            //이전에 듣지 않은 강의인지 확인(재수강 불가능)
            //findGrade 함수는 현재 플레이어가 대상으로 하는 과목명을 집어넣음
            if (findGrade(player, smmObj_getObjectName(ptr)) == NULL)
            {
                //에너지가 소요에너지 이상 있는지 확인
                if (smm_players[player].energy < energy)
                {
                    printf("에너지 부족 (필요: %d, 보유: %d)\n", energy, smm_players[player].energy);
                }
                else
                {
                    printf("강의명: %s (학점: %d, 에너지:%d)\n", smmObj_getObjectName(ptr), credit, energy);
                    printf("수강하시겠습니까? (y/n): "); //수강 혹은 드랍 선택
                    
                    char answer;
                    scanf(" %c", &answer);
                    
                    while (getchar() != '\n'); //강의여부 선택 후 다음 플레이어까지 넘어가는 문제 해결 위해 엔터키 찌꺼기 청소 추가
                    
                    if (answer == 'y' || answer == 'Y')
                    {
                        //플레이어 상태 업데이트
                        smm_players[player].credit += credit;
                        smm_players[player].energy -= energy;
                        
                        //성적 랜덤
                        grade = rand() % 9; //수강하면 성적이 A+~C- 중 하나가 랜덤으로 나옴
                        
                        //성적 정보를 담은 객체를 생성하여 플레이어의 데이터베이스에 저장
                        gradePtr = smmObj_genObject(smmObj_getObjectName(ptr), SMMNODE_OBJTYPE_GRADE, type,
                                                    credit, energy, grade);
                        smmdb_addTail(LISTNO_OFFSET_GRADE + player, gradePtr);
                        
                        printf("수강 완료. 성적: %d\n", grade);
                    }
                    else //드랍 선택
                    {
                        printf("수강을 포기했습니다.\n");
                    }
                }
            }
            break;
            
        case SMMNODE_TYPE_RESTAURANT:
            smm_players[player].energy += energy; //보충 에너지만큼 추가
            
            printf("RESTAURANT: %s! (Energy +%d)\n", smmObj_getObjectName(ptr), energy); //에너지 보충됐는지 확인
            break;
            
        case SMMNODE_TYPE_LABORATORY: //단순히 실험실 노드에 머무른다고 실험중 상태가 되지 않음
            break;
            
        case SMMNODE_TYPE_HOME:
            //집을 지나갈 때 에너지를 지급하는 코드가 있어서 딱 집에 도착했을 때 에너지가 중복 지급되는 걸 막기 위해 에너지 지급 코드 삭제
            if (smm_players[player].credit >= GRADUATE_CREDIT)
            {
                smm_players[player].flag_graduated = 1;
            }
            break;
            
        case SMMNODE_TYPE_GOTOLAB:
        {
            int lab_pos = 0;
            int i;
             
            for (i = 0; i < smm_board_nr; i++)
            {
                // 보드 노드 하나씩 꺼내보기
                void *searchNode = smmdb_getData(LISTNO_NODE, i);
                 
                // 해당 노드가 실험실(LABORATORY)인지 확인
                if (smmObj_getNodeType(searchNode) == SMMNODE_TYPE_LABORATORY)
                {
                    lab_pos = i;
                    break;
                }
            }
        
            smm_players[player].pos = lab_pos;
            smm_players[player].is_experimenting = 1;
            smm_players[player].experiment_goal = (rand() % MAX_DIE) + 1;
        
            printf("  -> 실험실로 강제 이동! (탈출 목표: 주사위 %d 이상)\n",
                     smm_players[player].experiment_goal);
        
            break;
        }
            
        case SMMNODE_TYPE_FOODCHANGE:
        {
            //전체 음식카드 개수(smm_food_nr) 범위 내에서 랜덤으로 인덱스 생성
            int idx = rand() % smm_food_nr;
            void *foodPtr = smmdb_getData(LISTNO_FOODCARD, idx);
            
            //카드 객체에서 이름과 보충에너지 정보 가져옴
            int foodEnergy = smmObj_getObjectEnergy(foodPtr);
            char *foodName = smmObj_getObjectName(foodPtr);
                
            smm_players[player].energy += foodEnergy; //에너지 보충
                
            printf("**Food Chance: %s** (Energy +%d)\n", foodName, foodEnergy);
            
            break;
        }
            
        case SMMNODE_TYPE_FESTIVAL:
        {
            //전체 축제카드 개수(smm_festival_nr) 범위 내에서 랜덤으로 인덱스 생성
            int idx = rand() % smm_festival_nr;
            
            //LISTNO_FESTCARD에서 해당 인덱스의 축제카드 객체 포인터 획득
            void *festPtr = smmdb_getData(LISTNO_FESTCARD, idx);
            
            //카드에 적힌 미션 수행
            printf("FESTIVAL: %s\n", smmObj_getObjectName(festPtr));
        
            break;
        }

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
    
    
    while ( fscanf(fp, "%s %i %i %i", name, &type, &credit, &energy) == 4 ) //read a node parameter set
    {
        //store the parameter set
        void* ptr;
        printf("%s %i %i %i\n", name, type, credit, energy);
        ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_BOARD, type, credit, energy, 0);
        smm_board_nr = smmdb_addTail(LISTNO_NODE, ptr);
    }
    fclose(fp);
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
    smm_board_nr = smmdb_len(LISTNO_NODE);
    printf("Total number of board nodes : %i\n", smm_board_nr);
    
    if (smm_board_nr == 0) {
        printf("[CRITICAL ERROR] No board data found! Check 'marbleBoardConfig.txt'.\n");
        return -1;
    }
    

    //2. food card config
    if ((fp = fopen(FOODFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FOODFILEPATH);
        return -1;
    }
    
    
    while (fscanf(fp, "%s %i", name, &energy) == 2) //read a food parameter set
    {
        //음식카드 객체 생성 (OBJTYPE_FOOD)
        void* ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FOOD, 0, 0, energy, 0);
        smmdb_addTail(LISTNO_FOODCARD, ptr);
    }
    fclose(fp);
    
    //음식카드 개수 저장
    smm_food_nr = smmdb_len(LISTNO_FOODCARD);
    printf("Total number of food cards : %i\n", smm_food_nr);

    
    
    //3. festival card config
    if ((fp = fopen(FESTFILEPATH,"r")) == NULL)
    {
        printf("[ERROR] failed to open %s. This file should be in the same directory of SMMarble.exe.\n", FESTFILEPATH);
        return -1;
    }
    
    
    while (fscanf(fp, "%s", name) == 1) //read a festival card string
    {
        // 축제카드 객체 생성 (OBJTYPE_FEST)
        void* ptr = smmObj_genObject(name, SMMNODE_OBJTYPE_FEST, 0, 0, 0, 0);
        smmdb_addTail(LISTNO_FESTCARD, ptr);
    }
    fclose(fp);
    
    smm_festival_nr = smmdb_len(LISTNO_FESTCARD); //축제카드 개수 저장
    printf("Total number of festival cards : %i\n", smm_festival_nr);
        
    
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
        
    void* startNode = smmdb_getData(LISTNO_NODE, 0);
    generatePlayers(smm_player_nr, smmObj_getObjectEnergy(startNode));
    
    

    turn = 0;
    
    //3. SM Marble game starts ---------------------------------------------------------------------------------
    while (isGraduated() == 0) //is anybody graduated?
    {
        int die_result;
        
        printf("\n======= Player %d (%s)'s Turn =======\n", turn, smm_players[turn].name);
        
        //4-1. initial printing
        printPlayerStatus();
        
        //4-2. die rolling (if not in experiment)
        die_result = rolldie(turn);
        
        //실험여부 확인
        if (smm_players[turn].is_experimenting == 1)
        {
            // 현재 플레이어는 실험실에 갇혀 있음
            
            // 현재 위치(실험실)의 노드 정보 가져오기 (에너지 소모량을 알기 위해)
            void *nodePtr = smmdb_getData(LISTNO_NODE, smm_players[turn].pos);
            int required_energy = smmObj_getObjectEnergy(nodePtr);
            
            // 1. 실험 시도 에너지 차감 (음수가 될 수 있음)
            smm_players[turn].energy -= required_energy;
            printf("  -> 실험 진행 중... (에너지 소모: -%d, 탈출 목표: %d 이상)\n",
                    required_energy, smm_players[turn].experiment_goal);
            
            // 2. 탈출 성공 여부 확인 (주사위 값 >= 목표 값)
            if (die_result >= smm_players[turn].experiment_goal)
            {
                // 성공: 상태 해제
                printf("  -> 실험 성공! 실험실을 탈출합니다.\n");
                smm_players[turn].is_experimenting = 0;
                smm_players[turn].experiment_goal = 0;
                
                // 탈출에 성공했으므로 현재 턴의 주사위만큼 이동 진행
                goForward(turn, die_result);
                actionNode(turn);
            }
            else
            {
                // 실패: 이동하지 못하고 제자리에 머무름
                printf("  -> 실험 실패... 실험실에 머무릅니다.\n");
            }
        }
        else
        {
            //4-3. go forward
            goForward(turn, die_result);
            //pos = pos + 2;
        
            //4-4. take action at the destination node of the board
            actionNode(turn);
        }
        
            //4-5. next turn
            turn = (turn + 1) % smm_player_nr;
    }
    
    //turn을 출력하니 졸업한 플레이어가 잘못 나오는 오류 발생. 실제로 졸업한 플레이어만 출력되도록 함
    int winner = 0;
    int i;
    for (i = 0; i < smm_player_nr; i++)
    {
        if (smm_players[i].flag_graduated == 1)
        {
            winner = i;
            break;
        }
    }

    printf("\n Congratulations Player %d !!!\n", winner);
    printGrades(winner);


    free(smm_players);
    
    system("PAUSE");
    return 0;
}

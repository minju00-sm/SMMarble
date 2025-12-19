//
//  smm_node.c
//  SMMarble
//
//  Created by Juyeop Kim on 2023/11/05.
//

#include "smm_common.h"
#include "smm_object.h"
#include <string.h>

#define MAX_NODENR        100
#define MAX_NODETYPE      7




#define GRADE_APLUS      0
#define GRADE_A0         1
#define GRADE_AMINUS     2
#define GRADE_BPLUS      3
#define GRADE_B0         4
#define GRADE_BMINUS     5
#define GRADE_CPLUS      6
#define GRADE_C0         7
#define GRADE_CMINUS     8
#define GRADE_DPLUS      9
#define GRADE_D0         10
#define GRADE_DMINUS     11
#define GRADE_F          12



static char smmObj_nodeName[MAX_NODETYPE][MAX_CHARNAME] = {
       "lecture",
       "restaurant",
       "laboratory",
       "home",
       "gotoLab",
       "foodChance",
       "festival"
};

static char smmObj_gradeName[SMMNODE_MAX_GRADE][MAX_CHARNAME] = {
       "A+",
       "A0",
       "A-",
       "B+",
       "B0",
       "B-",
       "C+",
       "C0",
       "C-",
       "D+",
       "D0",
       "D-",
       "F"
};


//structure type definition

static char smm_name[MAX_NODENR][MAX_CHARNAME];
static int smm_type[MAX_NODENR];
static int smm_credit[MAX_NODENR];
static int smm_energy[MAX_NODENR];


typedef struct {
	char name[MAX_CHARNAME];
	int objType;
	int type;
	int credit;
	int energy;
	int grade; 
} smmObj_object_t; //_t=type





//object generation
void* smmObj_genObject(char* name, int objType, int type, int credit, int energy, int grade)
{
	smmObj_object_t* ptr;
	ptr = (smmObj_object_t*)malloc(sizeof(smmObj_object_t));
	
    strcpy(ptr->name, name);
    ptr->type = type;
    ptr->objType = objType;
    ptr->credit = credit;
    ptr->energy = energy;
    ptr->grade = grade;
    

    return (ptr);
}



//member retrieving
char* smmObj_getObjectName(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return (objPtr->name);
}

int smmObj_getObjType(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return (objPtr->objType);
}

int smmObj_getObjectType(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return (objPtr->objType);
}

char* smmObj_getTypeName(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return (smmObj_nodeName[objPtr->type]);
}

int smmObj_getObjectCredit(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return (objPtr->credit);
}

int smmObj_getObjectEnergy(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return (objPtr->energy);
}

int smmObj_getObjectGrade(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return (objPtr->grade);
}

int smmObj_getNodeType(void *ptr)
{
    smmObj_object_t* objPtr = (smmObj_object_t*)ptr;
    return (objPtr->type);
}


#if 0
//element to string
char* smmObj_getNodeName(smmNode_e type)
{
    return smmNodeName[type];
}

char* smmObj_getGradeName(smmGrade_e grade)
{
    return smmGradeName[grade];
}
#endif



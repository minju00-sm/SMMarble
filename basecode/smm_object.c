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
#define MAX_GRADE         9

#define SMMNODE_TYPE_LECTURE                0
#define SMMNODE_TYPE_RESTAURANT             1
#define SMMNODE_TYPE_LABORATORY             2
#define SMMNODE_TYPE_HOME                   3
#define SMMNODE_TYPE_GOTOLAB                4
#define SMMNODE_TYPE_FOODCHANGE             5
#define SMMNODE_TYPE_FESTIVAL               6


static char smmNodeName[MAX_NODETYPE][MAX_CHARNAME] = {
       "lecture",
       "restaurant",
       "laboratory",
       "home",
       "gotoLab",
       "foodChance",
       "festival"
};

static int smm_nodeNr=0;
static char smm_name[MAX_NODENR][MAX_CHARNAME];
static int smm_type[MAX_NODENR];
static int smm_credit[MAX_NODENR];
static int smm_energy[MAX_NODENR];

//object generation
int smmObj_genNode(char* name, int type, int credit, int energy)
{
    strcpy(smm_name[smm_nodeNr],name);
    smm_type[smm_nodeNr] = type;
    smm_credit[smm_nodeNr] = credit;
    smm_energy[smm_nodeNr] = energy;
    
    smm_nodeNr++;
    
    return (smm_nodeNr);
}



//member retrieving
char* smmObj_getName(int node_nr)
{
      return (smm_name[node_nr]);
}

int smmObj_getType(int node_nr)
{
      return (smm_type[node_nr]);
}

int smmObj_getEnergy(int node_nr)
{
      return (smm_energy[node_nr]);
}

char* smmObj_getNodeName(int node_type)
{
      return (smmNodeName[node_type]);
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



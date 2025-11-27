//
//  smm_object.h
//  SMMarble object
//
//  Created by Juyeop Kim on 2023/11/05.
//

#ifndef smm_object_h
#define smm_object_h

/* node type :
    lecture,
    restaurant,
    laboratory,
    home,
    experiment,
    foodChance,
    festival
*/

#define SMMNODE_TYPE_LECTURE                0
#define SMMNODE_TYPE_RESTAURANT             1
#define SMMNODE_TYPE_LABORATORY             2
#define SMMNODE_TYPE_HOME                   3
#define SMMNODE_TYPE_GOTOLAB                4
#define SMMNODE_TYPE_FOODCHANGE             5
#define SMMNODE_TYPE_FESTIVAL               6


/* grade :
    A+,
    A0,
    A-,
    B+,
    B0,
    B-,
    C+,
    C0,
    C-
*/



//object generation
int smmObj_genNode(char* name, int type, int credit, int energy);
char* smmObj_getNodeName(int node_nr);
int smmObj_getNodeType(int node_nr);
int smmObj_getNodeEnergy(int node_nr);
char* smmObj_getTypeName(int node_type);
int smmObj_getNodeCredit(int node_nr);
//member retrieving


//element to string



#endif /* smm_object_h */


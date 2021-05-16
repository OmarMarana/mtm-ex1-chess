#ifndef NODE_H_
#define NODE_H_

#ifndef MAP_H_
#include "map.h"
#endif

typedef struct node_t *Node;

/** Type used for returning error codes from node functions */
typedef enum NodeResult_t {
    NODE_SUCCESS = 1,
    NODE_OUT_OF_MEMORY,
    NODE_NULL_ARGUMENT
} NodeResult;

/* compare_key_func may have to be renamed due to conventions */
Node nodeCreate();
NodeResult nodeAdd(Node node_head, Node new_node, compareMapKeyElements compare_key_func);

/* Returns the node with the same key. null if not found.  */
Node nodeGetByKey(Node list, MapKeyElement key, compareMapKeyElements compare_func);

/*Destroys a linked list*/
void nodeDestroy(Node list, freeMapDataElements freeDataElement,freeMapKeyElements freeKeyElement);

/*Frees a single node in a list using the free function supplied by the user*/
void nodeFree(Node node, freeMapDataElements freeDataElement,freeMapKeyElements freeKeyElement);

/* get */
Node nodeGetNext(Node node);
MapKeyElement nodeGetKey(Node node);
MapDataElement nodeGetData(Node node);

/* set */
void nodeSetNext(Node node_from, Node node_to);
void nodeSetKey(Node node, MapKeyElement key);
void nodeSetData(Node node, MapDataElement data);

#endif /* NODE_H_ */

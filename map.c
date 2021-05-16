#include "map.h"
#include "node.h"
#include <stdlib.h>
#include <stdio.h>


#define ITERATOR_UNDEFINED NULL
static NodeResult nodeEditData(Map map, Node node_to_edit, MapDataElement data);

struct Map_t
{
    Node elements_pair;
    Node iterator;

    copyMapDataElements copyDataFucntion;
    copyMapKeyElements copyKeyFucntion;
    freeMapKeyElements freeKeyFucntion;
    freeMapDataElements freeDataFucntion;
    compareMapKeyElements compareFunction;

};

MapResult mapPut(Map map, MapKeyElement keyElement, MapDataElement dataElement)
{
    if(map == NULL || keyElement == NULL || dataElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }
    if(mapContains(map ,keyElement))
    {
        NodeResult result = nodeEditData(map, nodeGetByKey(map->elements_pair, keyElement, map->compareFunction)
                                         , dataElement);
        if(result == NODE_SUCCESS)
        {
            return MAP_SUCCESS;
        }
        return MAP_OUT_OF_MEMORY;
    }

    Node new_node = nodeCreate();
    if(new_node == NULL)
    {
        return MAP_OUT_OF_MEMORY;
    }

    MapDataElement copy_data = map->copyDataFucntion(dataElement);
    if(copy_data == NULL)
    {
        nodeFree(new_node, map->freeDataFucntion, map->freeKeyFucntion);
        return MAP_OUT_OF_MEMORY;
    }

    MapKeyElement copy_key = map->copyKeyFucntion(keyElement);
    if(copy_key == NULL)
    {
        nodeFree(new_node, map->freeDataFucntion, map->freeKeyFucntion);
        map->freeDataFucntion(copy_data);
        return MAP_OUT_OF_MEMORY;
    }

    nodeSetData(new_node, copy_data);
    nodeSetKey(new_node, copy_key);

    NodeResult add_result = nodeAdd(map->elements_pair, new_node, map->compareFunction);

    if(add_result == NODE_SUCCESS)
    {
        map->iterator = ITERATOR_UNDEFINED;
        return MAP_SUCCESS;
    }

    map->freeDataFucntion(copy_data);
    map->freeKeyFucntion(copy_key);
    nodeFree(new_node, map->freeDataFucntion, map->freeKeyFucntion);
    
    return MAP_ERROR;
}

static NodeResult nodeEditData(Map map, Node node_to_edit, MapDataElement data)
{
    MapDataElement copy_data = map->copyDataFucntion(data);
    if(copy_data == NULL)
    {
        return NODE_OUT_OF_MEMORY;
    }

    map->freeDataFucntion(nodeGetData(node_to_edit));
    nodeSetData(node_to_edit, copy_data);

    return NODE_SUCCESS;
}

MapKeyElement mapGetFirst(Map map)
{
    if(map == NULL)
    {
        return NULL;
    }

    map->iterator = (map->elements_pair);

    return mapGetNext(map);
}

MapKeyElement mapGetNext(Map map)
{
    if(map == NULL || nodeGetNext(map->iterator) == NULL ||
       map->iterator == ITERATOR_UNDEFINED)
    {
        return NULL;
    }

    map->iterator = nodeGetNext(map->iterator);
    
    MapKeyElement copy = map->copyKeyFucntion(nodeGetKey(map->iterator));
    if(copy == NULL)
    {
        return NULL;
    }

    return copy;
}

Map mapCreate(copyMapDataElements copyDataElement, copyMapKeyElements copyKeyElement,
              freeMapDataElements freeDataElement,freeMapKeyElements freeKeyElement,
              compareMapKeyElements compareKeyElements)
{
    if(copyDataElement == NULL || copyKeyElement == NULL || freeDataElement == NULL || freeKeyElement == NULL
    || compareKeyElements == NULL)
    {
        return NULL;
    }
    
    Map map_to_create = malloc(sizeof(*map_to_create));
    if(map_to_create == NULL)
    {
        return NULL;
    }
   
    map_to_create->elements_pair = nodeCreate();
    if(map_to_create->elements_pair == NULL)
    {
        mapDestroy(map_to_create);
        return NULL;
    }
   
    /*NOTE: iterator may need malloc*/

    map_to_create->copyDataFucntion = copyDataElement;
    map_to_create->copyKeyFucntion = copyKeyElement;

    map_to_create->freeDataFucntion = freeDataElement;
    map_to_create->freeKeyFucntion = freeKeyElement;

    map_to_create->compareFunction = compareKeyElements;
    
    
    return map_to_create;

}


void mapDestroy(Map map)
{
    if(map == NULL)
    {
        return;
    }

    nodeDestroy(map->elements_pair, map->freeDataFucntion, map->freeKeyFucntion);

    free(map);
}

/**************************************
 maybe should do with mapCreate to not duplicate code*/
Map mapCopy(Map map)
{  
    if(map == NULL)
    {
        return NULL;
    }

    Map map_copy = mapCreate(map->copyDataFucntion , map->copyKeyFucntion, map->freeDataFucntion,
                             map->freeKeyFucntion, map->compareFunction);

    if(map_copy == NULL)
    {
        return NULL;
    }

    Node iterator_next = nodeGetNext(map->elements_pair);
    while(iterator_next  != NULL)
    {
        MapResult map_put_result = mapPut(map_copy, nodeGetKey(iterator_next), nodeGetData(iterator_next));
        if(map_put_result != MAP_SUCCESS) 
        {
            mapDestroy(map_copy); 
            return NULL;
        }

        iterator_next = nodeGetNext(iterator_next);
    }

    map->iterator = ITERATOR_UNDEFINED;
    map_copy->iterator = ITERATOR_UNDEFINED;

    return map_copy;
}

int mapGetSize(Map map)
{
    if(map == NULL)
    {
        return -1;
    }
    
    Node iterator_next = nodeGetNext(map->elements_pair);
    
    int number_of_elements = 0;
    while(iterator_next  != NULL)
    {
        number_of_elements++;
        iterator_next = nodeGetNext(iterator_next);
    }

    return number_of_elements;
}


bool mapContains(Map map, MapKeyElement element)
{
    if(map == NULL || element == NULL)
    {
        return false;
    }

    Node iterator_next = nodeGetNext(map->elements_pair);

    while(iterator_next  != NULL)
    {
        if(map->compareFunction(nodeGetKey(iterator_next), element) == 0)
        {
            return true;
        }
        iterator_next = nodeGetNext(iterator_next);
    }
    
    return false;

}


MapDataElement mapGet(Map map, MapKeyElement keyElement)
{
    if(map == NULL || keyElement == NULL)
    {
        return NULL;
    }

    Node iterator_next = nodeGetNext(map->elements_pair);

    while(iterator_next  != NULL)
    {
        if(map->compareFunction(nodeGetKey(iterator_next), keyElement) == 0)
        {
            return nodeGetData(iterator_next);
        }
        iterator_next = nodeGetNext(iterator_next);
    }

    return NULL;

}


MapResult mapRemove(Map map, MapKeyElement keyElement)
{
    if(map == NULL || keyElement == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    Node iterator_next = nodeGetNext(map->elements_pair);
    Node iterator_previous = map->elements_pair;


    while(iterator_next != NULL)
    {
        if(map->compareFunction(nodeGetKey(iterator_next), keyElement) == 0)
        {
            nodeSetNext(iterator_previous, nodeGetNext(iterator_next));
            nodeFree(iterator_next, map->freeDataFucntion, map->freeKeyFucntion);
            return MAP_SUCCESS;
        }
        iterator_previous = nodeGetNext(iterator_previous);
        iterator_next = nodeGetNext(iterator_next);
    }

    map->iterator = ITERATOR_UNDEFINED;
    return MAP_ITEM_DOES_NOT_EXIST;
}




MapResult mapClear(Map map)
{
    if(map == NULL)
    {
        return MAP_NULL_ARGUMENT;
    }

    nodeDestroy(map->elements_pair, map->freeDataFucntion, map->freeKeyFucntion);

    return MAP_SUCCESS;

}
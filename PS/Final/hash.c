#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_SIZE 10000
#define HASHING(X) X % HASH_SIZE

int printQuery = 0;        // 1-> true, 0-> false 채점시 0
int printCurrentCache = 1; // 1-> true, 0-> false 채점시 lRUCacheGet의 리턴값을 출력함.

typedef struct list
{
    struct list *next;
    struct list *prev;
    int value, key;
} list;

typedef struct hash
{
    struct hash *next;
    int key;
    list *value;
} hash;

typedef struct LRUCache
{
    hash *table;
    int capacity;
    int count;
    list *front;
    list *back;
} LRUCache;

list *get_val(hash *table, int key)
{
    int id = HASHING(key);
    if (table[id].key == 0)
        return NULL;
    hash *curr = &table[id];
    while (curr->next != NULL)
    {
        if (curr->key == key)
            return curr->value;
        curr = curr->next;
    }
    if (curr->key == key)
        return curr->value;
    return NULL;
}

void set_val(hash *table, int key, list *value)
{
    int id = HASHING(key);
    if (table[id].key == 0)
    {
        table[id].key = key;
        table[id].value = value;
        table[id].next = NULL;
    }
    else
    {
        hash *curr = &table[id];
        while (curr->next != NULL)
        {
            curr = curr->next;
        }
        hash *new = malloc(sizeof(hash));
        new->key = key;
        new->value = value;
        new->next = NULL;
        curr->next = new;
    }
}

void rm_val(hash *table, int key)
{
    int id = HASHING(key);
    hash *curr = &table[id];
    if (curr->key == key)
    {
        if(curr->next != NULL){
        curr->key = curr->next->key;
        curr->value = curr->next->value;
        curr->next = curr->next->next;
        }
        else{
            curr->key = 0;
            curr->value = 0;
            curr->next = NULL;
        }
        return;
    }
    hash *prev = curr;
    curr = curr->next;
    while (curr->key != key)
    {
        prev = curr;
        curr = curr->next;
    }
    prev->next = curr->next;
    free(curr);
}

LRUCache *lRUCacheCreate(int capacity)
{
    LRUCache *ret = malloc(sizeof(LRUCache));
    ret->capacity = capacity;
    ret->count = 0;
    ret->table = (hash *)calloc(HASH_SIZE, sizeof(hash));
    return ret;
}

int lRUCacheGet(LRUCache *obj, int key)
{
    list *tmp = get_val(obj->table, key);
    if (tmp == NULL)
        return -1;
    else
    {
        if (tmp == obj->front)
        {
            return tmp->value;
        }
        else if (tmp == obj->back)
        {
            obj->back = tmp->prev;
            tmp->prev->next = NULL;
            tmp->prev = NULL;
            tmp->next = obj->front;
            obj->front->prev = tmp;
            obj->front = tmp;
            return tmp->value;
        }
        tmp->prev->next = tmp->next;
        tmp->next->prev = tmp->prev;
        tmp->next = obj->front;
        obj->front->prev = tmp;
        obj->front = tmp;
        tmp->prev = NULL;
        return tmp->value;
    }
}

void lRUCachePut(LRUCache *obj, int key, int value)
{
    if (get_val(obj->table, key) != NULL)
    {
        lRUCacheGet(obj, key);
        return;
    }
    list *new = (list *)malloc(sizeof(list));
    new->value = value;
    new->key = key;
    set_val(obj->table, key, new);
    if (obj->count == 0)
    {
        obj->front = new;
        obj->back = new;
        obj->count++;
    }
    else
    {
        if (obj->count < obj->capacity)
        {
            new->next = obj->front;
            obj->front->prev = new;
            obj->front = new;
            obj->count++;
        }
        else
        {
            list *tmp = obj->back;
            tmp->prev->next = NULL;
            obj->back = tmp->prev;
            rm_val(obj->table, tmp->key);
            free(tmp);
            new->next = obj->front;
            obj->front->prev = new;
            obj->front = new;
        }
    }
}

void lRUCacheFree(LRUCache *obj)
{
    hash *table;
    int capacity;
    int count;
    list *front;
    list *back;
    for (int i = 0; i < HASH_SIZE; i++)
    {
        hash *tmp = &obj->table[i];
        hash *next = tmp->next;
        while (next != NULL)
        {
            free(tmp);
            tmp = next;
            next = next->next;
        }
    }
    free(obj->table);
    list *tmp = obj->front;
    obj->front = obj->front->next;
    while (obj->front != NULL)
    {
        free(tmp);
        tmp = obj->front;
        obj->front = obj->front->next;
    }
    free(obj);
}

void GetLRUCache(int key)
{
    printf("GET (%d)\n", key);
}

void PutLRUCache(int key, int value)
{
    printf("PUT (%d,%d)\n", key, value);
}

void PrintLRUCache()
{
    printf("Print Cache\n");
}
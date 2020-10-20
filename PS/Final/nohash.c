#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define ULL_MAX 0xffffffffffffffff

int printQuery = 0;        // 1-> true, 0-> false 채점시 0
int printCurrentCache = 1; // 1-> true, 0-> false 채점시 1 IRUCacheGet의 리턴값을 출력함.
int LRU_cnt = 0;
int cap = 0;
unsigned long long timet = 0;

typedef struct LRUCache
{
    int key;
    int value;
    unsigned long long time;
} LRUCache;

int comp1(const void *a, const void *b)
{
    LRUCache *x = (LRUCache *)a;
    LRUCache *y = (LRUCache *)b;
    if (x->time > y->time)
        return 1;
    else if (x->time == y->time)
        return 0;
    else
        return -1;
}

int comp2(const void *a, const void *b)
{
    LRUCache *x = (LRUCache *)a;
    LRUCache *y = (LRUCache *)b;
    if (x->key > y->key)
        return 1;
    else if (x->key == y->key)
        return 0;
    else
        return -1;
}

void time_reset(LRUCache *obj)
{
    qsort(obj, LRU_cnt, sizeof(LRUCache), comp1);
    timet = 0;
    for (int i = 0; i < LRU_cnt; i++)
        obj[i].time = timet++;
    qsort(obj, LRU_cnt, sizeof(LRUCache), comp2);
}

LRUCache *lRUCacheCreate(int capacity)
{
    LRUCache *ret = (LRUCache *)calloc(capacity, sizeof(LRUCache));
    LRU_cnt = 0;
    cap = capacity;
    return ret;
}

int lRUCacheGet(LRUCache *obj, int key)
{
    LRUCache *target = bsearch(&key, obj, LRU_cnt, sizeof(LRUCache), comp2);
    if (target == NULL)
    {
        return -1;
    }
    else
    {
        target->time = timet;
        if (timet == ULL_MAX)
            time_reset(obj);
        else
            timet++;
        return target->value;
    }
}

void lRUCachePut(LRUCache *obj, int key, int value)
{
    LRUCache *target = bsearch(&key, obj, LRU_cnt, sizeof(LRUCache), comp2);
    if (target != NULL)
    {
        target->time = timet;
        if (timet == ULL_MAX)
            time_reset(obj);
        else
            timet++;
        return;
    }
    if (LRU_cnt < cap)
    {
        obj[LRU_cnt].key = key;
        obj[LRU_cnt].value = value;
        obj[LRU_cnt].time = timet++;
        LRU_cnt++;
        qsort(obj, LRU_cnt, sizeof(LRUCache), comp2);
    }
    else
    {
        qsort(obj, LRU_cnt, sizeof(LRUCache), comp1);
        obj[0].key = key;
        obj[0].value = value;
        obj[0].time = timet;
        if (timet == ULL_MAX)
            time_reset(obj);
        else
            timet++;
        qsort(obj, LRU_cnt, sizeof(LRUCache), comp2);
    }
}

void lRUCacheFree(LRUCache *obj)
{
    free(obj);
}
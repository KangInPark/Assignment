#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>

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

/* main
LRUCache* obj = new lRUCacheCreate(capacity);
* int result = lRUCacheGet(obj, key);
* lRUCachePutput(obj,key,value);
*/
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

int main(int argc, char **argv)
{
    if (argc != 2)
        return 0;
    int cnt = 0;
    struct timeval start, end;
    gettimeofday(&start, NULL);
    while (cnt < 1000)
    {
        char filename_out[100] = {};
        char filename_kv[100] = {};
        strncpy(filename_out, argv[1], strlen(argv[1]));
        strncpy(filename_kv, argv[1], strlen(argv[1]));
        strcat(filename_out, ".out");
        strcat(filename_kv, ".kv");

        FILE *fop = fopen(filename_out, "r");
        FILE *fkv = fopen(filename_kv, "r");
        int *keys = 0;
        int *values = 0;
        int num_pairs = -1;

        char *operation = (char *)malloc(sizeof(char) * 100);
        LRUCache *myCache;

        while (fgets(operation, 100, fop) != NULL)
        {
            char *op = strtok(operation, " ");
            int func = atoi(op);
            switch (func)
            {
            case 0:
            {
                char *c_size = strtok(NULL, " ");
                myCache = lRUCacheCreate(atoi(c_size));
                // Initialize k,v table
                fscanf(fkv, "%d", &num_pairs);
                keys = malloc(sizeof(int) * num_pairs);
                values = malloc(sizeof(int) * num_pairs);
                for (int i = 0; i < num_pairs; i++)
                {
                    fscanf(fkv, "%d %d", &keys[i], &values[i]);
                }
                break;
            }
            case 1:
            {
                char *key = strtok(NULL, " ");
                char *value = strtok(NULL, " ");

                if (printQuery)
                    PutLRUCache(atoi(key), atoi(value));

                lRUCachePut(myCache, atoi(key), atoi(value));
                break;
            }
            case 2:
            {
                char *key = strtok(NULL, " ");
                int key_as_num = atoi(key);
                int value = lRUCacheGet(myCache, key_as_num);

                if (printQuery)
                    GetLRUCache(key_as_num);

                for (int i = 0; i < num_pairs; i++)
                {
                    if (keys[i] == key_as_num)
                    {
                        // Check for correct return. Naive implementation
                        if (value != -1 && value != values[i])
                        {
                            fprintf(stderr, "WRONG ANSWER %d %d %d\n", value, atoi(key), values[i]);
                            return 0;
                        }
                    }
                }

                break;
            }
            default:
                fprintf(stderr, "ERROR: WRONG OP\n");
                return 1;
            }
        }
        fclose(fop);
        fclose(fkv);
        free(keys);
        free(values);
        lRUCacheFree(myCache);
        printf("FINISH\n");
        cnt++;
    }
    gettimeofday(&end, NULL);
    printf("%ld", (end.tv_sec - start.tv_sec)*1000000 + (end.tv_usec - start.tv_usec));
    return 0;
}

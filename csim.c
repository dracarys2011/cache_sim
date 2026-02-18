#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "cachelab.h"
#include "getopt.h"

void parse(int *s_ptr, int* S_ptr, int *E_ptr, int *b_ptr, int *B_ptr, char **trace_file_ptr, int argc, char* argv[])
{
    int opt;
    while ((opt = getopt(argc, argv, "s:E:b:t:")) !=-1)
    {
        switch(opt)
        {
            case 's':
                *s_ptr = atoi(optarg);
                *S_ptr = pow(2, *s_ptr);
                break;
            case 'E':
                *E_ptr = atoi(optarg);
                break;
            case 'b':
                *b_ptr = atoi(optarg);
                *B_ptr = pow(2, *b_ptr);
                break;
            case 't':
                *trace_file_ptr = optarg;
                break;
            default:
                printf("Error: Unrecognized program option!");
                exit(1);
        }
    }
    if(*S_ptr <= 0 || *E_ptr <= 0 || *B_ptr <= 0 || trace_file_ptr ==NULL) {
        printf("Error: Invalid program option");
        exit(1);
    }
}

typedef struct cache_line
{
    int valid_bit;
    long long tag;
    int access_time;
} cache_line_t;

typedef cache_line_t* cache_line_ptr;



cache_line_ptr* init_cache(int S, int E)
{
    cache_line_ptr* cache = malloc(sizeof(cache_line_ptr) * S);
    for(int i=0; i<S; i++){
        cache[i] = malloc(sizeof(cache_line_t) * E);
        for(int j=0; j<E; j++){
            cache[i][j].valid_bit = 0;
            cache[i][j].tag = 0;
            cache[i][j].access_time = 0;
        }
    }
    return cache;
}

FILE * openFile(char *path, char* mode)
{
    FILE * file = fopen(path, mode);
    if(!file)
    {
        printf("Error: Cannot open file %s", path);
        exit(1);
    }

    return file;
}

int if_cache_hit(int set, int E, int tag_bits, cache_line_ptr* cache, int* hit_ptr, int time)
{
    for(int line = 0; line<E; line++){
        if(cache[set][line].valid_bit && cache[set][line].tag == tag_bits){
            ++(*hit_ptr);

            cache[set][line].access_time = time;

            printf("hit ");
            return 1;
        }
    }
    return 0;
}

int if_empty_line(int set, int E, cache_line_ptr* cache, int tag_bits, int *miss_ptr, int time)
{
    for(int line=0; line<E; line++){
        if(cache[set][line].valid_bit==0){
            ++(*miss_ptr);

            cache[set][line].valid_bit=1;
            cache[set][line].tag=tag_bits;
            cache[set][line].access_time=time;

            printf("miss ");
            return 1;
        }
    }
    return 0;
}

int LRU_line_idx(int set, int E, int time, cache_line_ptr* cache)
{
    int min_time = time;
    int idx;
    for(int line=0; line<E; line++){
        if(cache[set][line].access_time < min_time){
            min_time = cache[set][line].access_time;
            idx = line;
        }
    }
    return idx;
}

void replace(int set, int E, cache_line_ptr* cache, int tag_bits, int *miss_ptr, int *evict_ptr, int time)
{
    int replaced_line_idx = LRU_line_idx(set, E, time, cache);
    cache[set][replaced_line_idx].valid_bit = 1;
    cache[set][replaced_line_idx].access_time = time;
    cache[set][replaced_line_idx].tag = tag_bits;
    ++(*miss_ptr);
    ++(*evict_ptr);
    printf("miss evict ");
}

void load_to_cache(int set, int E, int tag_bits, cache_line_ptr* cache, int* hit_ptr, int time, int *miss_ptr, int *evict_ptr)
{
    if(if_cache_hit(set, E, tag_bits, cache, hit_ptr, time)){
        return;
    }
    if(if_empty_line(set, E, cache, tag_bits, miss_ptr, time)){
        return;
    }
    replace(set, E, cache, tag_bits, miss_ptr, evict_ptr, time);
    return;
}

int main(int argc, char *argv[])
{
    int s, S, E, b, B;
    char *file_path;

    int set;
    cache_line_ptr *cache;

    FILE *file;
    char *operationString;
    char operation;
    long long address;
    int size;
    long long unsigned set_mask, set_bits, tag_bits;
    int hit, miss, evict;
    int time;

    s = S = E = b = B = 0;
    file_path = NULL;
    parse(&s, &S, &E, &b, &B, &file_path, argc, argv);
    printf("s = %d, E = %d, b = %d, file = \"%s\"\n", s, E, b, file_path);

    cache = init_cache(S, E);

    file = openFile(file_path, "r");

    operationString = malloc(sizeof(char) * 2);

    set_mask = ~((-1)<<s);
    hit = miss = evict = 0;
    time = 0;
    while(fscanf(file, "%s %llx,%d\n", operationString, &address, &size) != EOF)
    {
        ++time;
        operation = *operationString;
        set_bits = set_mask & (address >> b);
        tag_bits = address >> (b+s);
        
        set = set_bits;
        if(operation == 'L'){
            load_to_cache(set, E, tag_bits, cache, &hit, time, &miss, &evict);
        }
        else if(operation == 'S') {
            load_to_cache(set, E, tag_bits, cache, &hit, time, &miss, &evict);
        }
        else if (operation == 'M'){
            load_to_cache(set, E, tag_bits, cache, &hit, time, &miss, &evict);
            hit++;
            printf("hit ");
        }

        
    }

    printSummary(hit, miss, evict);
    free(operationString);
    for(int i=0; i<S; i++){
        free(cache[i]);
    }
    free(cache);
    fclose(file);
    return 0;
}

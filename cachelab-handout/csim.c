#include "cachelab.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>

/*
 * usage - Print usage info
 */
void usage(char *argv[]) {
    printf("Usage: %s [-hv] -s <set_bits> -E <lines> -b <block_bits> -t <file>\n", argv[0]);
    printf("Options:\n");
    printf("  -h                Print this help message.\n");
    printf("  -v                Optinal verbose flag\n");
    printf("  -s <set_bits>     Number of set index bits.\n");
    printf("  -E <lines>        Number of lines per set.\n");
    printf("  -b <block_bits>   Number of block offset bits.\n");
    printf("  -t <file>         Trace file.\n");
    printf("Example: %s -s 4 -E 1 -b 4 -t traces/yi.trace\n", argv[0]);
    printf("Example: %s -v -s 8 -E 2 -b 4 -t traces/yi.trace\n", argv[0]);
}

typedef struct _t{
    int valid;
    int tag;
    int time_stamp;
} cache_line;

int _time = 0;
int vflag;

int touch(cache_line *cache, int S, int E, int set, int tag) {
    // return value
    // 1: hit
    // 2: miss
    // 3: miss eviction

    _time++;
    // hit
//    printf("cache[1]: %d %d %d\n", cache[1].valid, cache[1].tag, cache[1].time_stamp);
    for (int j = 0; j < E; j++) {
        cache_line *tmp = &cache[set * E + j];

        if (tmp->valid && tmp->tag == tag) {
            // hit
            tmp->time_stamp = _time;
            if(vflag) {
                printf("hit ");
            }
            return 1;
        }
        
    }

    // miss
    for (int j = 0; j < E; j++) {
        cache_line *tmp = &cache[set * E + j];

        if (!tmp->valid) {
            tmp->valid = 1;
            tmp->time_stamp = _time;
            tmp->tag = tag;
            if(vflag) {
                printf("miss ");
            }
            return 2;
        }
    }

    // miss eviction
    int earlest_time = cache[set * E].time_stamp;
    int _j = 0;

    for (int j = 0; j < E; j++) {
        cache_line *tmp = &cache[set * E + j];

        if (tmp->time_stamp < earlest_time) {
            earlest_time = tmp->time_stamp;
            _j = j;
        }
    }

    cache[set * E + _j].tag = tag;
    cache[set * E + _j].time_stamp = _time;
    cache[set * E + _j].valid = 1;
    if(vflag) {
        printf("miss eviction ");
    }
    return 3;
}

int main(int argc, char** argv)
{
    int s, E, b;
    int c;
    char *trace_path;

    while ((c = getopt (argc, argv, "hvs:E:b:t:")) != -1) {
        switch (c) {
        case 'h':
            usage(argv);
            exit(0);
        case 'v':
            vflag = 1;
            break;
        case 's':
            s = atoi(optarg);
            break;
        case 'E':
            E = atoi(optarg);
            break;
        case 'b':
            b = atoi(optarg);
            break;
        case 't':
            trace_path = optarg;
            break;
        default:
            usage(argv);
            exit(1);
        }
    }
    
//    printf("Input params: %s -s %d -E %d -b %d -t %s\n", argv[0], s, E, b, trace_path);

    const int num_bits = 64;
    int tag_len = num_bits - b - s;
    int S = 1 << s;
    // int B = 1 << b;

    cache_line *cache = (cache_line*)malloc(S * E * sizeof(cache_line));

    // initilize cache

    for (int i = 0; i < S; i++) {
        for (int j = 0; j < E; j++) {
            cache_line *tmp = &cache[i * E + j];
            tmp->tag = 0;
            tmp->valid = 0;
            tmp->time_stamp = 0;
        }
    }

    FILE *trace_file = fopen(trace_path, "r");
    
    char type;
    int address = -1;
    int size = 0;


    int set_mask = ((1 << s) - 1) << b;
    int tag_mask = ((1 << tag_len) - 1) << (s + b);

    int hit_count      = 0;
    int miss_count     = 0;
    int eviction_count = 0;

    while(fscanf(trace_file, " %c %x,%d", &type, &address, &size) != EOF) {
        if (type == 'I') continue;

        int set = (address & set_mask) >> b;
        int tag = (address & tag_mask) >> (b + s);

//        printf("set: %d, tag: %d\n", set, tag);
        if (vflag) {
            printf("%c %x,%d ", type, address, size);
        }

        if (type == 'L' || type == 'S') {
            int res = touch(cache, S, E, set, tag);
            if (res == 1) {
                hit_count++;
            } else if (res == 2) {
                miss_count++;
            } else {
                eviction_count++;
                miss_count++;
            }
        } else {
            int res = touch(cache, S, E, set, tag);
            if (res == 1) {
                hit_count++;
            } else if (res == 2) {
                miss_count++;
            } else {
                eviction_count++;
                miss_count++;
            }

            res = touch(cache, S, E, set, tag);
            if (res == 1) {
                hit_count++;
            } else if (res == 2) {
                miss_count++;
            } else {
                eviction_count++;
                miss_count++;
            }
            
        }

        if (vflag) {
            printf("\n");
        }
        
        
    }

    fclose(trace_file);
    free(cache);

    printSummary(hit_count, miss_count, eviction_count);
    return 0;
}
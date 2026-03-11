#ifndef CMD_PARSER_H
#define CMD_PARSER_H

#include "string_allocator.h"
#include "messages.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#define _SKS_MAX_FLAGS 8        // 4 flags because EXEC is an internal flag

typedef enum {
    _PATHFILE  = 1 << 0,        // must always be set as first flag
    _TEST      = 1 << 1,        // 0 -> NO TEST  | 1 -> TEST
    _BENCH     = 1 << 2,        // 0 -> NO BENCH | 1 -> BENCH
    _EXEC      = 1 << 3,        // 0 -> NO CASE EXEC | 1 -> CASE EXEC
    _LIST      = 1 << 4,        // 0 -> NOLIST | 1 -> LIST
    _SUITE     = 1 << 5,        // 0 -> NO SUITE | -> SUITE
} skester_cmd_flags;

typedef struct {
    uint8_t activated_flags;
    int pathfile_offset;
    int8_t flags_order[_SKS_MAX_FLAGS];
    int* suite_names_offsets;
    int suite_max_offsets;
    int* case_names_offsets;
    int case_max_offsets;
    char** arguments_base;
} skester_pipe;

#define ADD_FLAG_ORDER(pipe, index, flag)           \
   do {                                             \
   if (index >= _SKS_MAX_FLAGS) {                   \
       SKS_MSG_ERR("[ERROR] Too many flags..\n");   \
       exit(EXIT_FAILURE);                          \
   }                                                \
   pipe.flags_order[index++] = (uint8_t)flag;       \
   } while (0)                                      \
                            
#define SKS_ADD_CASE_OFFSET(pipe, index, offset)                                            \
   if (index >= pipe.case_max_offsets) {                                                    \
       pipe.case_max_offsets *= 2;                                                          \
       void* res = realloc(pipe.case_names_offsets, sizeof(int) * pipe.case_max_offsets);   \
       if (!res) {                                                                          \
           SKS_MSG_ERR("[ERROR] Failed to reallocated on skester pipe.\n");                 \
           exit(EXIT_FAILURE);                                                              \
       }                                                                                    \
       pipe.case_names_offsets = (int *)res;                                                \
   }                                                                                        \
   pipe.case_names_offsets[index++] = offset;                                               \

#define SKS_ADD_SUITE_OFFSET(pipe, index, offset)                                           \
   if (index >= pipe.suite_max_offsets) {                                                   \
       pipe.suite_max_offsets *= 2;                                                         \
       void* res = realloc(pipe.suite_names_offsets, sizeof(int) * pipe.suite_max_offsets); \
       if (!res) {                                                                          \
           SKS_MSG_ERR("[ERROR] Failed to reallocated on skester pipe.\n");                 \
           exit(EXIT_FAILURE);                                                              \
       }                                                                                    \
       pipe.suite_names_offsets = (int *)res;                                               \
   }                                                                                        \
   pipe.suite_names_offsets[index++] = offset;                                              \


void sks_pipe_new_flag_segment(skester_pipe* pipe, 
                               int* cases_index, 
                               int* suites_index, 
                               const int flag_order_index); 

skester_pipe sks_parse_args(int argc, char** argv); 

void sks_free_pipe(skester_pipe pipe); 

#endif

#define CMD_PARSER_IMPL
#if defined (CMD_PARSER_IMPL) && !defined (CMD_PARSER_IMPLEMENTED)
#define CMD_PARSER_IMPLEMENTED

void sks_pipe_new_flag_segment(skester_pipe* pipe, 
                               int* cases_index, 
                               int* suites_index, 
                               const int flag_order_index) {
     if (flag_order_index == 0) 
         return;

     uint8_t last_flag = pipe->flags_order[flag_order_index-1];

     if (last_flag == _SUITE || last_flag == _LIST) {
         SKS_ADD_SUITE_OFFSET((*pipe), *suites_index, -1);
     } else {
         SKS_ADD_CASE_OFFSET((*pipe), *cases_index, -1);
     }
}


skester_pipe sks_parse_args(int argc, char** argv) {
    if (argc == 1) {
        SKS_MSG_ERR("[ERROR] No flags given\n");
        exit(EXIT_FAILURE);
    }
    skester_pipe pipe        = {0};

    pipe.arguments_base      = argv;
    pipe.case_max_offsets    = 12;
    pipe.suite_max_offsets   = 12;
    pipe.case_names_offsets  = (int *)malloc(sizeof(int) * pipe.case_max_offsets);
    pipe.suite_names_offsets = (int *)malloc(sizeof(int) * pipe.suite_max_offsets);

    if (!pipe.case_names_offsets || !pipe.suite_names_offsets) {
        SKS_MSG_ERR("[ERROR] Failed to allocate memory for skester pipe\n");
        exit(EXIT_FAILURE);
    }

    int idx_flag_order    = 0;
    int idx_case_offsets  = 0;
    int idx_suite_offsets = 0;

    int8_t gotpath = 0;
    for (int x = 1; x < argc; x++) {
        if (strcmp(argv[x], "-f") == 0) {
            if (x == argc - 1) {
                SKS_MSG_ERR("[ERROR] Flag '-f' expects at least an argument.\n");
                exit(EXIT_FAILURE);
            }

            x++;
            pipe.pathfile_offset = x; 
            gotpath = 1;
            pipe.activated_flags |= _PATHFILE;

            continue;
        }
        if (strcmp(argv[x], "test") == 0) {
            if (x == argc - 1) {
                SKS_MSG_ERR("[ERROR] Flag 'test' expects at least an argument.\n");
                exit(EXIT_FAILURE);
            }
            sks_pipe_new_flag_segment(&pipe, &idx_case_offsets, &idx_suite_offsets, idx_flag_order);

            pipe.activated_flags |= _TEST;
            pipe.activated_flags |= _EXEC;

            ADD_FLAG_ORDER(pipe, idx_flag_order, _TEST);

            continue;
        } 
        if (strcmp(argv[x], "bench") == 0) {
            if (x == argc - 1) {
                SKS_MSG_ERR("[ERROR] Flag 'bench' expects at least an argument.\n");
                exit(EXIT_FAILURE);
            }
            sks_pipe_new_flag_segment(&pipe, &idx_case_offsets, &idx_suite_offsets, idx_flag_order);

            pipe.activated_flags |= _BENCH;
            pipe.activated_flags |= _EXEC;
            ADD_FLAG_ORDER(pipe, idx_flag_order, _BENCH);

            continue;
        }
        if (strcmp(argv[x], "list") == 0) {
            if (x == argc - 1) {
                SKS_MSG_ERR("[ERROR] Flag 'list' expects at least an argument.\n");
                exit(EXIT_FAILURE);
            }
            sks_pipe_new_flag_segment(&pipe, &idx_case_offsets, &idx_suite_offsets, idx_flag_order);

            pipe.activated_flags |= _LIST;
            ADD_FLAG_ORDER(pipe, idx_flag_order, _LIST);

            continue;
        }
        if (strcmp(argv[x], "-s") == 0) {
            if (x == argc - 1) {
                SKS_MSG_ERR("[ERROR] Flag '-s' expects at least an argument.\n");
                exit(EXIT_FAILURE);
            }
            sks_pipe_new_flag_segment(&pipe, &idx_case_offsets, &idx_suite_offsets, idx_flag_order);

            pipe.activated_flags |= _SUITE;
            ADD_FLAG_ORDER(pipe, idx_flag_order, _SUITE);

            continue;
        }

        // if not a flag its an argument, type of argument depends on the last flag added
        if (idx_flag_order == 0) { 
            SKS_MSG_ERR("[ERROR] No type of flag set before arguments.\n Maybe you called a flag without an argument? \n");
            exit(EXIT_FAILURE);
        }

        uint8_t last_flag = pipe.flags_order[idx_flag_order-1];

        if (last_flag == _SUITE) {
            SKS_ADD_SUITE_OFFSET(pipe, idx_suite_offsets, x);
        } else {
            SKS_ADD_CASE_OFFSET(pipe, idx_case_offsets, x);
        }
    }

    if (!gotpath) {
        SKS_MSG_ERR("[ERROR] No file path given.\n");
        SKS_MSG_WARN("[TIP] Use the `-f` flag\n");
        exit(EXIT_FAILURE);
    }

    if (idx_flag_order < _SKS_MAX_FLAGS) {
        ADD_FLAG_ORDER(pipe, idx_flag_order, 0);
    }

    pipe.suite_max_offsets = idx_suite_offsets;
    pipe.case_max_offsets  = idx_case_offsets;
    return pipe;
}

void sks_free_pipe(skester_pipe pipe) {
    free(pipe.suite_names_offsets);
    free(pipe.case_names_offsets);
    pipe.case_max_offsets = 0;
    pipe.suite_max_offsets = 0;
}

#endif

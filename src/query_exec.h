#ifndef QUERY_EXEC_H
#define QUERY_EXEC_H

#include "cmd_parser.h"
#include "pipe_exec.h"
#include "structures.h"
#include <stdio.h>
#include <stdlib.h>

void sks_query_list(skester_query* query) {
    for (int x = 0; x < query->suite_count; x++) {
        size_t suite_id = query->suites[x]->id;

        for (size_t i = 0; i < sks_suite_space.sp; i++) {
            if (suite_id != sks_suite_space.suites[i].id) 
                continue;

            ascii* suite_name;
            
            SKS_S_ALLOCATOR_GET(
                    sks_string_space, 
                    sks_suite_space.suites[i].suite_name.id, 
                    suite_name
            );

            for (size_t z = 0; z < sks_suite_space.suites[i].cp; z++) {

                ascii* case_name;

                SKS_S_ALLOCATOR_GET(
                        sks_string_space, 
                        sks_suite_space.suites[i].cases[z].name.id, 
                        case_name
                        );

                SKS_MSG_OK("[SUITE] ");
                printf("%s ", suite_name);

                if (sks_suite_space.suites[i].cases[z].type == TESTER) {
                    SKS_MSG_OK("[TEST] ");
                    printf("%s \n", case_name);
                } else if (sks_suite_space.suites[i].cases[z].type == BENCHER) {
                    SKS_MSG_OK("[BENCH] ");
                    printf("%s \n", case_name);
                }
            }
        }
    }
}

void sks_query_exec_fullsuites(skester_query* query) {
    char cmd[1024]; 

    for (int x = 0; x < query->suite_count; x++) {

        ascii* suite_name;

        skester_suite* suite = query->suites[x];

        SKS_S_ALLOCATOR_GET(
                sks_string_space, 
                suite->suite_name.id, 
                suite_name
        );

        for (size_t z = 0; z < suite->cp; z++) {
            ascii* case_name;

            SKS_S_ALLOCATOR_GET(
                    sks_string_space, 
                    suite->cases[z].name.id, 
                    case_name
            );

            if (suite->cases[z].type & TESTER 
                    && (query->bitflags & _TEST) == 0)
                continue;
            else if (suite->cases[z].type & BENCHER
                    && (query->bitflags & _BENCH) == 0)
                continue;


            SKS_MSG_OK("[RUN %s]", suite->cases[z].type ? "BENCHMARK" : "TEST");
            printf(" %s/%s\n", suite_name, case_name);

            snprintf(cmd, sizeof(cmd), "%s run %s", query->pathfile, case_name);

            int res = system(cmd);

            if (res != 0)
                SKS_MSG_ERR("[FAIL] %s/%s\n\n", suite_name, case_name);
            else
                SKS_MSG_OK("[PASS] %s/%s\n\n", suite_name, case_name);
        }
    }

}

void sks_query_exec_custom(skester_query* query) {
    char cmd[1024]; 

    for (int x = 0; x < query->case_count; x++) {

        ascii* suite_name;
        ascii* case_name;

        skester_suite* suite = &sks_suite_space.suites[query->cases[x]->suite_id];

        SKS_S_ALLOCATOR_GET(
                sks_string_space, 
                suite->suite_name.id, 
                suite_name
        );

        SKS_S_ALLOCATOR_GET(
                sks_string_space, 
                query->cases[x]->name.id, 
                case_name
        );

        if (query->cases[x]->type & TESTER 
                && (query->bitflags & _TEST) == 0)
            continue;
        else if (query->cases[x]->type & BENCHER
                && (query->bitflags & _BENCH) == 0)
            continue;

        SKS_MSG_OK("[RUN %s]", query->cases[x]->type ? "BENCHMARK" : "TEST");
        printf(" %s/%s\n", suite_name, case_name);

        snprintf(cmd, sizeof(cmd), "%s run %s %s", query->pathfile, suite_name, case_name);

        int res = system(cmd);

        if (res != 0)
            SKS_MSG_ERR("[FAIL] %s/%s\n\n", suite_name, case_name);
        else
            SKS_MSG_OK("[PASS] %s/%s\n\n", suite_name, case_name);
    }
}

void sks_execute_query(skester_query* query) {
    if (query->bitflags & _LISTALL) {
        char buffer[1024];
        snprintf(buffer, sizeof(buffer), "%s listall", query->pathfile);
        system(buffer);
        return;
    }
    if (query->bitflags & _LIST) {
        sks_query_list(query);
        return;
    }

    if (query->case_count == 0) {
        sks_query_exec_fullsuites(query);
        return;
    }

    sks_query_exec_custom(query);
}
#endif

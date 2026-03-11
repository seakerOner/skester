#ifndef SKESTER_H
#define SKESTER_H

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <stdio.h>

#define SKS_CAT(a, b) SKS_CAT_EXPAND (a, b)
#define SKS_CAT_EXPAND(a, b) a##b

// API
#define SKS_UNUSED(var) (void)var;

#define TEST(suite, func_name) SKS_EXPAND_CASE(#suite, func_name, TESTER, __COUNTER__)   
#define BENCH(suite, func_name) SKS_EXPAND_CASE(#suite, func_name, BENCHER, __COUNTER__) 


// implementation
    
typedef enum {
    TESTER = 0,
    BENCHER = 1
} skester_case_type;

typedef struct {
    const char* suite;
    const char* name;
    void (*fn)(void);
    const char* file;
    const size_t line; 
    skester_case_type type;
} skester_case_extern;

#define SKS_EXPAND_CASE(suite, func_name, type, unique_id)              \
static void SKS_CAT(SKS_CAT(func_name, unique_id),_impl)(void);         \
    __attribute__((used, section("skester_cases"), aligned(sizeof(void*))))         \
    static const skester_case_extern SKS_CAT(SKS_CAT(func_name, unique_id), _metadata) = { \
        suite,                                                          \
        #func_name,                                                     \
        SKS_CAT(SKS_CAT(func_name, unique_id), _impl),                  \
        __FILE__,                                                       \
        __LINE__,                                                       \
        type,                                                           \
    };                                                                  \
    void SKS_CAT(SKS_CAT(func_name, unique_id),_impl)(void)             \

#define SKS_SETREDCOLOR "\033[31m"
#define SKS_SETGREENCOLOR "\033[32m"
#define SKS_SETYELLOWCOLOR "\033[33m"
#define SKS_RESETALLSTYLES "\033[0m"

#define SKS_MSG_ERR(msg ...)            \
    do {                                \
      fprintf(stderr, SKS_SETREDCOLOR);     \
      fprintf(stderr, msg);             \
      fprintf(stderr, SKS_RESETALLSTYLES);  \
    } while (0)                         \

#define SKS_MSG_OK(msg ...)             \
    do {                                \
      fprintf(stdout, SKS_SETGREENCOLOR);   \
      fprintf(stdout, msg);             \
      fprintf(stdout, SKS_RESETALLSTYLES);  \
    } while (0)                         \

#define SKS_MSG_WARN(msg ...)           \
    do {                                \
      fprintf(stdout, SKS_SETYELLOWCOLOR);  \
      fprintf(stdout, msg);             \
      fprintf(stdout, SKS_RESETALLSTYLES);  \
    } while (0)                         \

#endif

 //#define SKESTER_IMPL
#if defined (SKESTER_IMPL)
    extern const skester_case_extern __start_skester_cases;
    extern const skester_case_extern __stop_skester_cases;

    static void skester_listall(void) {
        const skester_case_extern *c;

        for (c = &__start_skester_cases; c < &__stop_skester_cases; c++) {
            printf("%s[Suite]%s: %s | %s[Name]%s: %s | %s[Type]%s: %s | %s[PATH]%s%s:%zu\n",
                    SKS_SETGREENCOLOR,
                    SKS_RESETALLSTYLES,
                    c->suite,
                    SKS_SETGREENCOLOR,
                    SKS_RESETALLSTYLES,
                    c->name,
                    SKS_SETGREENCOLOR,
                    SKS_RESETALLSTYLES,
                    (c->type ? "BENCH" : "TEST"),
                    SKS_SETGREENCOLOR,
                    SKS_RESETALLSTYLES,
                    c->file,
                    c->line);
        }
    }

    static void skester_dump(void) {
        const skester_case_extern *c;

        for (c = &__start_skester_cases; c < &__stop_skester_cases; c++) {
            printf("%s %s %s %zu %d \n",
                    c->suite,
                    c->name,
                    c->file,
                    c->line,
                    c->type);
        }
    }

    static void skester_run_case(int argc, char** argv) {
        int arg_p = 3;          
        const skester_case_extern* t;

        if (arg_p > argc) {
            SKS_MSG_ERR("[ERROR] 'run' expects case name as argument.\n");
            exit(EXIT_FAILURE);
        }

        const char* filter = argv[arg_p - 1];

        for (t = &__start_skester_cases; t < &__stop_skester_cases; t++) {

            if (filter && strcmp(t->name, filter) != 0)
                continue;

            t->fn();

            return;
        }

        SKS_MSG_ERR("[ERROR] Case not found.\n Case name -> %s", filter);
    }

    __attribute__((weak))
    int main(int argc, char** argv) { 
        if (argc > 1 && strcmp(argv[1], "dump") == 0) {
            skester_dump();
        } else if (argc > 1 && strcmp(argv[1], "run") == 0) {
            skester_run_case(argc, argv);
        } else if (argc > 1 && strcmp(argv[1], "listall") == 0) {
            skester_listall();
        }

        return 0; 
    }
#endif

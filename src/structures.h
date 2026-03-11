#ifndef TESTER_STRUCTURES_H
#define TESTER_STRUCTURES_H

#include "string_allocator.h"
#include "../external/mystrings.h"
#include "messages.h"

typedef enum {
    TESTER = 0,
    BENCHER = 1
} skester_case_type;

// the data structure used in ELF section 
typedef struct {
    const char* suite;
    const char* name;
    const char* file;
    const size_t line; 
    skester_case_type type;
} skester_case_extern;

sks_string_allocator sks_string_space = {0};

typedef struct {
  sks_string name;
  skester_case_type type;
  size_t suite_id;
} skester_case;

typedef struct {
  sks_string suite_name;
  size_t id;
  
  size_t cp;
  size_t capacity;

  skester_case* cases;
} skester_suite;

typedef struct {
  skester_suite* suites;
  size_t sp;
  size_t capacity;
} skester_suitelist;

skester_suitelist sks_suite_space = {0};

void sks_create_suitelist(void) {
   skester_suitelist list; 

   list.capacity = 6;
   list.sp       = 0;
   list.suites   = (skester_suite *)malloc(sizeof(skester_suite) * list.capacity);

   if (!list.suites) {
       SKS_MSG_ERR("[ERROR] Failed to create Suite List\n");
       exit(EXIT_FAILURE);
   }

   sks_suite_space = list;
}

void sks_suitelist_addsuite(ascii* name, skester_suite* out_suite) {
    sks_string suite_name;

    if (sks_s_allocator_add(&sks_string_space, name, &suite_name) == -1) {
        SKS_MSG_ERR("[ERROR] Failed to add Suite.\n Suite name: %s", name);
        exit(EXIT_FAILURE);
    }

    if (sks_suite_space.sp >= sks_suite_space.capacity) {
        sks_suite_space.capacity *= 2;

        void* ptr = realloc(sks_suite_space.suites, 
                sizeof(skester_suite) * 
                sks_suite_space.capacity);

        if (!ptr) {
            SKS_MSG_ERR("[ERROR] Failed to add Suite to Suite List.\n Suite name: %s", name);
            exit(EXIT_FAILURE);
        }
        sks_suite_space.suites = (skester_suite *)ptr;
    }

    skester_suite suite;
    suite.suite_name = suite_name;
    suite.id         = sks_suite_space.sp;
    suite.cp         = 0;
    suite.capacity   = 12;
    suite.cases      = (skester_case *)malloc(sizeof(skester_case) * suite.capacity);

    sks_suite_space.suites[sks_suite_space.sp++] = suite;
    *out_suite = suite;
}

int sks_suitelist_contains(ascii* string, skester_suite* out_suite) {
    if (sks_suite_space.sp == 0) {
      return 0;
    }

    for (size_t i = 0; i < sks_suite_space.sp; i++) {
        ascii* str_to_check;

        SKS_S_ALLOCATOR_GET(sks_string_space, 
                            sks_suite_space.suites[i].suite_name.id, 
                            str_to_check);

        if (ascii_starts_with(str_to_check, 
                              string, 
                              sks_suite_space.suites[i].suite_name.len)) 
        {
            *out_suite = sks_suite_space.suites[i];
            return 1;
        }
    }

  return 0;
}

skester_suite sks_ensure_suite(ascii* name) {
    skester_suite suite;
    if (sks_suitelist_contains(name, &suite)) {
        return suite;
    }
    // create suite if it doesnt exist
    sks_suitelist_addsuite(name, &suite);
    return suite;
}

void sks_case_add(size_t testsuite_id, ascii* name, skester_case_type type) {
  sks_string name_string = {0};
  int add_str_res = sks_s_allocator_add(&sks_string_space, name, &name_string);

  if (add_str_res == -1) {
    SKS_MSG_ERR("[ERROR] Failed to add test case.\n[TEST] %s\n", (char *)name);
    exit(EXIT_FAILURE);                
  }

  skester_case test;
  test.name = name_string;
  test.type = type;
  test.suite_id = testsuite_id;

  // add case to suite
  
  skester_suite* suite = &sks_suite_space.suites[test.suite_id];

  if (suite->cp >= suite->capacity) {
      suite->capacity *= 2;
      void* res = realloc(suite->cases, sizeof(skester_case) * suite->capacity);
      if (!res) {
          SKS_MSG_ERR("[ERROR] Failed to allocate memory for suite cases\n");
          exit(EXIT_FAILURE);
      }
      suite->cases = (skester_case *)res;
  }

  suite->cases[suite->cp++] = test;
}

void structure_dump(FILE* stream) {
    char buffer[1024];

    // case structure coming from process dump
    // c->suite,
    // c->name,
    // c->file,
    // c->line,
    // c->type);

    while (fgets(buffer, sizeof(buffer), stream) != NULL) {
        int pipe_idx = 0;
        int p = 0;
        int last_tok = 0;
        int func_name_offset = 0;

        skester_suite suite;

    next_tok:
        if (buffer[p] == '\n')
            continue;

        int start_tok = p;

        while (buffer[p] != ' ') {
            p++;
        }

        if (buffer[p] == '\0') {
            last_tok = 1;
        }
        buffer[p] = '\0';

        switch (pipe_idx) {
            case 0:     // suite
                suite = sks_ensure_suite((ascii *)&buffer[start_tok]);
                break;
            case 1:     // func_name
                func_name_offset = start_tok;
                break;
            case 2:     // file
                // no used for now
                break;
            case 3:     // line
                // no used for now
                break;
            case 4:     // type
                if (buffer[start_tok] == '1') {             
                    sks_case_add(suite.id, (ascii *)&buffer[func_name_offset], BENCHER);
                } else if (buffer[start_tok] == '0') {  
                    sks_case_add(suite.id, (ascii *)&buffer[func_name_offset], TESTER);
                } else {
                    SKS_MSG_ERR("[FATAL] UNKNOWN TYPE RECEIVED FROM EXECUTABLE WITH CASES");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                break;
        }

        if (last_tok) 
            buffer[p] = '\n';

        p++;
        pipe_idx++;
        goto next_tok;

    }
}

skester_case* sks_find_case(skester_suite* suite, char* case_name) {
    for (size_t x = 0; x < suite->cp; x++) {

        ascii* _case_name;

        SKS_S_ALLOCATOR_GET(
                sks_string_space, 
                suite->cases[x].name.id,
                _case_name
                );

        if (strcmp((char *)_case_name, case_name) == 0)
            return &suite->cases[x];
    }

    return NULL;
}

skester_suite* sks_find_suite(char* suite_name) {
    for (size_t x = 0; x < sks_suite_space.sp; x++) {
        skester_suite* suite = &sks_suite_space.suites[x];

        ascii* _suite_name;
        SKS_S_ALLOCATOR_GET(
                sks_string_space, 
                suite->suite_name.id, 
                _suite_name
        );

        if (strcmp((char *)_suite_name, suite_name) != 0)
            continue;
    }

    return NULL;
}

#endif

#if defined (RUNNER_STRUCTURES_IMPL)
#endif

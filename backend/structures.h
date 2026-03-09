#ifndef TESTER_STRUCTURES_H
#define TESTER_STRUCTURES_H

#include "../external/mystrings.h"
#include "./messages.h"
#include "./string_allocator.h"

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

void sks_testcase_add(size_t testsuite_id, ascii* name) {
  sks_string name_string = {0};
  int add_str_res = sks_s_allocator_add(&sks_string_space, name, &name_string);

  if (add_str_res == -1) {
    SKS_MSG_ERR("[ERROR] Failed to add test case.\n[TEST] %s\n", (char *)name);
    exit(EXIT_FAILURE);                
  }

  skester_case test = {
    .name     = name_string,
    .type     = TESTER,
    .suite_id = testsuite_id,
  };
  (void)test;
  // add test to testsuite
}

#endif

#if defined (RUNNER_STRUCTURES_IMPL)
#endif

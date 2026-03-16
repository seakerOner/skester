#ifndef PIPE_EXEC_H
#define PIPE_EXEC_H

#include "stdio.h"
#include "string.h"
#include <stdlib.h>

#include "messages.h"
#include "cmd_parser.h"
#include "structures.h"

typedef struct { 
    uint8_t bitflags;
    char* pathfile;

    int case_count;
    int suite_count;
    skester_case* cases[128];    
    skester_suite* suites[128];
} skester_query;

skester_query sks_process_pipe(skester_pipe* pipe) {

  if (pipe->activated_flags & _LIST && pipe->activated_flags & _EXEC) {
      SKS_MSG_ERR("[ERROR] You can't list and execute cases in the same skester pipe.\n");
      exit(EXIT_FAILURE);
  }

  char* source_path = pipe->arguments_base[pipe->pathfile_offset];

  char buffer[256];

  {
      // popen() should be sufficient for this use case where the max cases are a 
      // few thousand max, we could read directly from the elf section on the executable. 
      // But it's not as trivial as using popen() and dump stdout -> skester.
      
      snprintf(buffer, sizeof(buffer) , "%s dump" , source_path);

      fflush(stdout);
      FILE* dump = popen(buffer, "r");

      if (dump == NULL) {
          SKS_MSG_ERR("[ERROR] Failed to create new stream pipe to test subprocess. \n");
          exit(EXIT_FAILURE);
      }

      structure_dump(dump);

      pclose(dump);

      memset(buffer, 0, sizeof(buffer));
  }

  skester_query query = {0};

  query.pathfile = source_path;

  query.bitflags = pipe->activated_flags;

  int idx_suites = 0;

  for (int x = 0; x < _SKS_MAX_FLAGS; x++) {
    switch (pipe->flags_order[x]) {
      case _LIST:
      case _SUITE:
          while (idx_suites < pipe->suite_max_offsets 
                    && pipe->suite_names_offsets[idx_suites] != -1) {

              int off = pipe->suite_names_offsets[idx_suites++];
              char* suite_name     = pipe->arguments_base[off];
              skester_suite* suite = sks_find_suite(suite_name);

              if (!suite) {
                  SKS_MSG_ERR("[ERROR] Suite not found: %s\n", suite_name);
                  exit(EXIT_FAILURE);
              }
              query.suites[query.suite_count++] = suite; 
          }
          break;
      case 0:
          goto end_suite_search;
          break;
      default:
          break;
    }
  } 

end_suite_search:
  // list only works with suites, so no need to check cases
  if (pipe->activated_flags & _LIST)   
       return query;

  int idx_cases  = 0;

  for (int x = 0; x < _SKS_MAX_FLAGS; x++) {
    switch (pipe->flags_order[x]) {
      case _TEST:
      case _BENCH:
          while (idx_cases < pipe->case_max_offsets 
                    && pipe->case_names_offsets[idx_cases] != -1) {
              int off = pipe->case_names_offsets[idx_cases++];

              for (int count = 0; count < query.suite_count; count++) {

                  skester_case* res = sks_find_case(query.suites[count], (char *)pipe->arguments_base[off]);

                  if (res) {
                      query.cases[query.case_count++] = res;
                  } else {
                      SKS_MSG_ERR("[ERROR] Unknown case: %s \n", (char *)pipe->arguments_base[off]);
                      exit(EXIT_FAILURE);
                  }
              }

          }
          break;
      case _LIST:
      case _SUITE:
      case 0:
          goto exit;
          break;
      default:
          break;
    }
  }
exit:
  return query;
}


#endif

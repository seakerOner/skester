#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>

#define SKS_UNUSED(var) (void)var;


#include "../external/mystrings.h"
#include "string_allocator.h"
#define SKS_STRING_ALLOCATOR_IMPL
#define MYSTRINGS_IMPLEMENTATION
#include "structures.h"
#include "cmd_parser.h"
#define CMD_PARSER_IMPL
#include "messages.h"
#include "pipe_exec.h"
#include "query_exec.h"

int main(int argc, char **argv) {
  SKS_MSG_OK("[INFO] ");
  printf("CREATING PIPE\n");
  skester_pipe pipe = sks_parse_args(argc, argv);

  sks_create_suitelist();

  if (sks_s_allocator_init(32, &sks_string_space) == -1) {
    SKS_MSG_ERR("[ERROR] Global string_space failed.\n");
    exit(EXIT_FAILURE);                
  }

  SKS_MSG_OK("[INFO] ");
  printf("PROCESSING PIPE\n");

  skester_query query = sks_process_pipe(&pipe);

  SKS_MSG_OK("[INFO] ");
  printf("PROCESSING QUERY AND EXECUTING FLAGS\n");

  sks_execute_query(&query);

  SKS_MSG_OK("[INFO] ");
  printf("SKESTER DONE :D\n");

  sks_free_pipe(pipe);
  sks_s_allocator_destroy(sks_string_space);

  return 0;
}

#include <stdio.h>
#include <stdlib.h>

#define SKS_UNUSED(var) (void)var;


#include "./external/mystrings.h"
#include "./src/string_allocator.h"
#include "./src/pipe_exec.h"
#define CMD_PARSER_IMPL
#include "./src/cmd_parser.h"
#define MYSTRINGS_IMPLEMENTATION
#define SKS_STRING_ALLOCATOR_IMPL
#include "./src/messages.h"
#include "./src/structures.h"

int main(int argc, char **argv) {
  skester_pipe pipe = sks_parse_args(argc, argv);

  sks_create_suitelist();

  if (sks_s_allocator_init(32, &sks_string_space) == -1) {
    SKS_MSG_ERR("[ERROR] Global string_space failed.\n");
    exit(EXIT_FAILURE);                
  }

  sks_process_pipe(&pipe);

  sks_free_pipe(pipe);
  sks_s_allocator_destroy(sks_string_space);

  return 0;
}

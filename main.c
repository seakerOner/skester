#include <stdio.h>
#include <stdlib.h>

#define SKS_UNUSED(var) (void)var;

#include "./external/mystrings.h"
#include "./backend/string_allocator.h"
#include "./backend/cmd_parser.h"
#define MYSTRINGS_IMPLEMENTATION
#define SKS_STRING_ALLOCATOR_IMPL
#include "./backend/messages.h"
#include "./backend/structures.h"

int main(int argc, char **argv) {
  skester_pipe pipe = sks_parse_args(argc, argv);

  int str_space_res = sks_s_allocator_init(32, &sks_string_space);

  if (str_space_res == -1) {
    SKS_MSG_ERR("[ERROR] Global string_space failed.\n");
    exit(EXIT_FAILURE);                
  }

  sks_create_suitelist();

  SKS_MSG_OK("Hello from skester!\n");

  sks_free_pipe(pipe);
  sks_s_allocator_destroy(sks_string_space);
  return 0;
}

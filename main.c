#include <stdio.h>
#include <stdlib.h>

#define SKS_UNUSED(var) (void)var;

#include "./backend/string_allocator.h"
#define SKS_STRING_ALLOCATOR_IMPL
#include "./external/mystrings.h"
#include "./backend/messages.h"
#define MYSTRINGS_IMPLEMENTATION
#include "./backend/structures.h"

// extern ELF section with all the metadata of the tests/benchmarks

void print_args(int argc, char **argv) {
    for (int x = 0; x < argc; x++) {
        printf("%s\n", argv[x]);
    }
}

int main(int argc, char **argv) {
  print_args(argc, argv);
  sks_create_suitelist();

  SKS_MSG_OK("Hello from skester!\n");

  return 0;
}

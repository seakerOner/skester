#ifndef PIPE_EXEC_H
#define PIPE_EXEC_H

#include "stdio.h"
#include "string.h"
#include <stdlib.h>

#include "messages.h"
#include "cmd_parser.h"
#include "structures.h"

void sks_process_pipe(skester_pipe* pipe) {

  int idx_cases  = 0;
  int idx_suites = 0;

  char* source_path = pipe->arguments_base[pipe->pathfile_offset];

  FILE* dump;

  {
      char buffer[256];
      sprintf(buffer,"%s dump" , source_path);

      fflush(stdout);
      dump = popen(buffer, "r");

      if (dump == NULL) {
          SKS_MSG_ERR("[ERROR] Failed to create new stream pipe to test subprocess");
          exit(EXIT_FAILURE);
      }

      structure_dump(dump);

      pclose(dump);
  }

  for (int x = 0; x < _SKS_MAX_FLAGS; x++) {
    switch (pipe->flags_order[x]) {
      case _TEST:
          break;
      case _BENCH:
          break;
      case _LIST:
          break;
      case _SUITE:
          break;
      case 0:
          break;
      default:
          break;
    }
  } 
}


#endif

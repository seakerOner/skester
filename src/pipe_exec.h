#ifndef PIPE_EXEC_H
#define PIPE_EXEC_H

#include "cmd_parser.h"
#include "messages.h"
#include "stdio.h"
#include <stdlib.h>

void sks_process_pipe(skester_pipe* pipe) {

  printf("path id %d", pipe->pathfile_offset);

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

#ifndef SKS_MESSAGES_IMPL
#define SKS_MESSAGES_IMPL

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

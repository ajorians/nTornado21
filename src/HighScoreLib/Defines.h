#ifndef DEFINES_H
#define DEFINES_H

#define DEFAULT_LIMIT_HIGHSCORE_ITEMS   (10)

#ifndef MAX_PATH
#define MAX_PATH                        (256)
#endif

#ifndef DEBUG_MSG
#define DEBUG_MSG//     printf
#endif

#ifndef DEBUG_FUNC_NAME
#define DEBUG_FUNC_NAME	DEBUG_MSG("%s: %s (%d)\n", __FILE__, __func__, __LINE__);
#endif

#ifndef HIGHSCORE_WRITE
#define HIGHSCORE_WRITE(data, length, file_ptr) \
   if( 0 >= fwrite(data, 1, length, file_ptr) ) {\
      DEBUG_MSG("I should have wrote %d bytes in %s on line %d\n", length, __FILE__, __LINE__); \
      return HIGHSCORE_WRITE_ERROR; }
#endif

void HS_itoa(int n, char buffer[], int nBufferSize);

#endif


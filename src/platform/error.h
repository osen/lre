#define Error(M) \
  do { \
    printf("Error: %s\n", M); \
    printf("  File: %s\n", __FILE__); \
    printf("  Line: %i\n", __LINE__); \
    abort(); \
  } while(0)

#define Warning(M) \
  do { \
    printf("Warning: %s\n", M); \
    printf("  File: %s\n", __FILE__); \
    printf("  Line: %i\n", __LINE__); \
  } while(0)

#define Log(M) \
  do { \
    printf("%s\n", M); \
    printf("  File: %s\n", __FILE__); \
    printf("  Line: %i\n", __LINE__); \
  } while(0)

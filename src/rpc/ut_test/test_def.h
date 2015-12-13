#include <string>
#include <stdint.h>

#define ADDR "127.0.0.1"
#define PORT "8899"

#define MAX_BUFFER 1024
#define BODY_SIZE 16

typedef struct MetaDataStruct {
    int32_t code;
    char text[BODY_SIZE];
} MetaData;

int32_t meta_size = sizeof(MetaData);

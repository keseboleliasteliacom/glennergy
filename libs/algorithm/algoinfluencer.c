#define MODULE_NAME "ALGOINFLUENCER"
#include "../../server/log/logger.h"
#include "algoinfluencer.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#include "../API/homesystem/homesystem.h"

char* read_FileInMemory(const char* filepath, size_t* max_size) //Utils?
{
    FILE* fileptr = fopen(filepath, "rb");
    if (!fileptr) {
        log_Error("fopen() failed for file: %s", filepath); //perror
        return NULL;
    }

    struct stat fileinfo;
    if (fstat(fileno(fileptr), &fileinfo) != 0) {
        log_Error("stat() failed for %s", filepath); //perror for syscalls?
        fclose(fileptr);
        return NULL;
    }
    // if (!S_ISREG(fileinfo.st_mode)) {                      
    //     log_Error("Not a regular file: %s", filepath);      //vi vet de e json
    //     return NULL;                                       
    // }

    size_t filesize = fileinfo.st_size;

    char* json_data = malloc((filesize + 1) * sizeof(char));
    if (!json_data) {
        log_Error("malloc() failed for file: %s", filepath);
        fclose(fileptr);
        return NULL;
    }

    size_t bytes_read = fread(json_data, sizeof(char), filesize, fileptr);
    if (bytes_read != filesize) {
        LOG_ERROR("fread() %zu / %zu bytes from :%s", bytes_read, filesize, filepath);
        free(json_data);
        fclose(fileptr);
        return NULL;
    }

    json_data[filesize] = '\0';
    fclose(fileptr);

    if (max_size)
        *max_size = bytes_read;
    
    return json_data;
}


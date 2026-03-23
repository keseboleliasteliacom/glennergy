/**
 * @file Pipes.c
 * @brief Implementation of FIFO (named pipe) operations.
 * 
 * Provides APIs for creating a FIFO, reading binary data from it,
 * and writing binary data to it, with proper error handling.
 * 
 * @author YourName
 * @date 2026-03-19
 */

#include "Pipes.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>

/**
 * @brief Read binary data from a file descriptor into a buffer.
 *
 * Performs repeated reads until the requested size is fulfilled,
 * handling EINTR and EAGAIN appropriately.
 *
 * @param _Fd File descriptor to read from.
 * @param _Buf Pointer to buffer to store the data.
 * @param _Size Number of bytes to read.
 * @return Total number of bytes read, or -1 on error.
 */
ssize_t Pipes_ReadBinary(int _Fd, void *_Buf, size_t _Size)
{
    size_t total = 0;
    ssize_t bytesRead;
    char *buffer = _Buf;

    while (total < _Size)
    {
        bytesRead = read(_Fd, buffer + total, _Size - total);

        if (bytesRead > 0) {
            total += bytesRead;
            continue;
        }

        if (bytesRead == 0) {
            break; // EOF
        }

        if (bytesRead < 0) {
            if (errno == EINTR)
                continue; // Retry if interrupted by signal
        
            if (errno == EAGAIN)
            {
                return total;
            }
            perror("read");
        }
    }

    return total;
}

/**
 * @brief Write binary data from a buffer to a file descriptor.
 *
 * Performs repeated writes until the requested size is fulfilled,
 * handling EAGAIN appropriately.
 *
 * @param _Fd File descriptor to write to.
 * @param _Buf Pointer to buffer containing data to write.
 * @param _Size Number of bytes to write.
 * @return Total number of bytes written, or -1 on error.
 */
ssize_t Pipes_WriteBinary(int _Fd, void *_Buf, size_t _Size)
{
    ssize_t bytesWritten = 0;
    size_t total = 0;
    char *buffer = _Buf;

    while (total < _Size)
    {
        bytesWritten = write(_Fd, buffer + total, _Size - total);

        if (bytesWritten > 0) {
            total += bytesWritten;
        }

        if (bytesWritten < 0)
        {
            if (errno == EAGAIN) {
                return total;
            }
            perror("write");
        }
        else {
            printf("Wrote %zd bytes to the pipe\n", bytesWritten);
        }
    }

    return total;
}
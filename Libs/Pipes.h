/**
 * @file Pipes.h
 * @brief Functions for creating and reading/writing named pipes (FIFOs).
 * 
 * Provides basic APIs for creating a FIFO and performing
 * blocking/non-blocking binary read/write operations.
 * 
 * @author YourName
 * @date 2026-03-19
 */

#ifndef PIPES_H
#define PIPES_H

#include <unistd.h>

/**
 * @brief Create a FIFO (named pipe) at the specified path.
 *
 * @param _Path Path of the FIFO to create. Must not be NULL.
 * @return 0 on success, -1 on failure.
 */
int Pipes_CreateFifo(const char* _Path);

/**
 * @brief Read binary data from a file descriptor into a buffer.
 *
 * @param _Fd File descriptor to read from.
 * @param _Buf Pointer to the buffer to store data.
 * @param _Size Number of bytes to read.
 * @return Total number of bytes read, or -1 on error.
 */
ssize_t Pipes_ReadBinary(int _Fd, void* _Buf, size_t _Size);

/**
 * @brief Write binary data from a buffer to a file descriptor.
 *
 * @param _Fd File descriptor to write to.
 * @param _Buf Pointer to the buffer containing data.
 * @param _Size Number of bytes to write.
 * @return Total number of bytes written, or -1 on error.
 */
ssize_t Pipes_WriteBinary(int _Fd, void* _Buf, size_t _Size);

#endif /* PIPES_H */
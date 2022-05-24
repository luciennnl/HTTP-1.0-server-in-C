#ifndef UTIL_H
#define UTIL_H

#include <string.h>

#define ERROR_STATUS_CODE 1

/**
 * @brief Util function for http_response_to_char_array
 *        Copies a void* source into destination by a size of strlen(source) bytes
 *        Also updates offset by adding strlen(source)
 * 
 * @param offset The offset to copy the source memory into
 * @param dest The destination to copy the source memory into
 * @param src The source memory that will be copied over by strlen(src)
 */
void memcpy_offset_strlen_helper(long *offset, void *dest, void *src);

#endif
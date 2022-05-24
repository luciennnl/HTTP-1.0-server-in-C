#include "../header/util.h"
void memcpy_offset_strlen_helper(long *offset, void *dest, void *src) {
    memcpy(dest + *offset, src, strlen(src));
    *offset = *offset + strlen(src);
}
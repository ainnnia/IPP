/** @file
  Implementacja modułu do bezpiecznej alokacji pamięci.

  @author Anna Pawłowska
*/

#include <errno.h>
#include "safe_memory_allocation.h"

void* safe_malloc(size_t size) {
    void *new_ptr = malloc(size);
    if (size > 0 && new_ptr == NULL) {
        errno = ENOMEM;
    }
    return new_ptr;
}

void* safe_calloc(size_t nmemb, size_t size) {
    void *new_ptr = calloc(nmemb, size);
    if (nmemb > 0 && size > 0 && new_ptr == NULL) {
        errno = ENOMEM;
    }
    return new_ptr;
}
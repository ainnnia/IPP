/** @file
  Interfejs modułu do bezpiecznej alokacji pamięci.

  @author Anna Pawłowska
*/

#ifndef __SAFE_MEMORY_ALLOCATION_H_
#define __SAFE_MEMORY_ALLOCATION_H_

#include <stdlib.h>

/**
 * Bezpiecznie alokuje blok pamięci.
 * Kończy program w przypadku nieudanej próby alokacji.
 * @param[in] size : rozmiar potrzebnej pamięci.
 * @return wskaźnik na nowy blok pamięci.
 */
void* safe_malloc(size_t size);

/**
 * Bezpiecznie alokuje blok pamięci ustawiając jego zawartość na 0.
 * Kończy program w przypadku nieudanej próby alokacji.
 * @param[in] nmemb : liczba elementów.
 * @param[in] size : rozmiar jednego elementu.
 * @return wskaźnik na nowy blok pamięci.
 */
void* safe_calloc(size_t nmemb, size_t size);

#endif /* __SAFE_MEMORY_ALLOCATION_H__ */


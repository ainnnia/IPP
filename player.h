/** @file
 * Interfejs modułu gracza
 *
 * @author Anna Pawłowska <ap429162@students.mimuw.edu.pl>
 * @date 2023
 */

#ifndef PLAYER_H
#define PLAYER_H

#include <stdbool.h>
#include <stdint.h>

/**
 * To jest deklaracja struktury przechowującej gracza.
 */
typedef struct player_t {
    uint32_t busy_fields;
    uint32_t free_neighbours; /* Liczba wolnych pól sąsiadujących z obszarami gracza. */
    uint32_t areas;
    char symbol;
    bool neighbour_to_remove;
} player_t;

/**
 * Tworzy nowego gracza.
 */
player_t player_new(char symbol);

/**
 * Ustawia informację, że istnieje sąsiadujące z obszarami gracza pole,
 * które należy usunąć, gdyż zostało zajęte.
 */
void player_set_neighbour_to_remove(player_t* p);

/**
 * Zmniejsza liczbę wolnych pól sąsiadujących z obszarami gracza.
 */
void player_remove_neighbour(player_t* p);

/**
 * Wykonuje ruch gracza.
 */
void player_move(player_t *p, uint32_t new_neighbours, uint32_t merged_areas);

/**
 * Podaje liczbę pól, które jeszcze gracz może zająć.
 */
uint64_t player_free_fields(const player_t *p, uint32_t areas, uint64_t free_fields);

/**
 * Podaje informację czy gracz może wykonać ruch w zależności od tego,
 * czy pole które chce zająć jest polem sąsiadującym z którymś z obszarów gracza
 * i ilości obszarów dotychczas zajmowanych przez gracza.
 */
bool player_can_move(const player_t* p, uint32_t areas, uint64_t free_fields, bool neighbour);

/**
 * Aktualizuje liczbę obszarów zajmowaną przez gracza.
 */
void player_update_area(player_t *p, uint32_t merged_areas);

#endif /* PLAYER_H */

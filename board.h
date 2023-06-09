/** @file
 * Interfejs modułu planszy gry
 *
 * @author Anna Pawłowska <ap429162@students.mimuw.edu.pl>
 * @date 2023
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdint.h>

/**
 * To jest deklaracja struktury przechowującej planszę gry.
 */
typedef struct board* board_t;

/**
 * Tworzy nową planszę gry.
 */
board_t board_new(uint32_t width, uint32_t height);

/**
 * Usuwa planszę gry.
 */
void board_delete(board_t b);

/**
 * Zwraca napis przedstawiający planszę gry.
 */
char* board_draw(board_t b);

/**
 * Sprawdza czy pole gry jest puste. Zwraca true, jeśli jest lub false
 * w przeciwnym wypadku.
 */
bool board_field_free(board_t b, uint32_t x, uint32_t y);

/**
 * Zwraca numer gracza z pola o jeden wyżej.
 */
uint32_t board_up_neighbour_player(board_t b, uint32_t x, uint32_t y);

/**
 * Zwraca numer gracza z pola o jeden niżej.
 */
uint32_t board_down_neighbour_player(board_t b, uint32_t x, uint32_t y);

/**
 * Zwraca numer gracza z pola o jeden na lewo.
 */
uint32_t board_left_neighbour_player(board_t b, uint32_t x, uint32_t y);

/**
 * Zwraca numer gracza z pola o jeden na prawo.
 */
uint32_t board_right_neighbour_player(board_t b, uint32_t x, uint32_t y);

/**
 * Sprawdza, czy pole planszy sąsiaduje z jakimś polem o zadanym symbolu.
 */
bool board_has_neighbour_with_symbol(board_t b, uint32_t x, uint32_t y, char symbol);

/**
 * Zwraca ile nowych pustych sąsiednich pól ma gracz o zadanym symbolu.
 */
uint32_t board_new_free_neighbours(board_t b, uint32_t x, uint32_t y, char symbol);

/**
 * Wykonuje ruch gracza na planszy.
 */
uint32_t board_move(board_t b, uint32_t x, uint32_t y,
                    char symbol, uint32_t player);

#endif /* BOARD_H */
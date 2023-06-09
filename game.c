/** @file
 * Implementacja modułu silnika gry.
 *
 * @author Anna Pawłowska <ap429162@students.mimuw.edu.pl>
 * @date 2023
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "game.h"
#include "player.h"
#include "safe_memory_allocation.h"
#include "constants.h"

struct game {
    uint32_t width;
    uint32_t height;
    uint32_t players;
    uint32_t areas;
    uint64_t free_fields;
    board_t board;
    player_t *player;
};

/* FUNKCJE POMOCNICZE */

/**
 * Sprawdza poprawność parametrów gry. Zwraca true, jeśli są poprawne lub false
 * w przeciwnym wypadku.
 */
static bool game_parameters_correct(uint32_t width, uint32_t height,
                                    uint32_t players, uint32_t areas) {
    if (width == 0 || height == 0 || areas == 0
        || players > MAX_NUMBER_OF_PLAYERS || players == 0) {
        return false;
    }
    return true;
}

/**
 * Ustawia parametry gry.
 */
static bool game_set_parameters(game_t *g, uint32_t width, uint32_t height,
								uint32_t players, uint32_t areas) {
	assert(g != NULL);

	g->width = width;
	g->height = height;
	g->players = players;
	g->areas = areas;
	g->free_fields = (uint64_t)width * (uint64_t)height;
	g->player = safe_malloc((players + 1) * sizeof(player_t));
	if (g->player) {
		g->player[0] = player_new(EMPTY_FIELD_SYMBOL);
		for (uint32_t i = 1; i < players + 1; i++) {
			g->player[i] = player_new((i < 10) ? '0' + i : 'A' + i - 10);
		}
		g->board = board_new(width, height);
		if (g->board == NULL) {
			printf("board to null przy tworzeniu\n");
			free(g->player);
			free(g);
			return false;
		}
	}
	else {
		free(g);
		return false;
	}
	return true;
}

/**
 * Sprawdza poprawność gracza. Zwraca true, jeśli jest poprawny lub false
 * w przeciwnym wypadku.
 */
static bool game_player_correct(game_t const *g, uint32_t player) {
    if (g == NULL || player > g->players || player == 0) {
        return false;
    }
    return true;
}

/**
 * Sprawdza poprawność pola gry. Zwraca true, jeśli jest poprawne lub false
 * w przeciwnym wypadku.
 */
static bool game_field_correct(game_t const *g, uint32_t x, uint32_t y) {
    if (g == NULL || x >= g->width || y >= g->height
		|| !board_field_free(g->board, x, y)) {
        return false;
    }
    return true;
}

/**
 * Aktualizuje liczbę sąsiednich wolnych pól, dla pól będących sąsiadami pola
 * o współrzędnych (x, y).
 */
static void game_update_neighbours(game_t *g, uint32_t x, uint32_t y) {
	assert(game_field_correct(g, x, y));

	uint32_t player_right = board_right_neighbour_player(g->board, x, y);
	uint32_t player_left = board_left_neighbour_player(g->board, x, y);
	uint32_t player_up = board_up_neighbour_player(g->board, x, y);
	uint32_t player_down = board_down_neighbour_player(g->board, x, y);

	player_set_neighbour_to_remove(&g->player[player_right]);
	player_set_neighbour_to_remove(&g->player[player_left]);
	player_set_neighbour_to_remove(&g->player[player_up]);
	player_set_neighbour_to_remove(&g->player[player_down]);

	player_remove_neighbour(&g->player[player_right]);
	player_remove_neighbour(&g->player[player_left]);
	player_remove_neighbour(&g->player[player_up]);
	player_remove_neighbour(&g->player[player_down]);
}

/**
 * Wykonuje ruch gracza.
 */
static void game_make_move(game_t *g, uint32_t player, uint32_t x, uint32_t y) {
	assert(game_player_correct(g, player));
	assert(game_field_correct(g, x, y));
	assert(g->free_fields > 0);

	g->free_fields--;
	game_update_neighbours(g, x, y);

	char symbol = game_player(g, player);
	uint32_t new_neighbours = board_new_free_neighbours(g->board, x, y, symbol);
	uint32_t merged_areas = board_move(g->board, x, y, symbol, player);
	player_move(&g->player[player], new_neighbours, merged_areas);
}

/* FUNKCJE MODUŁU GRY */

game_t* game_new(uint32_t width, uint32_t height,
                 uint32_t players, uint32_t areas) {
    if (!game_parameters_correct(width, height, players, areas)) {
        return NULL;
    }

    game_t *g = safe_malloc(sizeof(struct game));
	if (g != NULL) {
		if(!game_set_parameters(g, width, height, players, areas)) {
			return NULL;
		}
	}
    return g;
}

void game_delete(game_t *g) {
    if (g != NULL) {
        board_delete(g->board);
		if (g->player != NULL) {
			free(g->player);
		}
        free(g);
    }
}

bool game_move(game_t *g, uint32_t player, uint32_t x, uint32_t y) {
	if (!game_player_correct(g, player)
		|| game_free_fields(g, player) == 0
		|| !game_field_correct(g, x, y)
		|| !board_field_free(g->board, x, y)) {
		return false;
	}
	printf("jestem w game_move\n");
	char symbol = game_player(g, player);
	bool neighbour = board_has_neighbour_with_symbol(g->board, x, y, symbol);
	if (!player_can_move(&g->player[player], g->areas,
						 g->free_fields, neighbour)) {
		return false;
	}
	game_make_move(g, player, x, y);
	return true;
}

uint64_t game_busy_fields(game_t const *g, uint32_t player) {
    if (!game_player_correct(g, player)) {
        return 0;
    }
    return g->player[player].busy_fields;
}

uint64_t game_free_fields(game_t const *g, uint32_t player) {
    if (!game_player_correct(g, player)) {
        return 0;
    }
    return player_free_fields(&g->player[player], g->areas, g->free_fields);
}


uint32_t game_board_width(game_t const *g) {
    if (g == NULL) {
        return 0;
    }
    return g->width;
}

uint32_t game_board_height(game_t const *g) {
    if (g == NULL) {
        return 0;
    }
    return g->height;
}

char game_player(game_t const *g, uint32_t player) {
    if (!game_player_correct(g, player)) {
        return EMPTY_FIELD_SYMBOL;
    }
    return g->player[player].symbol;
}

char* game_board(game_t const *g) {
    if (g == NULL) {
        return NULL;
    }
    return board_draw(g->board);
}

uint32_t game_players(game_t const *g) {
    if (g == NULL) {
        return 0;
    }
    return g->players;
}

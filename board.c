/** @file
 * Implementacja modułu planszy gry
 *
 * @author Anna Pawłowska <ap429162@students.mimuw.edu.pl>
 * @date 2023
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "board.h"
#include "safe_memory_allocation.h"
#include "constants.h"

/**
 * To jest struktura przechowująca pole planszy.
 */
typedef struct field {
    uint32_t player;
    char player_symbol;
    uint64_t color;
} field;

struct board {
    uint32_t width;
    uint32_t height;
    uint64_t new_color;
    field** fields;
    uint64_t* colors;
};

/**
 * To jest struktura przechowująca parę współrzędnych.
 */
typedef struct coordinates_t {
    int64_t x;
    int64_t y;
} coordinates_t;

/* FUNKCJE POMOCNICZE */

/**
 * Zwalnia pamięć zajmowaną przez pola planszy.
 */
static void board_free_fields(field** fields, uint32_t width) {
	for (uint32_t i = 0; i < width; i++) {
		free(fields[i]);
	}
	free(fields);
}

/**
 * Tworzy tablicę pól planszy.
 */
static field** board_new_fields(uint32_t width, uint32_t height) {
	field** fields = safe_calloc(width, sizeof(field*));
	if (fields != NULL) {
		for (uint32_t i = 0; i < width; i++) {
			fields[i] = safe_calloc(height, sizeof(field));
			if (fields[i] == NULL) {
				board_free_fields(fields, i);
				return NULL;
			}
		}
	}
	return fields;
}

/**
 * Ustawia parametry planszy.
 */
static void board_set_parameters(board_t b, uint32_t width, uint32_t height) {
	assert(b != NULL);

	b->width = width;
	b->height = height;
	b->new_color = 0;
	b->colors = safe_malloc(((uint64_t)width * (uint64_t)height + 1) * sizeof(uint64_t));
	if (b->colors != NULL) {
		b->fields = board_new_fields(width, height);
		if (b->fields == NULL) {
			free(b->colors);
		}
	}
}

/**
 * Sprawdza, czy pole należy do planszy (pole graczy + brzegi).
 */
bool board_field_correct(board_t b, uint32_t x, uint32_t y) {
    if (b == NULL) {
        printf("board_field_correct: b == NULL\n");
        return false;
    }
    if (x >= b->width || y >= b->height) {
        printf("board_field_correct: x >= b->width || y >= b->height\n");
        return false;
    }
    return true;
}

/* COORDINATES FUNCTIONS */

/**
 * Sprawdza, czy współrzędne są poprawne.
 */
static bool coordinates_correct(board_t b, coordinates_t c) {
    return b != NULL
           && c.x < b->width && c.y < b->height && c.x >= 0 && c.y >= 0;
}

/**
 * Zwraca współrzędne pola (x, y)
 */
static coordinates_t coordinates(uint32_t x, uint32_t y) {
    return (coordinates_t) {.x = x, .y = y};
}

/**
 * Sprawdza, czy współrzędne pola o jeden w górę.
 */
static coordinates_t up_coordinates(coordinates_t c) {
    c.y++;
    return c;
}

/**
 * Sprawdza, czy współrzędne pola o jeden w dół.
 */
static coordinates_t down_coordinates(coordinates_t c) {
    c.y--;
    return c;
}

/**
 * Sprawdza, czy współrzędne pola o jeden na lewo.
 */
static coordinates_t left_coordinates(coordinates_t c) {
    c.x--;
    return c;
}

/**
 * Sprawdza, czy współrzędne pola o jeden na prawo.
 */
static coordinates_t right_coordinates(coordinates_t c) {
    c.x++;
    return c;
}

/**
 * Sprawdza, czy współrzędne pola sąsiedniego w zadanym kierunku.
 */
static coordinates_t neighbour_coordinates(coordinates_t c, uint32_t direction) {
    switch (direction) {
        case 0:
            return up_coordinates(c);
        case 1:
            return right_coordinates(c);
        case 2:
            return down_coordinates(c);
        case 3:
            return left_coordinates(c);
        default:
            assert(false);
            return c;
    }
}

/**
 * Zwraca symbol pola o zadanych współrzędnych.
 */
static char coordinates_symbol(board_t b, coordinates_t c) {
    if (!coordinates_correct(b, c)) {
        return NONEXISTENT_FIELD_SYMBOL;
    }
    if (b->fields[c.x][c.y].player == NO_PLAYER) {
        return EMPTY_FIELD_SYMBOL;
    }
    return b->fields[c.x][c.y].player_symbol;
}

/**
 * Zwraca numer gracza pola o zadanych współrzędnych.
 */
static uint32_t coordinates_player(board_t b, coordinates_t c) {
    return coordinates_correct(b, c) ? b->fields[c.x][c.y].player : NO_PLAYER;
}

/**
 * Zwraca kolor pola o zadanych współrzędnych.
 */
static uint64_t coordinates_color(board_t b, coordinates_t c) {
    return coordinates_correct(b, c) ? b->fields[c.x][c.y].color : NO_COLOR;
}

/**
 * Sprawdza czy pola są tego samego gracza.
 */
static bool are_same_player(board_t b, coordinates_t c1, coordinates_t c2) {
    return coordinates_player(b, c1) == coordinates_player(b, c2);
}

/**
 * Sprawdza czy pola są tego samego symbolu.
 */
static bool are_same_symbol(board_t b, coordinates_t c1, coordinates_t c2) {
    return coordinates_symbol(b, c1) == coordinates_symbol(b, c2);
}

/**
 * Sprawdza czy pola są sąsiednie.
 */
static bool are_neighbours(coordinates_t c1, coordinates_t c2) {
    return (c1.x == c2.x && labs(c1.y - c2.y) == 1)
           || (c1.y == c2.y && labs(c1.x - c2.x) == 1);
}

/**
 * Sprawdza czy pole jest wolne.
 */
static bool coordinates_free(board_t b, coordinates_t c) {
    return coordinates_symbol(b, c) == EMPTY_FIELD_SYMBOL;
}

/**
 * Sprawdza czy pole ma sąsiada o zadanym symbolu.
 */
static bool has_neighbour_with_symbol(board_t b, coordinates_t c, char symbol) {
    if (!coordinates_correct(b, c)) {
        return false;
    }
    for (uint32_t i = 0; i < DIRECTIONS; i++) {
        coordinates_t neighbour = neighbour_coordinates(c, i);
        if (coordinates_symbol(b, neighbour) == symbol) {
            return true;
        }
    }
    return false;
}

/* END OF COORDINATES FUNCTIONS */

static uint32_t board_get_player(board_t b, uint32_t x, uint32_t y) {
    return coordinates_player(b, coordinates(x, y));
}

static char board_get_symbol(board_t b, uint32_t x, uint32_t y) {
    return coordinates_symbol(b, coordinates(x, y));
}

static void board_set_symbol(board_t b, uint32_t x, uint32_t y, char symbol) {
    assert(board_field_free(b, x, y));
    b->fields[x][y].player_symbol = symbol;
}

static void board_set_player(board_t b, uint32_t x, uint32_t y, uint32_t player) {
    assert(board_field_free(b, x, y));
    b->fields[x][y].player = player;
}

static void board_set_color(board_t b, uint32_t x, uint32_t y, uint32_t color) {
    assert(board_field_correct(b, x, y));
    b->fields[x][y].color = color;
}

static void board_make_move(board_t b, uint32_t x, uint32_t y, char symbol,
							uint32_t player) {
    assert(board_field_free(b, x, y));
    board_set_symbol(b, x, y, symbol);
    board_set_player(b, x, y, player);

    b->new_color++;
    board_set_color(b, x, y, b->new_color);
    b->colors[b->new_color] = b->new_color;
}

static void set_coordinates_color(board_t b, coordinates_t c, uint64_t color) {
    board_set_color(b, c.x, c.y, color);
}

static void add_field_to_area(board_t b, coordinates_t c_field,
							  coordinates_t c_area) {
    assert(are_same_symbol(b, c_field, c_area));
    assert(are_same_player(b, c_field, c_area));
    uint64_t color = coordinates_color(b, c_area);
    set_coordinates_color(b, c_field, color);
}

static bool color_correct(board_t b, uint64_t color) {
    return b != NULL && color < b->new_color && color > 0;
}

static uint64_t find_true_color(board_t b, uint64_t color) {
    assert(color_correct(b, color));
    assert(color < b->new_color);
    if (b->colors[color] != color) {
        b->colors[color] = find_true_color(b, b->colors[color]);
    }
    return b->colors[color];
}

static void union_true_colors(board_t b, uint64_t c1, uint64_t c2) {
    assert(c1 != c2);
    b->colors[c1] = c2;
}

static bool recolor(board_t b, coordinates_t c1, coordinates_t c2) {
    assert(are_same_symbol(b, c1, c2));
    assert(are_same_player(b, c1, c2));
    assert(are_neighbours(c1, c2));

    uint64_t c1_true_color = find_true_color(b, coordinates_color(b, c1));
    uint64_t c2_true_color = find_true_color(b, coordinates_color(b, c2));
    if (c1_true_color == c2_true_color) {
        return false;
    }
    union_true_colors(b, c1_true_color, c2_true_color);
    return true;
}

static bool is_coordinates_color_new(board_t b, coordinates_t c) {
    assert(coordinates_correct(b, c));
    return coordinates_color(b, c) == b->new_color;
}

static uint32_t merge_areas(board_t b, coordinates_t c1, coordinates_t c2) {
    assert(are_same_symbol(b, c1, c2));
    assert(are_same_player(b, c1, c2));
    assert(are_neighbours(c1, c2));
    assert(coordinates_player(b, c1) == coordinates_player(b, c2));

    if (is_coordinates_color_new(b, c1) || recolor(b, c1, c2)) {
        add_field_to_area(b, c1, c2);
        return 1;
    }
    return 0;
}

/* FUNKCJE MODUŁU */

board_t board_new(uint32_t width, uint32_t height) {
	board_t b = safe_malloc(sizeof(struct board));
	if (b) {
		board_set_parameters(b, width, height);
	}
	if (b->colors == NULL || b->fields == NULL) {
		free(b);
		return NULL;
	}
	return b;
}

void board_delete(board_t b) {
    if (b != NULL) {
        if (b->fields != NULL) {
			board_free_fields(b->fields, b->width);
        }
        if (b->colors != NULL) {
            free(b->colors);
        }
        free(b);
    }
}

char* board_draw(board_t b) {
    if (b == NULL) {
        return NULL;
    }
    char *b_d = safe_calloc((uint64_t)(b->width + 1) * (uint64_t)b->height + 1,
							sizeof(char));

    for (uint64_t i = 0; i < b->height; i++) {
        for (uint64_t x = 0; x < b->width; x++) {
            uint64_t y = (uint64_t)b->height - i -1;
            b_d[(uint64_t)(b->width + 1) * i + x] = board_get_symbol(b, x, y);
        }
        char endl = '\n';
        b_d[i * (uint64_t)(b->width + 1) + (uint64_t)b->width] = endl;
    }
    return b_d;
}

bool board_field_free(board_t b, uint32_t x, uint32_t y) {
    assert(board_field_correct(b, x, y));
    return board_get_player(b, x, y) == NO_PLAYER
           && board_get_symbol(b, x, y) == EMPTY_FIELD_SYMBOL;
}

uint32_t board_up_neighbour_player(board_t b, uint32_t x, uint32_t y) {
    coordinates_t c = coordinates(x, y);
    return coordinates_player(b, up_coordinates(c));
}

uint32_t board_down_neighbour_player(board_t b, uint32_t x, uint32_t y) {
    coordinates_t c = coordinates(x, y);
    return coordinates_player(b, down_coordinates(c));
}

uint32_t board_left_neighbour_player(board_t b, uint32_t x, uint32_t y) {
    coordinates_t c = coordinates(x, y);
    return coordinates_player(b, left_coordinates(c));
}

uint32_t board_right_neighbour_player(board_t b, uint32_t x, uint32_t y) {
    coordinates_t c = coordinates(x, y);
    return coordinates_player(b, right_coordinates(c));
}

bool board_has_neighbour_with_symbol(board_t b, uint32_t x, uint32_t y,
									 char symbol) {
    assert(board_field_correct(b, x, y));
    return has_neighbour_with_symbol(b, coordinates(x, y), symbol);
}

uint32_t board_new_free_neighbours(board_t b, uint32_t x, uint32_t y,
								   char symbol) {
    assert(board_field_free(b, x, y));

    uint32_t new_neighbours = 0;
    coordinates_t c = coordinates(x, y);
    for (uint32_t i = 0; i < DIRECTIONS; i++) {
        coordinates_t neighbour = neighbour_coordinates(c, i);
        if (coordinates_free(b, neighbour)
            && !has_neighbour_with_symbol(b, neighbour, symbol)) {
            new_neighbours++;
        }
    }
    return new_neighbours;
}

uint32_t board_move(board_t b, uint32_t x, uint32_t y,
                    char symbol, uint32_t player) {
    board_make_move(b, x, y, symbol, player);

    uint32_t merged_areas = 0;
    coordinates_t c = coordinates(x, y);
    for (uint32_t i = 0; i < DIRECTIONS; i++) {
        coordinates_t neighbour = neighbour_coordinates(c, i);
        if (are_same_symbol(b, c, neighbour)) {
            merged_areas += merge_areas(b, c, neighbour);
        }
    }
    return merged_areas;
}
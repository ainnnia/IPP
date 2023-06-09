/** @file
 * Uruchamianie programu i parsowanie argamentów.
 *
 * @author Anna Pawłowska <ap429162@students.mimuw.edu.pl>
 * @date 2023
 */

#include <errno.h>
#include <stdio.h>
#include "constants.h"
#include "game.h"
#include "interactive_mode.h"
#include "safe_memory_allocation.h"

typedef struct game game_t;

/**
 * Konwertuje napis na liczbę 32 bitową.
 */
static uint32_t parse_uint32(const char* str) {
    char *endptr;
    errno = 0;
    uint32_t number = strtoul(str, &endptr, 10);
    if (errno != 0 || *endptr != '\0') {
        return 0;
    }
    return number;
}

int main(int argc, char *argv[]) {
    if (argc == 5) {
        uint32_t w = parse_uint32(argv[1]);
        if (w == 0) {
            fprintf(stderr, "Niepoprawna szerokość.\n");
            return WRONG_INPUT;
        }
        uint32_t h = parse_uint32(argv[2]);
        if (h == 0) {
            fprintf(stderr, "Niepoprawna wysokość.\n");
            return WRONG_INPUT;
        }
        uint32_t players = parse_uint32(argv[3]);
        if (players == 0 || players > MAX_NUMBER_OF_PLAYERS) {
            fprintf(stderr, "Niepoprawna liczba graczy.\n");
            return WRONG_INPUT;
        }
        uint32_t areas = parse_uint32(argv[4]);
        if (areas == 0) {
            fprintf(stderr, "Niepoprawna liczba obszarów.\n");
            return WRONG_INPUT;
        }

        game_t *g = game_new(w, h, players, areas);
        if (g == NULL) {
            fprintf(stderr, "Nie udało się zaalokować pamiąci.\n");
            return MEMORY_ERROR;
        }
        run_interactive(g);
        return 0;
    }
    fprintf(stderr, "Użycie:\n%s width height players areas\n", argv[0]);
    return WRONG_INPUT;
}
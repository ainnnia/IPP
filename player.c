/** @file
 * Implementacja modułu gracza
 *
 * @author Anna Pawłowska <ap429162@students.mimuw.edu.pl>
 * @date 2023
 */

#include <assert.h>
#include <stdio.h>
#include "player.h"
#include "constants.h"

player_t player_new(char symbol) {
    return (player_t) {.symbol = symbol, .areas = 0, .busy_fields = 0, .free_neighbours = 0};
}

void player_set_neighbour_to_remove(player_t *p) {
    assert(p != NULL);
    if (p->symbol == EMPTY_FIELD_SYMBOL) {
        return;
    }
    p->neighbour_to_remove = true;
}

void player_remove_neighbour(player_t *p) {
    assert(p != NULL);
    if (p->symbol != EMPTY_FIELD_SYMBOL && p->neighbour_to_remove) {
        p->neighbour_to_remove = false;
        assert(p->free_neighbours > 0);
        p->free_neighbours--;
    }
}

void player_move(player_t *p, uint32_t new_neighbours, uint32_t merged_areas) {
    assert(p != NULL);
    p->busy_fields++;
    p->free_neighbours += new_neighbours;
    player_update_area(p, merged_areas);
}

uint64_t player_free_fields(const player_t *p, uint32_t areas, uint64_t free_fields) {
    assert(p != NULL);
    if (p->areas < areas) {
        return free_fields;
    }
    else {
        return p->free_neighbours;
    }
}

bool player_can_move(const player_t* p, uint32_t areas, uint64_t free_fields, bool neighbour) {
    assert(p != NULL);
    assert(p->symbol != EMPTY_FIELD_SYMBOL);
    if (p->areas < areas && free_fields > 0) {
        return true;
    }
    return p->free_neighbours > 0 && neighbour;
}

void player_update_area(player_t *p, uint32_t merged_areas) {
    assert(p != NULL);
    p->areas++;
    assert(p->areas >= merged_areas);
    p->areas -= merged_areas;
}

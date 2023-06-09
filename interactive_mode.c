/** @file
 * Implementacja interaktywnego trybu tekstowego gry
 *
 * @author Anna Pawłowska <ap429162@students.mimuw.edu.pl>
 * @date 2023
 */

#include <errno.h>
#include <stdint.h>
#include <unistd.h>
#include <stdio.h>
#include <termios.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/ioctl.h>
#include "game.h"
#include "interactive_mode.h"
#include "constants.h"

/**
* Czyści terminal.
*/
static void clear() {
    printf("\x1b[H\x1b[J");
}

/**
 * Wypisuje komórkę planszy.
 */
static void print_board_cell(char* board, uint64_t* i, int64_t current_player) {
    uint32_t player;
    if (board[*i] == '.') {
        player = 0;
    }
    else if (board[*i] >= '1' && board[*i] <= '9') {
        player = board[*i] - '0';
    }
    else if (board[*i] >= 'A' && board[*i] <= 'Z') {
        player = board[*i] - 'A' + 10;
    }
    else {
        player = -1;
    }

    if (current_player != -1 && player == current_player) {
        printf("\x1b[38;2;%sm", DARK_ORCHID);
    }
    else if (player == 0) {
        printf("\x1b[38;2;%sm", MINT_GREEN);
    }
    printf("%c", board[*i]);
    printf("\x1b[0m");
}

/**
 * Wypisuje planszę.
 */
static void print_board(game_t *g, uint32_t cursor_x, uint32_t cursor_y, int64_t current_player) {
    clear();
    char* board = game_board(g);
    if (!board) {
        game_delete(g);
        fprintf(stderr, "Nie udało się wczytać planszy.\n");
        exit(1);
    }
    uint64_t i = 0;
    uint32_t current_x_pos = 0;
    uint32_t current_y_pos = 0;
    while (board[i] != '\0') {
        //nowy wiersz
        if (board[i] == '\n') {
            current_y_pos++;
            current_x_pos = 0;
            i++;
            printf("\n");
            continue;
        }
        //kolorowanie kursora
        if (cursor_x == current_x_pos && cursor_y == current_y_pos) {
            printf("\x1b[48;2;%sm", DARK_ORCHID);
            printf("\x1b[30m");
        }
        print_board_cell(board, &i, current_player);
        printf("\x1b[0m");
        i++;
        current_x_pos++;
    }
    free(board);
}

/**
 * Wypisuje statystyki gry.
 */
static void print_stats(game_t *g, uint32_t current_player, bool move_failed) {
    printf("\x1b[38;2;%sm", DARK_ORCHID);
    printf("Wykonaj ruch! Gracz: %d\nSymbol: %c\n", current_player,
           game_player(g, current_player));
    printf("Dostępne pola: %lu\n",
           game_free_fields(g, current_player));
    printf("\x1b[0m");
    if (move_failed) {
        printf("\x1b[38;2;%sm", DARK_WASHED_BLUE);
        printf("Niemożliwy do wykonania ruch.\n");
        printf("\x1b[0m");
    }
}

/** 
* Wypisuje wynik gry.
*/
static inline void print_game_results(game_t *g) {
    clear();
    printf("Finalny stan gry:\n\n");
    print_board(g, 0, 0, -1);
    printf("\nWynik:\n");
    for (uint32_t i = 1; i <= game_players(g); i++) {
        printf("Gracz %u: %lu \n", i, game_busy_fields(g, i));
    }
}

/**
 * Sprawdza, czy znak jest strzałką.
 */
static bool arrow_input(char* ch) {
    *ch = (char) getchar();
    if (*ch != '[') {
        return false;
    }
    *ch = (char) getchar();
    if (*ch == 'A' || *ch == 'B' || *ch == 'C' || *ch == 'D') {
        return true;
    } else {
        return false;
    }

}

/**
 * Obsługuje ruchy kursora.
 */
static bool handle_cursor_movement(char ch, uint32_t* cursor_x,
                                   uint32_t* cursor_y, game_t *g) {
    uint32_t x = *cursor_x;
    uint32_t y = *cursor_y;
    switch (ch) {
        case 'A':
            if (y == 0)
                return true;
            y--;
            break;
        case 'B':
            if (y == game_board_height(g) - 1)
                return true;
            y++;
            break;
        case 'C':
            if (x == game_board_width(g) - 1)
                return true;
            x++;
            break;
        case 'D':
            if (x == 0)
                return true;
            x--;
            break;
        default:
            assert(false);
            return false;
    }
    *cursor_x = x;
    *cursor_y = y;
    return true;
}

/**
* Wykonuje ruch gracza. Zwraca true, jeśli ruch był możliwy do wykonania,
* false w przeciwnym wypadku.
*/
static bool handle_move(char ch, uint32_t player, uint32_t x,
                        uint32_t y, game_t* game) {
    if (ch == ' ') {
        return game_move(game, player, x, y);
    }
    return true;
}

/* FUNKCJE TERMINALA */

static bool correct_terminal(game_t* g) {
    struct winsize terminal;
    ioctl(0, TIOCGWINSZ, &terminal);
    if (terminal.ws_col < game_board_width(g) 
        || terminal.ws_row < game_board_height(g) + game_players(g) + 2
        || terminal.ws_row < game_board_height(g) + STATS_ROWS + 2) {
        return false;
    }
    return true;
}

static void setup_terminal(struct termios* terminal, game_t* g) {
    tcgetattr(STDIN_FILENO, terminal);
    struct termios term = *terminal;
    term.c_lflag &= ~(ECHO | ICANON);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &term)) {
        game_delete(g);
        fprintf(stderr, "Błąd terminala.\n");
        exit(1);
    }
    printf("\x1b[?25l");
    clear();
}

static void reset_terminal(struct termios* terminal, game_t* g) {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, terminal)) {
        game_delete(g);
        fprintf(stderr, "Błąd terminala.\n");
        exit(1);
    }
    printf("\x1b[?25h");
    fflush(stdout);
}

/**
 * Zwraca następnego gracza, który może wykonać ruch. Jeśli nie ma takiego gracza zwraca -1.
 */
static int next_player(int64_t current_player, game_t* game, uint32_t number_of_players) {
    uint32_t tmp_player = current_player;
    for (uint32_t i = 0; i < number_of_players + 1; i++) {
        tmp_player = ((tmp_player + i) % number_of_players) + 1;
        if (game_free_fields(game, tmp_player)) {
            return tmp_player;
        }
    }
    return -1;
}

/**
 * Zwraca false jeśli gra powinna się zakończyć.
 */
static bool interactive_loop_step(game_t *g, char ch, uint32_t* cursor_x,
                                  uint32_t* cursor_y, int64_t* player,
                                  bool* skip_read) {
    print_board(g, *cursor_x, *cursor_y, *player);
    print_stats(g, *player, false);
    bool move_failed = false;

    if (ch == ' ' || ch == 'c' || ch == 'C') {
        if (handle_move(ch, *player, *cursor_x, game_board_height(g) - *cursor_y - 1, g)) {
            *player = next_player(*player, g, game_players(g));
            if (*player == -1) {
                return false;
            }
        } else {
            move_failed = true;
        }
    } else if (ch == 27) {
        if (arrow_input(&ch)) {
            handle_cursor_movement(ch, cursor_x, cursor_y, g);
        } else {
            *skip_read = true;
        }
    }
    print_board(g, *cursor_x, *cursor_y, *player);
    print_stats(g, *player, move_failed);
    return true;
}

void run_interactive(game_t *g) {
    if (!correct_terminal(g)) {
        game_delete(g);
        fprintf(stderr, "Za mały terminal.\n");
        exit(1);
    }

    struct termios terminal;
    setup_terminal(&terminal, g);

    uint32_t cursor_x = 0;
    uint32_t cursor_y = 0;
    int64_t player = 1;

    bool skip_read = false;
    clear();
    print_board(g, cursor_x, cursor_y, player);
    print_stats(g, player, false);

    char ch = 1;
    while ((skip_read && ch != 4) || ((ch = (char) getchar()) && ch != 4)) {
        skip_read = false;
        if (!interactive_loop_step(g, ch, &cursor_x, &cursor_y, &player, &skip_read)) {
            break;
        }
    }
    print_game_results(g);
    reset_terminal(&terminal, g);
    game_delete(g);
}
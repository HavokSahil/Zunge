#include "interactive.h"
#include "utils.h"
#include <signal.h>
#include <ncurses.h>

volatile sig_atomic_t stop_flag = 0;

void interrupt_handler(int signum)
{
    if (signum == SIGINT)
    {
        stop_flag = 1;
    }
}

void interactive_window(char *proc_file)
{
    printf("Opening processed file......");
    FILE *input_file = fopen(proc_file, "r");
    if (input_file == NULL)
    {
        printf("failed\n");
        printf("The file address was %s\n", proc_file);
        exit(EXIT_FAILURE);
    }
    printf("done\n");

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);

    if (has_colors() == FALSE)
    {
        endwin();
        printf("The terminal does not support color\n");
        exit(EXIT_FAILURE);
    }

    start_color();

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_RED, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_WHITE, COLOR_BLACK);

    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    int y_center = rows / 2;
    int x_center = (cols - 11) / 2;

    char prevText[TEXT_WINDOW] = "";
    char presentTEXT[TEXT_WINDOW] = "";

    signal(SIGINT, interrupt_handler);

    char line[TEXT_WINDOW];
    while (!stop_flag && fgets(line, sizeof(line), input_file) != NULL)
    {
        int start_col = max(0, (x_center - strlen(line) / 2));
        attron(COLOR_PAIR(4));
        mvprintw(y_center - 2, start_col, "%s", prevText);

        attron(A_BOLD | COLOR_PAIR(1));
        mvprintw(y_center, start_col, "%s", presentTEXT);

        attron(COLOR_PAIR(4));
        mvprintw(y_center + 2, start_col, "%s", line);

        attron(COLOR_PAIR(3));
        mvprintw(rows - 5, 0, "Print Ctrl+C  twice to exit");
        refresh();

        char command[TEXT_WINDOW + 24];
        snprintf(command, sizeof(command), "echo '%s' | festival --tts", presentTEXT);
        system(command);
        strcpy(prevText, presentTEXT);
        strcpy(presentTEXT, line);
        sleep(1);
    }

    endwin();
    fclose(input_file);
}
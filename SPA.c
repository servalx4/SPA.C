/*
   _____ ____  ___     ______
  / ___// __ \/   |   / ____/
  \__ \/ /_/ / /| |  / /
 ___/ / ____/ ___ |_/ /___
/____/_/   /_/  |_(_)____/

created by sunlitHorizon, 2024
-the spaghettiest code you'll ever see :)
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>
#include <string.h>

int main(int argc, char *argv[]) {
    //check for the --help flag
    if (argc > 1 && strcmp(argv[1], "--help") == 0) {
        printf("   _____ ____  ___     ______\n  / ___// __ \\/   |   / ____/\n  \\__ \\/ /_/ / /| |  / /     \n ___/ / ____/ ___ |_/ /___   \n/____/_/   /_/  |_(_)____/   \n");
        printf("written by plasmaCotton, version 1\n");
        printf("\nSPA.C is a simple terminal-based space shooting game.\n");
        printf("Use the arrow keys to move and SPACE to shoot.\n");
        printf("Avoid dangers and try to get the highest score possible!\n");
        printf("\nOptions:\n");
        printf("  --help     Show this help page\n");
        printf("  q or Q     Quit the game\n");
        printf("  SPACE      Shoot a laser\n");
        printf("  Arrows     Move the player (up, down, left, right)\n");
        printf("\nthank you for playing <3\n");
        return 0;
    }

    typedef struct {
        int type;
        int state;
    } cell;

    const char CHARLIST[] = " ><-~X";
    int score = 0;
    int lost = 0;
    const int gameWidth = 50;
    const int gameHeight = 20;
    cell xyMatrix[gameWidth][gameHeight];
    int enemySpawnTimer = 0;

    for (int i = 0; i < gameWidth; i++) { //make sure the matrix isnt fucked up
        for (int j = 0; j < gameHeight; j++) {
            xyMatrix[i][j].type = 0;
            xyMatrix[i][j].state = 0;
        }
    }

    int playerXY[] = {2, gameHeight / 2}; //position the dumbass
    xyMatrix[playerXY[0]][playerXY[1]].type = 1;

    void Explode(int x, int y) {
        xyMatrix[x-1][y].type = 5;
        xyMatrix[x][y].type = 5;
        xyMatrix[x+1][y].type = 5;
        xyMatrix[x][y+1].type = 5;
        xyMatrix[x][y-1].type = 5;
        xyMatrix[x-1][y].state = 1;
        xyMatrix[x][y].state = 1;
        xyMatrix[x+1][y].state = 1;
        xyMatrix[x][y+1].state = 1;
        xyMatrix[x][y-1].state = 1;
    }

    //initialize ncurses
    initscr();
    timeout(0);
    start_color();
    noecho();
    cbreak();
    curs_set(0);
    keypad(stdscr, TRUE);

    init_pair(0, COLOR_BLACK, COLOR_BLACK);
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_BLACK);
    init_pair(3, COLOR_BLUE, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_YELLOW, COLOR_RED);
    init_pair(6, COLOR_CYAN, COLOR_BLACK);

    while (1) { //game loop
    clear(); //clear the screen
    move(0, 0); //move drawing cursor to the top left
    int drawX = 1; //reset drawing X coordinate
    int drawY = 1; //reset drawing Y coordinate

    if (enemySpawnTimer > 0) {
        enemySpawnTimer-=1;
    } else {
        int randInt = rand() % gameHeight;
        xyMatrix[gameWidth - 1][randInt].type = 2;
        xyMatrix[gameWidth - 1][randInt].state = 8;
        enemySpawnTimer = 40;
    }

    //draw the game itself
    for (int y = 0; y < gameHeight; y++) {
        for (int x = 0; x < gameWidth; x++) {

            //move player laser
            if (xyMatrix[x][y].type == 3) {
                if (xyMatrix[x][y].state == 0) {
                    xyMatrix[x][y].type = 0;
                    if (x + 1 < gameWidth || xyMatrix[x + 1][y].type == 0) {
                        if (xyMatrix[x + 1][y].type == 4) { score += 10; }
                        if (xyMatrix[x + 1][y].type == 2) { score += 100; Explode(x + 1, y); }
                        xyMatrix[x + 1][y].type = 3;
                        xyMatrix[x + 1][y].state = 1;
                    }
                } else { xyMatrix[x][y].state -= 1; }
            }
            //move enemy laser
            if (xyMatrix[x][y].type == 4) {
                if (xyMatrix[x][y].state == 0) {
                    if (x - 1 >= 0) {
                        if (xyMatrix[x - 1][y].type == 1) { lost = 1; Explode(playerXY[0], playerXY[1]); }
                        if (xyMatrix[x - 1][y].type == 2) { xyMatrix[x][y].type=0; xyMatrix[x+1][y].type=0; }
                        xyMatrix[x - 1][y].type = 4;
                        xyMatrix[x - 1][y].state = 2;
                    }

                    xyMatrix[x][y].type = 0;
                } else { xyMatrix[x][y].state -= 1; }

            }
            //move enemy
            if (xyMatrix[x][y].type == 2) {
                if (xyMatrix[x][y].state == 16) { xyMatrix[x - 2][y].type = 4; xyMatrix[x - 2][y].state = 1; }
                if (xyMatrix[x][y].state == 0) {
                    if (x - 1 >= 0) {
                        xyMatrix[x - 1][y].type = 2;
                        xyMatrix[x - 1][y].state = 16;
                    }

                    xyMatrix[x][y].type = 0;
                } else { xyMatrix[x][y].state -= 1; }

            }

            if (xyMatrix[x][y].type == 5) {
                if (xyMatrix[x][y].state == 0) { xyMatrix[x][y].type = 0; }
                else { xyMatrix[x][y].state=0; }
            }
            attron(COLOR_PAIR(xyMatrix[x][y].type));
            printw("%c ", CHARLIST[xyMatrix[x][y].type]);
            attroff(COLOR_PAIR(xyMatrix[x][y].type));

        }
        printw("\n"); //newline after finishing a row
    }

    printw("SCORE: ");
    attron(COLOR_PAIR(6));
    printw("%d\n> ", score);
    attroff(COLOR_PAIR(6));

    int key = getch(); //read dumbass input

    switch (key) {

        case 'q':
        case 'Q':
            printw("Exiting the game...\n"); //show exit message
            refresh();
            break;

        case 32:
            xyMatrix[playerXY[0] + 1][playerXY[1]].type = 3;
            xyMatrix[playerXY[0] + 1][playerXY[1]].state = 1;
            printw("-");
            break;

        case KEY_RIGHT:
            if (playerXY[0] < gameWidth - 1) {
                xyMatrix[playerXY[0]][playerXY[1]].type = 0;
                playerXY[0]++;
            }
            xyMatrix[playerXY[0]][playerXY[1]].type = 1;
            printw(">");
            break;

        case KEY_LEFT:
            if (playerXY[0] > 0) {
                xyMatrix[playerXY[0]][playerXY[1]].type = 0;
                playerXY[0]--;
            }
            xyMatrix[playerXY[0]][playerXY[1]].type = 1;
            printw("<");
            break;

        case KEY_UP:
            if (playerXY[1] > 0) {
                xyMatrix[playerXY[0]][playerXY[1]].type = 0;
                playerXY[1]--;
            }
            xyMatrix[playerXY[0]][playerXY[1]].type = 1;
            printw("^");
            break;

        case KEY_DOWN:
            if (playerXY[1] < gameHeight - 1) {
                xyMatrix[playerXY[0]][playerXY[1]].type = 0;
                playerXY[1]++;
            }
            xyMatrix[playerXY[0]][playerXY[1]].type = 1;
            printw("v");
            break;

    }

    if (key == 'q' || key == 'Q') { //check if player wants to get out of the basement
        usleep(500000); //500ms
        break;
    }

    if (lost == 2) {
	attron(COLOR_PAIR(4));
        printw("You died!");
	attroff(COLOR_PAIR(4));
        refresh();
        sleep(3); //3s
        break;

    } else if (lost ==1) { lost++; }


    refresh();
    usleep(50000); //50ms
    }

  endwin();

  return 0; //if this ever happens, my brain has at least 2 braincells

}


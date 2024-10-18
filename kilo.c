#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

#define CTRL_KEY(k) ((k) & 0x1f) // bitwise ANDs with 00011111 a.k.a. strips the left 3 bits
// 'q': 113=01110001 ==> 00010001=17

/*** data ***/
struct termios original_termios; // variable to save original terminal settings

/*** terminal ***/
void die(const char * s) {
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    perror(s);
    exit(1);
}

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios) == -1)
        die("tcsetattr");
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &original_termios) == -1)
        die("tcgetattr");
    atexit(disableRawMode);
    struct termios raw = original_termios;
    // some flags to modify the behaviour of the terminal
    // https://www.man7.org/linux/man-pages/man3/termios.3.html for more info
    raw.c_iflag &= ~(IXON | ICRNL | INPCK | ISTRIP | BRKINT);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;     //sets the minimum number of bytes of input needed before read() can return
    raw.c_cc[VTIME] = 1;    // tenths of a second; time to wait before read() returns
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
        die("tcsetattr");
}

char editorReadKey() {
    char c;
    int nread;
    // repeats loop while read() times out
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) {
        if (nread == -1 && errno != EAGAIN) {
            die("read");
        }
    }
    return c;
}

/*** output ***/

void editorRefreshScreen() {
    // we are writing 4 bytes to the terminal
    // \x1b means escape; eacape sequence always starts with \0x1b[
    // 2 is the argument (means clear entire screen), J is the command (erase in display)
    // we will be using VT100 esc sequences
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3); //reposition cursor at 1 1 (same as \x1b[1;1H)
}

/*** input ***/
void editorProcessKeypress() {
    char c = editorReadKey();
    switch (c) {
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2J", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
        case 'A':
            printf("%d ('%c')\r\n", c, c);
            break;
    }
}

/*** init ***/
int main() {
    enableRawMode();
    while (1) {
        editorRefreshScreen();
        editorProcessKeypress();
    }
    return 0;
}

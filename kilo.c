#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios original_termios;

void disableRawMode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void enableRawMode() {
    tcgetattr(STDIN_FILENO, &original_termios);
    atexit(disableRawMode);
    struct termios raw = original_termios;
    // some flags to modify the behaviour of the terminal
    raw.c_iflag &= ~(IXON | ICRNL | INPCK | ISTRIP | BRKINT);
    raw.c_oflag &= ~(OPOST);
    raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN);
    raw.c_cflag |= (CS8);
    raw.c_cc[VMIN] = 0;     //sets the minimum number of bytes of input needed before read() can return
    raw.c_cc[VTIME] = 1;    // tenths of a second; time to wait before read() returns
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

int main() {
    enableRawMode();
    while (1) {
        char c = '\0';
        read(STDIN_FILENO, &c, 1);
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') break;
    }
    return 0;
}

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>
#include <errno.h>

struct termios original_termios;

void die(const char * s) {
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

int main() {
    enableRawMode();
    while (1) {
        char c = '\0';
        if (read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) // when read() times out, EAGAIN is thrown
            die("read");
        if (iscntrl(c)) {
            printf("%d\r\n", c);
        } else {
            printf("%d ('%c')\r\n", c, c);
        }
        if (c == 'q') break;
    }
    return 0;
}

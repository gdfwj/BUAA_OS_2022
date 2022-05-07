#include "lib.h"

void umain() {
    writef(
        "Smashing some kernel codes...\n"
        "If your implementation is correct, you may see some TOO LOW here:\n"
		//"this is a very big file AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n"
		//"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n"
		//"AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA\n"
    );
    *(int *) KERNBASE = 0;
    writef("My mission completed!\n");
}

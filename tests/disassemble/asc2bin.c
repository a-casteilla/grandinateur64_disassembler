#include <stdio.h>
#include <stdint.h>

int main () {

    uint32_t nb_to_convert;

    do {
        scanf("%lx", &nb_to_convert);
        if (!feof(stdin)) fwrite(&nb_to_convert, sizeof(nb_to_convert), 1, stdout);
    } while (!feof(stdin));
}

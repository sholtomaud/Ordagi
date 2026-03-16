#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv) {
    if (argc > 1) {
        if (strcmp(argv[1], "--init") == 0) {
            printf("Initializing ontology.cbor...\n");
            return 0;
        }
    }
    printf("OrdAGI REPL (stub)\n");
    return 0;
}

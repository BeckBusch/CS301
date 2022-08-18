// Includes

#include <stdio.h>
#include <limits.h>
#include <unistd.h>

int main() {

    // Get current working directory.
    char cwd[PATH_MAX];
    getcwd(cwd, PATH_MAX);

    // Set the specific file to be read from.
    char filename[] = "map_1.txt";

    char buf[PATH_MAX];
    sprintf(buf, "%s\\%s", cwd, filename);

    // Open the file to be read and copy the values into an array.
    FILE *fp;
    fp = fopen(buf, "r");

    char seq;

    // Read characters from the file and print them until we reach EOF.
    while (1) {
        if (feof(fp) != 0) {
            break;
        }
        seq = fgetc((FILE*)fp);
        printf("%c", seq);
    }

    // Keep the window open indefinitely.
    while (1) {

    }

    return 0;

}

// Includes

#include <stdio.h>
#include <limits.h>
#include <unistd.h>

// Unicode things
#include <Windows.h>

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
    SetConsoleOutputCP(CP_UTF8);

    // Read characters from the file and print them until we reach EOF.
    while (1) {
        if (feof(fp) != 0) {
            break;
        }
        seq = fgetc((FILE*)fp);
        // Use ASCII code for 1, not 1 itself.
        if (seq == 49) {
            printf("%s", "██");
        } else if (seq == 48) {
            printf("▒▒");
        } else if (seq == 10) {
            printf("\n");
        }
        // printf("%c", seq);
    }

    // Close the stream.
    fclose(fp);

    struct queue {
        int grid[100];
        int start;
        int end;
    }

    int x1; int y1; int x2; int y2;

    // Print statements to prompt the user for the start and end co-ordinates.
    printf("\nFrom what point should the traversal start?\n");
    printf("Please enter the x co-ordinate:\n");
    scanf("%d", &x1);
    printf("Please enter the y co-ordinate:\n");
    scanf("%d", &y1);

    printf("\nFrom what point should the traversal end?\n");
    printf("Please enter the x co-ordinate:\n");
    scanf("%d", &x2);
    printf("Please enter the y co-ordinate:\n");
    scanf("%d", &y2);

    BFS(x1, y1, x2, y2);

    //█ is for the completed path...

    // Keep the window open indefinitely.
    while (1) {

    }

    return 0;

}

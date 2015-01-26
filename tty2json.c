#include <stdio.h>

#pragma pack(push,1)
typedef struct {
    unsigned int sec;
    unsigned int usec;
    unsigned int len;
} tty_header;
#pragma pack(pop)

char buffer[BUFSIZ];


int main(int argc, char **argv)
{
    FILE *fp = fopen(argv[1], "rb");
    tty_header header;

    if (fp != NULL) {
        puts("[");
    }

    // Do things manually once so we can place "," in the loop
    if (fread(&header, sizeof(tty_header), 1, fp) != 0) {
        if (header.len > BUFSIZ) {
            perror("Buffer too small\n");
            return -1;
        } else {
            fread(buffer, sizeof(char), header.len, fp);
            buffer[header.len + 1] = '\0';
            printf("[%d,%d,\"%s\"]", header.sec, header.usec, buffer);
        }
    }

    while (fread(&header, sizeof(tty_header), 1, fp) != 0) {
        if (header.len > BUFSIZ) {
            perror("Buffer too small\n");
            return -1;
        } else {
            fread(buffer, sizeof(char), header.len, fp);
            buffer[header.len + 1] = '\0';
            printf(",[%d,%d,\"%s\"]", header.sec, header.usec, buffer);
        }
    }
    puts("]");

    return 0;
}

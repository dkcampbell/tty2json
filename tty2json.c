#include <stdio.h>

#pragma pack(push,1)
typedef struct {
    unsigned int sec;
    unsigned int usec;
    unsigned int len;
} tty_header;
#pragma pack(pop)

char buffer[BUFSIZ];

void read_frame(FILE *fp, unsigned int length) {
    fread(buffer, sizeof(char), length, fp);
    buffer[length + 1] = '\0';
    return;
}

int main(int argc, char **argv)
{
    FILE *fp = NULL;
    tty_header header;

    if (argc == 1) {
        fp = stdin;
    } else if (argc == 2) {
        fp = fopen(argv[1], "rb");
    } else {
        perror("Wrong number of arguments");
        fprintf(stderr, "Read from standard input\nUsage: %s\n", argv[0]);
        fprintf(stderr, "Read from file\nUsage: %s rec.file\n", argv[0]);
        return -1;
    }

    if (fp == NULL) {
        perror("Could not open file.\n");
        return -1;
    } else {
        puts("[");
    }

    // Do things manually once so we can place "," in the loop
    if (fread(&header, sizeof(tty_header), 1, fp) != 0) {
        if (header.len > BUFSIZ) {
            perror("Buffer too small\n");
            return -1;
        } else {
            read_frame(fp, header.len);
            printf("[%d,%d,\"%s\"]", header.sec, header.usec, buffer);
        }
    }

    while (fread(&header, sizeof(tty_header), 1, fp) != 0) {
        if (header.len > BUFSIZ) {
            perror("Buffer too small\n");
            return -1;
        } else {
            read_frame(fp, header.len);
            printf(",[%d,%d,\"%s\"]", header.sec, header.usec, buffer);
        }
    }
    puts("]");

    return 0;
}

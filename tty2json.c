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
    int i = 0, j = 0;
    char tmp;

    for (i = 0; i < length; i++) {
        tmp = fgetc(fp);
        switch (tmp) {
            case '\0':
                buffer[j++] = '\\';
                buffer[j++] = '0';
                break;
            case '\'':
                buffer[j++] = '\\';
                buffer[j++] = '\'';
                break;
            case '\"':
                buffer[j++] = '\\';
                buffer[j++] = '\"';
                break;
            case '\\':
                buffer[j++] = '\\';
                buffer[j++] = '\\';
                break;
            case '\n':
                buffer[j++] = '\\';
                buffer[j++] = 'n';
                break;
            case '\r':
                buffer[j++] = '\\';
                buffer[j++] = 'r';
                break;
            case '\v':
                buffer[j++] = '\\';
                buffer[j++] = 'v';
                break;
            case '\t':
                buffer[j++] = '\\';
                buffer[j++] = 't';
                break;
            case '\b':
                buffer[j++] = '\\';
                buffer[j++] = 'b';
                break;
            case '\f':
                buffer[j++] = '\\';
                buffer[j++] = 'f';
            default:
                if (tmp < ' ') { /* Unprintable ASCII characters */
                    buffer[j++] = '\\';
                    buffer[j++] = 'u';
                    buffer[j++] = '0';
                    buffer[j++] = '0';
                    sprintf(buffer + j, "%02x", tmp);
                    j += 2;
                } else {
                    buffer[j++] = tmp;
                }
                break;
        }
    }

    buffer[j] = '\0';
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
        fprintf(stderr, "Read from file or stdin\nUsage: %s [filename]\n", argv[0]);
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

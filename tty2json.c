/*
 * Copyright (c) 2015, Dan Campbell <dan@compiledworks.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
*/

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

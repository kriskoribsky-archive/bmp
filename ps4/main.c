#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "bmp.h"
#include "transformations.h"

void print_wrong_args(FILE *stream);

void print_desc(FILE *stream);
void print_usage(FILE *stream);
void print_help(FILE *stream);

int main(int arc, char **argv)
{
    FILE *input_stream = stdin;
    FILE *output_stream = stdout;

    struct bmp_image *img = NULL;

    int opt;
    bool terminate = false;
    while (!terminate && (opt = getopt(arc, argv, "rlhvc:s:e:o:i:")) != -1)
    {
        // terminate on each option except in/out streams
        terminate = true;

        switch (opt)
        {
        case 'r':
            img = rotate_right(read_bmp(input_stream));
            break;

        case 'l':
            img = rotate_left(read_bmp(input_stream));
            break;

        case 'h':
            img = flip_horizontally(read_bmp(input_stream));
            break;

        case 'v':
            img = flip_vertically(read_bmp(input_stream));
            break;

        case 'c':;
            uint32_t start_y, start_x, height, width;
            if ((sscanf(optarg, "%u,%u,%u,%u", &start_x, &start_y, &height, &width)) != 4)
            {
                print_wrong_args(stderr);
                print_usage(stderr);
                exit(EXIT_FAILURE);
            }
            img = crop(read_bmp(input_stream), start_y, start_x, height, width);
            break;

        case 's':;
            float factor;
            if ((sscanf(optarg, "%f", &factor)) != 1)
            {
                print_wrong_args(stderr);
                print_usage(stderr);
                exit(EXIT_FAILURE);
            }
            img = scale(read_bmp(input_stream), factor);
            break;

        case 'e':
            img = extract(read_bmp(input_stream), optarg);
            break;

        case 'o':
            output_stream = fopen(optarg, "wb");
            terminate = false;
            break;

        case 'i':
            input_stream = fopen(optarg, "rb");
            terminate = false;
            break;

        default: // '?'
            print_desc(stderr);
            print_usage(stderr);
            print_help(stderr);
            exit(EXIT_FAILURE);
        }
    }

    bool success = write_bmp(output_stream, img);

    free_bmp_image(img);
    fclose(input_stream);
    fclose(output_stream);

    exit(success ? EXIT_SUCCESS : EXIT_FAILURE);
}

void print_wrong_args(FILE *stream)
{
    fprintf(stream, "Error: Wrong option arguments\n");
}

void print_desc(FILE *stream)
{
    fprintf(stream, "Simple BMP transformation tool.\n");
}

void print_usage(FILE *stream)
{
    fprintf(stream, "Usage: bmp [OPTION]... [FILE]...\n");
}

void print_help(FILE *stream)
{
    fprintf(stream, "\n");
    fprintf(stream, "With no FILE, read from standard input or write to standard output.");
    fprintf(stream, "\n");
    fprintf(stream, "  -r            rotate image right\n");
    fprintf(stream, "  -l            rotate image left\n");
    fprintf(stream, "  -h            flip image horizontally\n");
    fprintf(stream, "  -v            flip image vertically\n");
    fprintf(stream, "  -c y,x,h,w    crop image from position [y,x] of giwen height and widht\n");
    fprintf(stream, "  -s factor     scale image by factor\n");
    fprintf(stream, "  -e string     extract colors\n");
    fprintf(stream, "  -o file       write output to file\n");
    fprintf(stream, "  -i file       read input from the file\n");
}
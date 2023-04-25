#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>

#include "bmp.h"
#include "transformations.h"

void print_wrong_args(FILE *stream);

void print_desc(FILE *stream);
void print_usage(FILE *stream);
void print_help(FILE *stream);

#define OPTIONS "hrlxyc:s:e:o:i:"

int main(int arc, char **argv)
{
    FILE *input_stream = stdin;
    FILE *output_stream = stdout;

    // scan streams
    int opt;
    while ((opt = getopt(arc, argv, OPTIONS)) != -1)
    {
        switch (opt)
        {
        case 'i':
            input_stream = fopen(optarg, "rb");
            break;

        case 'o':
            output_stream = fopen(optarg, "wb");
            break;

        case 'h':
            print_desc(stdout);
            print_usage(stdout);
            print_help(stdout);
            exit(EXIT_SUCCESS);
            break;
        }
    }

    // scan transforms
    struct bmp_image *img = read_bmp(input_stream);

    optind = 0;
    while ((opt = getopt(arc, argv, OPTIONS)) != -1)
    {
        switch (opt)
        {
        case 'r':
            img = rotate_right(img);
            break;

        case 'l':
            img = rotate_left(img);
            break;

        case 'x':
            img = flip_horizontally(img);
            break;

        case 'y':
            img = flip_vertically(img);
            break;

        case 'c':;
            uint32_t start_y, start_x, height, width;
            if ((sscanf(optarg, "%u,%u,%u,%u", &start_x, &start_y, &height, &width)) != 4)
            {
                print_wrong_args(stderr);
                print_usage(stderr);
                exit(EXIT_FAILURE);
            }
            img = crop(img, start_y, start_x, height, width);
            break;

        case 's':;
            float factor;
            if ((sscanf(optarg, "%f", &factor)) != 1)
            {
                print_wrong_args(stderr);
                print_usage(stderr);
                exit(EXIT_FAILURE);
            }
            img = scale(img, factor);
            break;

        case 'e':
            img = extract(img, optarg);
            break;

        case 'i':
        case 'o':
            break;

        default: // '?'
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
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"
#include "transformations.h"

int main(void)
{

    // FILE *fr = fopen("data/assets/square.1x1.bmp", "rb");
    // FILE *fr = fopen("data/assets/square.2x1.bmp", "rb");
    // FILE *fr = fopen("data/assets/square.3x1.bmp", "rb");
    // FILE *fr = fopen("data/assets/square.3x2.bmp", "rb");
    // FILE *fr = fopen("data/assets/square.4x4.bmp", "rb");
    FILE *fr = fopen("data/assets/bmp.bmp", "rb");

    // FILE *fr = fopen("data/assets/saboteur.bmp", "rb");
    // FILE *fr = fopen("data/assets/adventurer.bmp", "rb");
    // FILE *fr = fopen("data/assets/prva.akcia.bmp", "rb");

    FILE *fw = fopen("build/results/out/result.bmp", "wb");

    struct bmp_image *img = read_bmp(fr);

    // struct bmp_image *horizontal_flip = flip_horizontally(img);
    struct bmp_image *vertical_flip = flip_vertically(img);

    // write_bmp(fw, horizontal_flip);
    write_bmp(fw, vertical_flip);

    fclose(fr);
    fclose(fw);
    free_bmp_image(img);
    // free_bmp_image(horizontal_flip);
    free_bmp_image(vertical_flip);

    return EXIT_SUCCESS;
}
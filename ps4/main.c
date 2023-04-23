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
    // FILE *fr = fopen("data/assets/bmp.bmp", "rb");

    // FILE *fr = fopen("data/assets/adventurer.bmp", "rb");
    FILE *fr = fopen("data/assets/saboteur.bmp", "rb");
    // FILE *fr = fopen("data/assets/prva.akcia.bmp", "rb");

    FILE *fw1 = fopen("build/results/out/original.bmp", "wb");
    FILE *fw2 = fopen("build/results/out/rotated-right.bmp", "wb");
    FILE *fw3 = fopen("build/results/out/rotated-left.bmp", "wb");
    FILE *fw4 = fopen("build/results/out/scaled.bmp", "wb");

    struct bmp_image *img = read_bmp(fr);

    struct bmp_image *rotated_right = rotate_right(img);
    struct bmp_image *rotated_left = rotate_left(img);
    struct bmp_image *scaled = scale(img, 30.0f);

    write_bmp(fw1, img);
    write_bmp(fw2, rotated_right);
    write_bmp(fw3, rotated_left);
    write_bmp(fw4, scaled);

    fclose(fr);
    fclose(fw1);
    fclose(fw2);
    fclose(fw3);
    fclose(fw4);

    free_bmp_image(img);
    free_bmp_image(rotated_right);
    free_bmp_image(rotated_left);
    free_bmp_image(scaled);

    return EXIT_SUCCESS;
}
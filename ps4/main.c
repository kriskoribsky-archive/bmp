#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"
#include "transformations.h"

int main(void)
{
    FILE *fr = fopen("data/assets/prva.akcia.bmp", "rb");

    FILE *fw1 = fopen("build/results/out/original.bmp", "wb");
    FILE *fw2 = fopen("build/results/out/flipped-horizontally.bmp", "wb");
    FILE *fw3 = fopen("build/results/out/flipped-vertically.bmp", "wb");
    FILE *fw4 = fopen("build/results/out/rotated-right.bmp", "wb");
    FILE *fw5 = fopen("build/results/out/rotated-left.bmp", "wb");
    FILE *fw6 = fopen("build/results/out/scaled.bmp", "wb");
    FILE *fw7 = fopen("build/results/out/extracted.bmp", "wb");

    struct bmp_image *img = read_bmp(fr);

    struct bmp_image *flipped_horizontally = flip_horizontally(img);
    struct bmp_image *flipped_vertically = flip_vertically(img);
    struct bmp_image *rotated_right = rotate_right(img);
    struct bmp_image *rotated_left = rotate_left(img);
    struct bmp_image *scaled = scale(img, 0.548151f);
    struct bmp_image *extracted = extract(img, "br");

    write_bmp(fw1, img);
    write_bmp(fw2, flipped_horizontally);
    write_bmp(fw3, flipped_vertically);
    write_bmp(fw4, rotated_right);
    write_bmp(fw5, rotated_left);
    write_bmp(fw6, scaled);
    write_bmp(fw7, extracted);

    fclose(fr);
    fclose(fw1);
    fclose(fw2);
    fclose(fw3);
    fclose(fw4);
    fclose(fw5);
    fclose(fw6);
    fclose(fw7);

    free_bmp_image(img);
    free_bmp_image(flipped_horizontally);
    free_bmp_image(flipped_vertically);
    free_bmp_image(rotated_right);
    free_bmp_image(rotated_left);
    free_bmp_image(scaled);
    free_bmp_image(extracted);

    return EXIT_SUCCESS;
}
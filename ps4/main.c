#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "bmp.h"

int main(void)
{

    // FILE *fr = fopen("data/assets/square.1x1.bmp", "rb");
    // FILE *fw = fopen("build/results/out/square.1x1OUT.bmp", "wb");

    // FILE *fr = fopen("data/assets/square.2x2.bmp", "rb");
    // FILE *fw = fopen("build/results/out/square.2x2OUT.bmp", "wb");

    // FILE *fr = fopen("data/assets/square.4x4.bmp", "rb");
    // FILE *fw = fopen("build/results/out/square.4x4OUT.bmp", "wb");

    // FILE *fr = fopen("data/assets/saboteur.bmp", "rb");
    // FILE *fw = fopen("build/results/out/saboteur-out.bmp", "wb");

    FILE *fr = fopen("data/assets/adventurer.bmp", "rb");
    FILE *fw = fopen("build/results/out/adventurer-out.bmp", "wb");

    // FILE *fr = fopen("data/assets/prva.akcia.bmp", "rb");
    // FILE *fw = fopen("build/results/out/prva.akcia-out.bmp", "wb");

    struct bmp_image *img = read_bmp(fr);

    printf("original:\n");
    printf("offset: %u\n", img->header->offset);
    printf("img width: %u\n", img->header->width);
    printf("img height: %u\n", img->header->height);
    printf("bits per pixel: %d\n", img->header->bpp);

    struct bmp_image *copy = copy_bmp(img);
    printf("copy:\n");
    printf("offset: %u\n", copy->header->offset);
    printf("img width: %u\n", copy->header->width);
    printf("img height: %u\n", copy->header->height);
    printf("bits per pixel: %d\n", copy->header->bpp);

    write_bmp(fw, copy);

    fclose(fr);
    fclose(fw);
    free_bmp_image(img);
    free_bmp_image(copy);

    return EXIT_SUCCESS;
}
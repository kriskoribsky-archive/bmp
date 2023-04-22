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

    struct bmp_image *img = read_bmp(fr);

    printf("offset: %u\n", img->header->offset);
    printf("img width: %u\n", img->header->width);
    printf("img height: %u\n", img->header->height);
    printf("bits per pixel: %d\n", img->header->bpp);

    for (int i = 0; i < (int)(img->header->width * img->header->height); i++)
    {
        printf("%d: r=%d g=%d b=%d\n", i, img->data[i].red, img->data[i].green, img->data[i].blue);
    }

    write_bmp(fw, img);

    fclose(fr);
    fclose(fw);
    free_bmp_image(img);

    return EXIT_SUCCESS;
}
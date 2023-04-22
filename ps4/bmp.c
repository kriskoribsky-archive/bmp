#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bmp.h"

#define IS_LITTLE_ENDIAN *(uint8_t *)&((uint16_t){1}) // host endianness
#define IS_BIG_ENDIAN !IS_LITTLE_ENDIAN

/* private functions */
uint16_t swap_uint16(uint16_t x);
uint32_t swap_uint32(uint32_t x);
void swap_endianness(struct bmp_header *header);

/* public functions */
struct bmp_image *read_bmp(FILE *stream)
{
    if (stream == NULL)
    {
        return NULL;
    }

    struct bmp_image *img = malloc(sizeof(struct bmp_image));

    img->header = read_bmp_header(stream);
    if (img->header == NULL)
    {
        fprintf(stderr, "Error: This is not a BMP file.\n");
        free_bmp_image(img);
        return NULL;
    }
    img->data = read_data(stream, img->header);
    if (img->data == NULL)
    {
        fprintf(stderr, "Error: Corrupted BMP file.\n");
        free_bmp_image(img);
        return NULL;
    }

    return img;
}

struct bmp_header *read_bmp_header(FILE *stream)
{
    if (stream == NULL)
    {
        return NULL;
    }

    struct bmp_header *header = malloc(sizeof(struct bmp_header));
    if (header == NULL)
    {
        return NULL;
    }

    fseek(stream, 0, SEEK_SET);
    fread(header, sizeof(struct bmp_header), 1, stream);

    // swap to system's big endian
    if (IS_BIG_ENDIAN)
        swap_endianness(header);

    return header->type != BMP_HEADER ? NULL : header;
}

struct pixel *read_data(FILE *stream, const struct bmp_header *header)
{
    if (stream == NULL || header == NULL)
    {
        return NULL;
    }

    struct pixel *data = malloc(header->width * header->height * sizeof(struct pixel));
    if (data == NULL)
    {
        return NULL;
    }

    uint32_t offset = header->offset;
    uint32_t width = header->width;
    uint32_t height = header->height;
    uint16_t bpp = header->bpp;

    uint8_t pad_bytes = (BMP_WORD - ((width * bpp / BYTE) % BMP_WORD)) % BMP_WORD;

    fseek(stream, offset, SEEK_SET);      // skip header & color pallette
    for (uint32_t i = 0; i < height; i++) // load pixel rows without padding
    {
        fread(data + i * width, sizeof(struct pixel), width, stream);
        fseek(stream, pad_bytes, SEEK_CUR);
    }
    return data;
}

bool write_bmp(FILE *stream, const struct bmp_image *image)
{
    if (stream == NULL || image == NULL)
    {
        return false;
    }

    fseek(stream, 0, SEEK_SET);
    fwrite(image->header, sizeof(struct bmp_header), 1, stream);

    uint32_t offset = image->header->offset;
    uint32_t width = image->header->width;
    uint32_t height = image->header->height;
    uint16_t bpp = image->header->bpp;

    uint8_t pad_bytes = (BMP_WORD - ((width * bpp / BYTE) % BMP_WORD)) % BMP_WORD;
    uint8_t padding[pad_bytes];
    memset(padding, PADDING_CHAR, pad_bytes);

    // swap back to little endian
    if (IS_BIG_ENDIAN)
        swap_endianness(image->header);

    fseek(stream, offset, SEEK_SET);      // skip header & color pallette
    for (uint32_t i = 0; i < height; i++) // write padded pixel rows
    {
        fwrite(image->data + i * width, sizeof(struct pixel), width, stream);
        fwrite(&padding, sizeof(padding), 1, stream);
    }
    return true;
}

void free_bmp_image(struct bmp_image *image)
{
    free(image->header);
    image->header = NULL;

    free(image->data);
    image->data = NULL;

    free(image);
}

void swap_endianness(struct bmp_header *header)
{
    header->type = swap_uint16(header->type);
    header->size = swap_uint32(header->size);
    header->reserved1 = swap_uint16(header->reserved1);
    header->reserved2 = swap_uint16(header->reserved2);
    header->offset = swap_uint32(header->offset);
    header->dib_size = swap_uint32(header->dib_size);
    header->width = swap_uint32(header->width);
    header->height = swap_uint32(header->height);
    header->planes = swap_uint16(header->planes);
    header->bpp = swap_uint16(header->bpp);
    header->compression = swap_uint32(header->compression);
    header->image_size = swap_uint32(header->image_size);
    header->x_ppm = swap_uint32(header->x_ppm);
    header->y_ppm = swap_uint32(header->y_ppm);
    header->num_colors = swap_uint32(header->num_colors);
    header->important_colors = swap_uint32(header->important_colors);
}

uint16_t swap_uint16(uint16_t x)
{
    return (x >> 8) | (x << 8);
}
uint32_t swap_uint32(uint32_t x)
{
    x = ((x >> 8) & 0x00FF00FF) | ((x << 8) & 0xFF00FF00);
    return (x >> 16) | (x << 16);
}
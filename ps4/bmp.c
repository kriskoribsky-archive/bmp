#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "bmp.h"
#include "helpers.h"

// PUBLIC IMPLEMENTATION
// ================================================================================

struct bmp_image *read_bmp(FILE *stream)
{
    CHECK_NULL(stream);

    struct bmp_image *img = alloc_bmp_image();
    CHECK_NULL(img);

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

bool write_bmp(FILE *stream, const struct bmp_image *image)
{
    CHECK_NULL(stream);
    CHECK_NULL(image);

    fseek(stream, 0, SEEK_SET);
    fwrite(image->header, sizeof(struct bmp_header), 1, stream);

    uint32_t offset = image->header->offset;
    uint32_t width = image->header->width;
    uint32_t height = image->header->height;
    uint8_t pad_bytes = pixel_padding_size(image->header);

    uint8_t padding[pad_bytes];
    memset(padding, PADDING, pad_bytes);

    swap_endianness(image->header); // swap back to default endian

    fseek(stream, offset, SEEK_SET);      // skip header & color pallette
    for (uint32_t i = 0; i < height; i++) // write padded pixel rows
    {
        fwrite(image->data + i * width, sizeof(struct pixel), width, stream);
        fwrite(&padding, sizeof(padding), 1, stream);
    }
    return true;
}

struct bmp_header *read_bmp_header(FILE *stream)
{
    CHECK_NULL(stream);

    struct bmp_header *header = alloc_bmp_header();
    CHECK_NULL(header);

    fseek(stream, 0, SEEK_SET);
    fread(header, sizeof(struct bmp_header), 1, stream);

    swap_endianness(header); // swap to system's endianness

    if (!bmp_header_valid(header))
    {
        return NULL;
    }

    return header;
}

struct pixel *read_data(FILE *stream, const struct bmp_header *header)
{
    CHECK_NULL(stream);
    CHECK_NULL(header);

    struct pixel *data = alloc_data(header->width, header->height);
    CHECK_NULL(data);

    uint32_t offset = header->offset;
    uint32_t width = header->width;
    uint32_t height = header->height;
    uint8_t pad_bytes = pixel_padding_size(header);

    fseek(stream, offset, SEEK_SET);      // skip header & color pallette
    for (uint32_t i = 0; i < height; i++) // load pixel rows without padding
    {
        fread(data + i * width, sizeof(struct pixel), width, stream);
        fseek(stream, pad_bytes, SEEK_CUR);
    }
    return data;
}

void free_bmp_image(struct bmp_image *image)
{
    if (image == NULL)
    {
        return;
    }

    free(image->header);
    image->header = NULL;

    free(image->data);
    image->data = NULL;

    free(image);
}

// HELPER IMPLEMENTATION
// ================================================================================

struct bmp_image *copy_bmp(const struct bmp_image *image)
{
    CHECK_NULL(image);

    struct bmp_image *copy = alloc_bmp_image();
    CHECK_NULL(copy);

    copy->header = copy_bmp_header(image->header);
    CHECK_NULL_AND_FREE(copy->header, copy, copy);

    copy->data = copy_data(image->header, image->data);
    CHECK_NULL_AND_FREE(copy->data, copy->header, copy);

    return copy;
}

struct bmp_image *create_bmp(const struct bmp_header *header, uint32_t width, uint32_t height)
{
    CHECK_NULL(header);
    CHECK_VALID_BMP(header);

    struct bmp_image *copy = alloc_bmp_image();
    CHECK_NULL(copy);

    copy->header = copy_bmp_header(header);
    CHECK_NULL_AND_FREE(copy->header, copy, copy);

    // update metadata & size
    copy->header->width = width;
    copy->header->height = height;
    copy->header->size = bmp_file_size(copy->header);
    CHECK_VALID_BMP_AND_FREE(copy->header, copy, copy);

    // allocate memory for pixel array, but do not copy any data
    copy->data = alloc_data(width, height);
    CHECK_NULL_AND_FREE(copy->data, copy->header, copy);

    return copy;
}

struct bmp_header *copy_bmp_header(const struct bmp_header *header)
{
    CHECK_NULL(header);

    struct bmp_header *header_copy = alloc_bmp_header();
    CHECK_NULL(header_copy);

    memcpy(header_copy, header, sizeof(struct bmp_header));

    return header_copy;
}

struct pixel *copy_data(const struct bmp_header *header, const struct pixel *data)
{
    CHECK_NULL(header);
    CHECK_NULL(data);

    struct pixel *data_copy = alloc_data(header->width, header->height);
    CHECK_NULL(data_copy);

    memcpy(data_copy, data, header->width * header->height * sizeof(struct pixel));

    return data_copy;
}

struct bmp_image *alloc_bmp_image(void)
{
    struct bmp_image *img = malloc(sizeof(struct bmp_image));
    if (img == NULL)
    {
        return NULL;
    }

    img->header = NULL;
    img->data = NULL;

    return img;
}

struct bmp_header *alloc_bmp_header(void)
{
    struct bmp_header *header = malloc(sizeof(struct bmp_header));
    return header;
}

struct pixel *alloc_data(uint32_t width, uint32_t height)
{
    struct pixel *data = malloc(width * height * sizeof(struct pixel));
    return data;
}

bool bmp_header_valid(const struct bmp_header *header)
{
    CHECK_METADATA(header->type == MAGIC);
    CHECK_METADATA(header->offset == OFFSET);
    CHECK_METADATA(header->dib_size == DIB_SIZE);
    CHECK_METADATA(header->planes == PLANES);
    CHECK_METADATA(header->compression == COMPRESSION);
    CHECK_METADATA(header->num_colors == NUM_CLR);
    CHECK_METADATA(header->important_colors == IMPORANT_CLR);
    CHECK_METADATA(header->bpp == BPP16 || header->bpp == BPP24);
    CHECK_METADATA(header->width >= MIN_SIZE && header->width <= MAX_SIZE);
    CHECK_METADATA(header->height >= MIN_SIZE && header->height <= MAX_SIZE);
    CHECK_METADATA(header->size == bmp_file_size(header));

    return true;
}

uint32_t bmp_file_size(const struct bmp_header *header)
{
    return header->offset + pixel_array_size(header);
}

uint32_t pixel_row_size(const struct bmp_header *header)
{
    return (uint32_t)(((float)(header->bpp * header->width) / DWORD) * BMPWORD);
}

uint8_t pixel_padding_size(const struct bmp_header *header)
{
    return (BMPWORD - pixel_row_size(header) % BMPWORD) % BMPWORD;
}

uint32_t pixel_array_size(const struct bmp_header *header)
{
    return header->height * (pixel_row_size(header) + pixel_padding_size(header));
}

void swap_endianness(struct bmp_header *header)
{
    if (IS_BIG_ENDIAN)
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
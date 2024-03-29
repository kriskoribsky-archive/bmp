#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdint.h>

#include "bmp.h"

// HELPER MACROS
// ================================================================================

#ifdef DEBUG
#include <assert.h>
#define ASSERT(expr) assert(expr)
#else
#define ASSERT(expr) ((void)0)
#endif

#define IS_LITTLE_ENDIAN *(uint8_t *)&((uint16_t){1}) // host endianness
#define IS_BIG_ENDIAN !IS_LITTLE_ENDIAN

#define FREE(ptr)     \
    {                 \
        free(ptr);    \
        (ptr) = NULL; \
    }

#define CHECK_NULL(ptr)    \
    {                      \
        if ((ptr) == NULL) \
        {                  \
            return NULL;   \
        }                  \
    }

#define CHECK_NULL_AND_FREE(ptr, free1, free2) \
    {                                          \
        if ((ptr) == NULL)                     \
        {                                      \
            FREE(free1);                       \
            FREE(free2);                       \
            return NULL;                       \
        }                                      \
    }

#define CHECK_VALID_BMP(header)        \
    {                                  \
        if (!bmp_header_valid(header)) \
        {                              \
            return NULL;               \
        }                              \
    }

#define CHECK_VALID_BMP_AND_FREE(header, free1, free2) \
    {                                                  \
        if (!bmp_header_valid(header))                 \
        {                                              \
            FREE(free1);                               \
            FREE(free2);                               \
            return NULL;                               \
        }                                              \
    }

#define CHECK_METADATA(expr) \
    {                        \
        ASSERT(expr);        \
        if ((expr) == false) \
        {                    \
            return false;    \
        }                    \
    }

/* currently supported types of bmp metadata */
enum BMP_META
{
    MAGIC = 0x4d42,                     // "BM" (0x42, 0x4D)
                                        // file size
    RESERVED1 = 0,                      // not used (0)
    RESERVED2 = 0,                      // not used (0)
    OFFSET = sizeof(struct bmp_header), // offset to image data (54B)
    DIB_SIZE = 40,                      // DIB header size (40B)
    MIN_SIZE = 1,                       // width/height in pixels
    MAX_SIZE = UINT32_MAX,              // width/height in pixels
    PLANES = 1,                         // number of color planes
    BPP24 = 24,                         // bits per pixel (1/4/8/24)
    BPP16 = 16,                         // bits per pixel (1/4/8/24)
    COMPRESSION = 0,                    // compression type (0/1/2) 0
    XPPM = 0,                           // X Pixels per meter (0)
    YPPM = 0,                           // Y Pixels per meter (0)
    NUM_CLR = 0,                        // number of colors (0)
    IMPORANT_CLR = 0                    // important colors (0)
};

/* properties of bmp image format */
enum BMP_FORMAT
{
    DWORD = 32,    // 32 bits
    BMPWORD = 4,   // 4 bytes
    PADDING = '\0' // pixel row padding
};

// HELPER DECLARATION
// ================================================================================

/**
 * Copy BMP image
 *
 * Performs a deep copy of image and data it points to. If the original
 * image is not provided, returns `NULL`
 *
 * @param image the image to copy
 * @return reference to the `bmp_image` structure of the copied image or `NULL` if `image` is `NULL`
 */
struct bmp_image *copy_bmp(const struct bmp_image *image);

/**
 * Create new BMP image
 *
 * Performs a deep copy of a header with new dimensions.
 * Allocates new memory for pixel array of size width x height.
 * DOESN'T COPY any pixel memory.
 *
 * @param header the BMP header structure
 * @return reference to the `bmp_image` structure or `NULL` if `header` is invalid
 */
struct bmp_image *create_bmp(const struct bmp_header *header, uint32_t width, uint32_t height);

/**
 * Copy BMP header
 *
 * Performs a deep copy of BMP header. If the original header is not
 * provied, returns `NULL`.
 *
 * @param header the BMP header structure
 * @return `bmp_header` structure of the copied header or `NULL` if header is `NULL`
 */
struct bmp_header *copy_bmp_header(const struct bmp_header *header);

/**
 * Copy the pixels
 *
 * Performs a deep copy of pixels. If the reference to the original
 * pixels is not provied,  returns `NULL`. If the header is not provided
 * also returns `NULL`.
 *
 * @param header the BMP header structure
 * @param data reference to the original pixel data to copy
 * @return the pixels of the image or `NULL` if pixels or header are broken
 */
struct pixel *copy_data(const struct bmp_header *header, const struct pixel *data);

/**
 * Allocate memory for a `bmp_image` structure.
 *
 * Allocates memory for a `bmp_image` structure and initializes its fields
 * to NULL values. Returns a pointer to the allocated `bmp_image` structure.
 *
 * @return Pointer to the allocated `bmp_image` structure or `NULL` if memory allocation fails
 * @note The allocated memory should be freed with free() when it is no longer needed.
 * @note The returned pointer should be checked for NULL to ensure successful allocation.
 */
struct bmp_image *alloc_bmp_image(void);

/**
 * Allocate memory for a `bmp_header` structure.
 *
 * Allocates memory for a `bmp_header` structure and leaves its fields uninitialized.
 * Returns a pointer to the allocated `bmp_header` structure.
 *
 * @return Pointer to the allocated `bmp_header` structure or `NULL` if memory allocation fails
 * @note The allocated memory should be freed with free() when it is no longer needed.
 * @note The returned pointer should be checked for NULL to ensure successful allocation.
 */
struct bmp_header *alloc_bmp_header(void);

/**
 * Allocates memory for pixel data based on the provided width and height.
 *
 * @param width The width of the pixel data.
 * @param height The height of the pixel data.
 * @return A pointer to the allocated pixel data.
 * @note The allocated memory should be freed with free() when it is no longer needed.
 * @note The returned pointer should be checked for NULL to ensure successful allocation.
 */
struct pixel *alloc_data(uint32_t width, uint32_t height);

/**
 * Check if bmp image header is valid.
 *
 * Use metadata from bmp header to determine whether bmp image is valid.
 * Compare values with predefined supported bmp image types.
 * Header is valid if:
 *
 * 1. its magic number is 0x4d42
 * 2. image data begins immediately after the header data
 * 3. the DIB header is the correct size
 * 4. there is only one image plane
 * 5. there is no compression
 * 6. num_colors and important_colors are both 0
 * 7. the image has either 16 or 24 bits per pixel
 * 8. the size and imagesize fields are correct in relation to the bits, width, and height fields or the file size
 */
bool bmp_header_valid(const struct bmp_header *header);

/**
 * Calculate BMP file size
 *
 * Calculates the total size of the BMP file in bytes, including the BMP header
 * and pixel array, using the metadata from the BMP header.
 *
 * @param header the BMP header structure
 * @return size of the BMP file in bytes
 */
uint32_t bmp_file_size(const struct bmp_header *header);

/**
 * Calculate gross pixel array row size without padding
 *
 * The bits representing the bitmap pixels are packed in rows (scan lines).
 * This function calculates number of bytes necessary to store one row of pixels.
 *
 * @param header the BMP header structure
 * @return number of bytes in each row of pixel array (without padding bytes)
 */
uint32_t pixel_row_size(const struct bmp_header *header);

/**
 * Calculate total pixel array size with padding
 *
 * Uses metadata from header to calculate total bmp image size in bytes.
 * Calculated size corresponds to size in bmp header file size,
 * which is located right after tha magic file format.
 *
 * @param header the BMP header structure
 * @return size of image in bytes
 */
uint32_t pixel_array_size(const struct bmp_header *header);

/**
 * Calculate image data padding
 *
 * The pixel array is a block of 32-bit DWORDs. Padding bytes must be appended
 * to the end of the rows in order to bring up the length of the rows
 * to a multiple of four bytes.
 *
 * @param header the BMP header structure
 * @return number of bytes used for padding for each pixel row
 */
uint8_t pixel_padding_size(const struct bmp_header *header);

/**
 * Swap endianness of BMP header
 *
 * Swaps the endianness of a BMP header structure in place.
 *
 * @param header the BMP header structure to swap endianness
 */
void swap_endianness(struct bmp_header *header);

/**
 * Swap uint16_t value endianness
 *
 * Swaps the endianness of a 16-bit unsigned integer value.
 *
 * @param x the value to swap endianness
 * @return the value with swapped endianness
 */
uint16_t swap_uint16(uint16_t x);

/**
 * Swap uint32_t value endianness
 *
 * Swaps the endianness of a 32-bit unsigned integer value.
 *
 * @param x the value to swap endianness
 * @return the value with swapped endianness
 */
uint32_t swap_uint32(uint32_t x);

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

    CHECK_VALID_BMP_AND_FREE(header, header, header);

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
    copy->header->image_size = pixel_array_size(copy->header);
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
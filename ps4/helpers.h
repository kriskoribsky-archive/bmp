#ifndef _HELPERS_H
#define _HELPERS_H

#include <stdbool.h>
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

#endif

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

#ifndef _BMP_H
#define _BMP_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define BMP_HEADER 0x4d42 // "MB"
#define BMP_WORD 4        // DWORD
#define PADDING_CHAR '\0' // pixels
#define BYTE 8            // 8 bits

#define IS_LITTLE_ENDIAN *(uint8_t *)&((uint16_t){1}) // host endianness
#define IS_BIG_ENDIAN !IS_LITTLE_ENDIAN

#define CHECK_NULL(ptr) \
    if (ptr == NULL)    \
        return NULL;

#define FREE(ptr) \
    free(ptr);    \
    ptr = NULL

/**
 * Structure contains information about the type, size, layout, dimensions
 * and color format of a BMP file. Size of structure is 54 bytes.
 */
struct bmp_header
{
    uint16_t type;             // "BM" (0x42, 0x4D)
    uint32_t size;             // file size
    uint16_t reserved1;        // not used (0)
    uint16_t reserved2;        // not used (0)
    uint32_t offset;           // offset to image data (54B)
    uint32_t dib_size;         // DIB header size (40B)
    uint32_t width;            // width in pixels
    uint32_t height;           // height in pixels
    uint16_t planes;           // 1
    uint16_t bpp;              // bits per pixel (1/4/8/24)
    uint32_t compression;      // compression type (0/1/2) 0
    uint32_t image_size;       // size of picture in bytes, 0
    uint32_t x_ppm;            // X Pixels per meter (0)
    uint32_t y_ppm;            // X Pixels per meter (0)
    uint32_t num_colors;       // number of colors (0)
    uint32_t important_colors; // important colors (0)
} __attribute__((__packed__));

/**
 * This structure describes a color consisting of relative intensities of
 * red, green, and blue.
 */
struct pixel
{
    uint8_t blue;
    uint8_t green;
    uint8_t red;
    // uint8_t alpha;
} __attribute__((__packed__));

/**
 * Structure describes the BMP file format, which consists from two parts:
 * 1. the header (metadata)
 * 2. the data (pixels)
 */
struct bmp_image
{
    struct bmp_header *header;
    struct pixel *data; // nr. of pixels is `width` * `height`
};

/**
 * Loads a BMP file from an input stream
 *
 * Creates BMP structure from data comming from an opened stream. If stream
 * is `NULL` or is corrupted (not a BMP file), function returns `NULL`
 * and prints error message to standard error output.
 *
 * @param stream opened stream, where the image data are located
 * @return reference to the `bmp_image` structure of the created image or `NULL` if `stream` is `NULL`
 */
struct bmp_image *read_bmp(FILE *stream);

/**
 * Writes a BMP file to an output stream
 *
 * Function writes BMP image to opened stream. If stream is not open
 * (is `NULL`) or image is `NULL`, function returns `false`.
 *
 * @param stream opened stream, where the image will be written
 * @param image the image to write
 * @return `true`, if BMP image was saved successfully, `false` otherwise.
 */
bool write_bmp(FILE *stream, const struct bmp_image *image);

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
 * Reads BMP header from input stream
 *
 * Reads and returns BMP header from opened input stream. The header is located
 * at it's beginning. If the stream is not opened or it is corrupted, function
 * returns `NULL`.
 *
 * @param stream opened stream, where the image data are located
 * @return `bmp_header` structure or `NULL`, if stream is not open or broken
 */
struct bmp_header *read_bmp_header(FILE *stream);

/**
 * Read the pixels
 *
 * Reads the data (pixels) from stream representing the image. If the stream
 * is not open or header is not provided, returns `NULL`.
 *
 * @param stream opened stream, where the image data are located
 * @param header the BMP header structure
 * @return the pixels of the image or `NULL` if stream or header are broken
 */
struct pixel *read_data(FILE *stream, const struct bmp_header *header);

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
 * Free the BMP image from the memory
 *
 * Function frees the allocated memory for the BMP image.
 *
 * @param image the BMP image object
 */
void free_bmp_image(struct bmp_image *image);

#endif

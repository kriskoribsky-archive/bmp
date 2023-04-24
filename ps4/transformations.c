#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "transformations.h"
#include "bmp.h"

// HELPER MACROS
// ================================================================================
#define CHECK_NULL(ptr)    \
    {                      \
        if ((ptr) == NULL) \
        {                  \
            return NULL;   \
        }                  \
    }

// PUBLIC IMPLEMENTATION
// ================================================================================

extern struct bmp_image *copy_bmp(const struct bmp_image *image);
extern struct bmp_image *create_bmp(const struct bmp_header *header, uint32_t width, uint32_t height);

struct bmp_image *flip_horizontally(const struct bmp_image *image)
{
    CHECK_NULL(image);

    struct bmp_image *copy = copy_bmp(image);
    CHECK_NULL(copy);

    uint32_t width = copy->header->width;
    uint32_t height = copy->header->height;

    // flipping horizontally means flipping along vertical axis
    for (uint32_t left_col = 0, right_col = width - 1; left_col < right_col; left_col++, right_col--)
    {
        for (uint32_t row = 0; row < height; row++)
        {
            struct pixel tmp = copy->data[row * width + left_col];
            copy->data[row * width + left_col] = copy->data[row * width + right_col];
            copy->data[row * width + right_col] = tmp;
        }
    }
    return copy;
}

struct bmp_image *flip_vertically(const struct bmp_image *image)
{
    CHECK_NULL(image);

    struct bmp_image *copy = create_bmp(image->header, image->header->width, image->header->height);
    CHECK_NULL(copy);

    uint32_t width = copy->header->width;
    uint32_t height = copy->header->height;
    size_t row_bytes = width * sizeof(struct pixel);

    // flipping vertically means flipping along horizontal axis
    // bmp images are stored in bottom to top order
    for (uint32_t botom_row = 0, top_row = height - 1; botom_row < top_row; botom_row++, top_row--)
    {
        memcpy(&copy->data[botom_row * width], &image->data[top_row * width], row_bytes);
        memcpy(&copy->data[top_row * width], &image->data[botom_row * width], row_bytes);
    }

    // copy the middle row
    if (height % 2 == 1)
    {
        memcpy(&copy->data[height / 2 * width], &image->data[height / 2 * width], row_bytes);
    }

    return copy;
}

struct bmp_image *rotate_right(const struct bmp_image *image)
{
    CHECK_NULL(image);

    struct bmp_image *copy = copy_bmp(image);
    CHECK_NULL(copy);

    uint32_t width = copy->header->width;
    uint32_t height = copy->header->height;

    // overwrite copied pixel data with rotated data
    for (uint32_t row = 0; row < height; row++)
    {
        for (uint32_t col = 0; col < width; col++)
        {
            memcpy(&copy->data[(width - 1 - col) * height + row], &image->data[row * width + col], sizeof(struct pixel));
        }
    }

    // update header metadata
    copy->header->width = image->header->height;
    copy->header->height = image->header->width;

    return copy;
}

struct bmp_image *rotate_left(const struct bmp_image *image)
{
    CHECK_NULL(image);

    struct bmp_image *copy = copy_bmp(image);
    CHECK_NULL(copy);

    uint32_t width = copy->header->width;
    uint32_t height = copy->header->height;

    // overwrite copied pixel data with rotated data
    for (uint32_t row = 0; row < height; row++)
    {
        for (uint32_t col = 0; col < width; col++)
        {
            memcpy(&copy->data[col * height + height - 1 - row], &image->data[row * width + col], sizeof(struct pixel));
        }
    }

    // update header metadata
    copy->header->width = image->header->height;
    copy->header->height = image->header->width;

    return copy;
}

struct bmp_image *crop(const struct bmp_image *image, const uint32_t start_y, const uint32_t start_x, const uint32_t height, const uint32_t width)
{
    CHECK_NULL(image);

    if (start_x + width > image->header->width || start_y + height > image->header->height)
    {
        return NULL;
    }

    struct bmp_image *copy = create_bmp(image->header, width, height);
    CHECK_NULL(copy);

    uint32_t old_w = image->header->width;
    uint32_t old_h = image->header->height;
    uint32_t start_row = (old_h - (start_y + height)) * old_w; // bmp is indexed bottom up
    size_t row_bytes = width * sizeof(struct pixel);

    for (uint32_t row = 0; row < height; row++)
    {
        memcpy(&copy->data[row * width], &image->data[start_row + row * old_w + start_x], row_bytes);
    }

    return copy;
}

struct bmp_image *scale(const struct bmp_image *image, float factor)
{
    CHECK_NULL(image);

    uint32_t w = image->header->width;
    uint32_t new_w = (uint32_t)roundf((float)image->header->width * factor);
    uint32_t new_h = (uint32_t)roundf((float)image->header->height * factor);

    struct bmp_image *copy = create_bmp(image->header, new_w, new_h);
    CHECK_NULL(copy);

    for (uint32_t new_row = 0; new_row < new_h; new_row++)
    {
        for (uint32_t new_col = 0; new_col < new_w; new_col++)
        {
            uint32_t row = (uint32_t)((float)new_row / factor);
            uint32_t col = (uint32_t)((float)new_col / factor);

            memcpy(&copy->data[new_row * new_w + new_col], &image->data[row * w + col], sizeof(struct pixel));
        }
    }
    return copy;
}

struct bmp_image *extract(const struct bmp_image *image, const char *colors_to_keep)
{
    CHECK_NULL(image);
    CHECK_NULL(colors_to_keep);

    uint8_t blue = 0x00;
    uint8_t green = 0x00;
    uint8_t red = 0x00;
    for (int c, i = 0; (c = colors_to_keep[i]) != '\0'; i++)
    {
        switch (c)
        {
        case 'b':
            blue |= 0xFF;
            break;
        case 'g':
            green |= 0xFF;
            break;
        case 'r':
            red |= 0xFF;
            break;
        default:
            return NULL;
            break;
        }
    }

    struct bmp_image *copy = copy_bmp(image);
    CHECK_NULL(copy);

    uint32_t w = copy->header->width;
    uint32_t h = copy->header->height;

    // pointer arithmetic
    struct pixel *pixel = copy->data;
    for (uint32_t i = 0; i < w * h; i++, pixel++)
    {
        pixel->blue &= blue;
        pixel->green &= green;
        pixel->red &= red;
    }
    return copy;
}
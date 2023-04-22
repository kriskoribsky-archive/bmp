#include <stdlib.h>

#include "transformations.h"
#include "bmp.h"

struct bmp_image *flip_horizontally(const struct bmp_image *image)
{
    CHECK_NULL(image);

    struct bmp_image *copy = copy_bmp(image);

    uint32_t width = image->header->width;
    uint32_t height = image->header->height;

    // flipping horizontally means flipping along vertical axis
    for (uint32_t left_col = 0, right_col = width - 1; left_col < right_col; left_col++, right_col--)
    {
        for (uint32_t row = 0; row < height; row++)
        {
            uint32_t offset = row * width;

            struct pixel tmp = copy->data[offset + left_col];
            copy->data[offset + left_col] = copy->data[offset + right_col];
            copy->data[offset + right_col] = tmp;
        }
    }

    return copy;
}
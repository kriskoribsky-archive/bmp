#include <stdlib.h>
#include <string.h>

#include "transformations.h"
#include "bmp.h"

struct bmp_image *flip_horizontally(const struct bmp_image *image)
{
    CHECK_NULL(image);

    struct bmp_image *copy = copy_bmp(image);

    uint32_t width = copy->header->width;
    uint32_t height = copy->header->height;

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

struct bmp_image *flip_vertically(const struct bmp_image *image)
{
    CHECK_NULL(image);

    struct bmp_image *copy = copy_bmp(image);

    uint32_t width = copy->header->width;
    uint32_t height = copy->header->height;
    size_t bytes = width * sizeof(struct pixel);

    // flipping vertically means flipping along horizontal axis
    // bmp images are stored in bottom to top order
    for (uint32_t botom_row = 0, top_row = height - 1; botom_row < top_row; botom_row++, top_row--)
    {
        struct pixel *tmp = malloc(bytes);
        if (tmp == NULL)
        {
            free_bmp_image(copy);
            return NULL;
        }

        memcpy(tmp, &copy->data[botom_row * width], bytes);
        memcpy(&copy->data[botom_row * width], &copy->data[top_row * width], bytes);
        memcpy(&copy->data[top_row * width], tmp, bytes);

        free(tmp);
    }
    return copy;
}





#include "../unity/src/unity.h"

#include "transformations.h"
#include "bmp.h"

void setUp(void);
void tearDown(void);

void test_right_rotate_header_size1(void);
void test_right_rotate_header_size2(void);
void test_right_rotate_header_size3(void);

void test_right_rotate_image_size1(void);
void test_right_rotate_image_size2(void);
void test_right_rotate_image_size3(void);

void test_left_rotate_header_size1(void);
void test_left_rotate_header_size2(void);
void test_left_rotate_header_size3(void);

void test_left_rotate_image_size1(void);
void test_left_rotate_image_size2(void);
void test_left_rotate_image_size3(void);

void test_crop_new_image_size1(void);
void test_crop_new_image_size2(void);
void test_crop_new_image_size3(void);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_right_rotate_header_size1);
    RUN_TEST(test_right_rotate_header_size2);
    RUN_TEST(test_right_rotate_header_size3);

    RUN_TEST(test_right_rotate_image_size1);
    RUN_TEST(test_right_rotate_image_size2);
    RUN_TEST(test_right_rotate_image_size3);

    RUN_TEST(test_left_rotate_header_size1);
    RUN_TEST(test_left_rotate_header_size2);
    RUN_TEST(test_left_rotate_header_size3);

    RUN_TEST(test_left_rotate_image_size1);
    RUN_TEST(test_left_rotate_image_size2);
    RUN_TEST(test_left_rotate_image_size3);

    RUN_TEST(test_crop_new_image_size1);
    RUN_TEST(test_crop_new_image_size2);
    RUN_TEST(test_crop_new_image_size3);

    return UNITY_END();
}

// TEST RIGHT ROTATE
// ================================================================================

void test_right_rotate_header_size1(void)
{
    FILE *fp = fopen("data/test_right_rotate_header_size1.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(78, rotated_image->header->size);
}

void test_right_rotate_header_size2(void)
{
    FILE *fp = fopen("data/test_right_rotate_header_size2.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(70, rotated_image->header->size);
}

void test_right_rotate_header_size3(void)
{
    FILE *fp = fopen("data/test_right_rotate_header_size3.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(86, rotated_image->header->size);
}

void test_right_rotate_image_size1(void)
{
    FILE *fp = fopen("data/test_right_rotate_image_size1.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(24, rotated_image->header->image_size);
}

void test_right_rotate_image_size2(void)
{
    FILE *fp = fopen("data/test_right_rotate_image_size2.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(16, rotated_image->header->image_size);
}

void test_right_rotate_image_size3(void)
{
    FILE *fp = fopen("data/test_right_rotate_image_size3.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(32, rotated_image->header->image_size);
}

// TEST LEFT ROTATE
// ================================================================================

void test_left_rotate_header_size1(void)
{
    FILE *fp = fopen("data/test_left_rotate_header_size1.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(78, rotated_image->header->size);
}

void test_left_rotate_header_size2(void)
{
    FILE *fp = fopen("data/test_left_rotate_header_size2.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(70, rotated_image->header->size);
}

void test_left_rotate_header_size3(void)
{
    FILE *fp = fopen("data/test_left_rotate_header_size3.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(86, rotated_image->header->size);
}

void test_left_rotate_image_size1(void)
{
    FILE *fp = fopen("data/test_left_rotate_image_size1.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(24, rotated_image->header->image_size);
}

void test_left_rotate_image_size2(void)
{
    FILE *fp = fopen("data/test_left_rotate_image_size2.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(16, rotated_image->header->image_size);
}

void test_left_rotate_image_size3(void)
{
    FILE *fp = fopen("data/test_left_rotate_image_size3.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *rotated_image = rotate_right(image);

    fclose(fp);
    TEST_ASSERT_EQUAL(32, rotated_image->header->image_size);
}

// TEST CROP
// ================================================================================

void test_crop_new_image_size1(void)
{
    FILE *fp = fopen("data/test_crop_new_image_size1.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *cropped_image = crop(image, 0, 0, 1, 2);

    fclose(fp);
    TEST_ASSERT_EQUAL(8, cropped_image->header->image_size);
}

void test_crop_new_image_size2(void)
{
    FILE *fp = fopen("data/test_crop_new_image_size2.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *cropped_image = crop(image, 0, 0, 1, 2);

    fclose(fp);
    TEST_ASSERT_EQUAL(8, cropped_image->header->image_size);
}

void test_crop_new_image_size3(void)
{
    FILE *fp = fopen("data/test_crop_new_image_size3.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);
    struct bmp_image *cropped_image = crop(image, 0, 0, 1, 2);

    fclose(fp);
    TEST_ASSERT_EQUAL(8, cropped_image->header->image_size);
}

void setUp(void)
{
}

void tearDown(void)
{
}
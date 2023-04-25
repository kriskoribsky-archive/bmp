#include "../unity/src/unity.h"

#include "bmp.h"

void setUp(void);
void tearDown(void);

void test_read_bmp_header_null_stream(void);
void test_read_bmp_header_not_bmp_stream(void);

void test_read_bmp_header_valid_bmp_stream(void);

void test_read_bmp_header_correct_width(void);
void test_read_bmp_header_correct_height(void);

void test_read_bmp_header_correct_filesize(void);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_read_bmp_header_null_stream);
    RUN_TEST(test_read_bmp_header_not_bmp_stream);

    RUN_TEST(test_read_bmp_header_valid_bmp_stream);

    RUN_TEST(test_read_bmp_header_correct_width);
    RUN_TEST(test_read_bmp_header_correct_height);

    RUN_TEST(test_read_bmp_header_correct_filesize);

    return UNITY_END();
}

void test_read_bmp_header_null_stream(void)
{
    struct bmp_image *image = read_bmp(NULL);

    TEST_ASSERT_NULL(image);
}

void test_read_bmp_header_not_bmp_stream(void)
{
    FILE *fp = fopen("data/tests/test_read_bmp_header_not_bmp_stream.txt", "rb");
    struct bmp_image *image = read_bmp(fp);

    TEST_ASSERT_NULL(image);
}

void test_read_bmp_header_valid_bmp_stream(void)
{
    FILE *fp = fopen("data/tests/test_read_bmp_header_valid_bmp_stream.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);

    TEST_ASSERT_NOT_NULL(image);
}

void test_read_bmp_header_correct_width(void)
{
    FILE *fp = fopen("data/tests/test_read_bmp_header_correct_width.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);

    TEST_ASSERT_EQUAL(256, image->header->width);
}

void test_read_bmp_header_correct_height(void)
{
    FILE *fp = fopen("data/tests/test_read_bmp_header_correct_height.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);

    TEST_ASSERT_EQUAL(192, image->header->height);
}

void test_read_bmp_header_correct_filesize(void)
{
    FILE *fp = fopen("data/tests/test_read_bmp_header_correct_filesize.bmp", "rb");
    struct bmp_image *image = read_bmp(fp);

    TEST_ASSERT_EQUAL(102, image->header->size);
}

void setUp(void)
{
}

void tearDown(void)
{
}
#include <stdio.h>
#define WIDTH 512
#define HEIGHT 512
#define RGB 3
#define RED 0
#define GREEN 1
#define BLUE 2
#define MAX_COLOR 255
#define MAX_TYPE 4

typedef struct _pixel {
    unsigned short int red;
    unsigned short int green;
    unsigned short int blue;
} Pixel;

typedef struct _image {
    unsigned short int pixel[WIDTH][HEIGHT][RGB];
    unsigned int width;
    unsigned int height;
} Image;

Image gray_scale_filter(Image image) {
    for (unsigned int line = 0; line < image.height; ++line) {
        for (unsigned int column = 0; column < image.width; ++column) {
            int average = image.pixel[line][column][RED] +
                        image.pixel[line][column][GREEN] +
                        image.pixel[line][column][BLUE];
            average /= 3;
            image.pixel[line][column][RED] = average;
            image.pixel[line][column][GREEN] = average;
            image.pixel[line][column][BLUE] = average;
        }
    }

    return image;
}

void blur_filter(unsigned int height, unsigned int width, unsigned short int pixel[WIDTH][HEIGHT][RGB], int blur_size) {
    for (unsigned int line = 0; line < height; ++line) {
        for (unsigned int column = 0; column < width; ++column) {
            Pixel average = {0, 0, 0};

            int small_height = (height - 1 > line + blur_size/2) ? line + blur_size/2 : height - 1;
            int min_width = (width - 1 > column + blur_size/2) ? column + blur_size/2 : width - 1;
            for(int line2 = (0 > line - blur_size/2 ? 0 : line - blur_size/2); line2 <= small_height; ++line2) {
                for(int column2 = (0 > column - blur_size/2 ? 0 : column - blur_size/2); column2 <= min_width; ++column2) {
                    average.red += pixel[line2][column2][RED];
                    average.green += pixel[line2][column2][GREEN];
                    average.blue += pixel[line2][column2][BLUE];
                }
            }

            average.red /= blur_size * blur_size;
            average.green /= blur_size * blur_size;
            average.blue /= blur_size * blur_size;

            pixel[line][column][RED] = average.red;
            pixel[line][column][GREEN] = average.green;
            pixel[line][column][BLUE] = average.blue;
        }
    }
}

Image rotate_right_90_filter(Image image) {
    Image rotation;

    rotation.width = image.height;
    rotation.height = image.width;

    for (unsigned int line = 0, line2 = 0; line < rotation.height; ++line, ++line2) {
        for (int column = rotation.width - 1, column2 = 0; column >= 0; --column, ++column2) {
            rotation.pixel[line][column][RED] = image.pixel[line2][column2][RED];
            rotation.pixel[line][column][GREEN] = image.pixel[line2][column2][GREEN];
            rotation.pixel[line][column][BLUE] = image.pixel[line2][column2][BLUE];
        }
    }

    return rotation;
}

void invert_colors_filter(unsigned short int pixel[WIDTH][HEIGHT][RGB],
                    unsigned int width, unsigned int height) {
    for (unsigned int line = 0; line < height; ++line) {
        for (unsigned int column = 0; column < width; ++column) {
            pixel[line][column][RED] = MAX_COLOR - pixel[line][column][RED];
            pixel[line][column][GREEN] = MAX_COLOR - pixel[line][column][GREEN];
            pixel[line][column][BLUE] = MAX_COLOR - pixel[line][column][BLUE];
        }
    }
}

Image cut_image_filter(Image image, int coord_x, int coord_y, int width, int height) {
    Image cropped;

    cropped.width = width;
    cropped.height = height;

    for(int line = 0; line < height; ++line) {
        for(int column = 0; column < width; ++column) {
            cropped.pixel[line][column][RED] = image.pixel[line + coord_x][column + coord_y][RED];
            cropped.pixel[line][column][GREEN] = image.pixel[line + coord_x][column + coord_y][GREEN];
            cropped.pixel[line][column][BLUE] = image.pixel[line + coord_x][column + coord_y][BLUE];
        }
    }

    return cropped;
}

Image sepia_filter(Image image) {
    for(int line = 0; line < image.height; ++line) {
        for(int column = 0; column < image.width; ++column) {
            unsigned short int pixel[3];
            pixel[RED] = image.pixel[line][column][RED];
            pixel[GREEN] = image.pixel[line][column][GREEN];
            pixel[BLUE] = image.pixel[line][column][BLUE];

            int filter =  pixel[RED] * .393 + pixel[GREEN] * .769 + pixel[BLUE] * .189;
            int small_r = (MAX_COLOR >  filter) ? filter : MAX_COLOR;
            image.pixel[line][column][RED] = small_r;

            filter =  pixel[RED] * .349 + pixel[GREEN] * .686 + pixel[BLUE] * .168;
            small_r = (MAX_COLOR >  filter) ? filter : MAX_COLOR;
            image.pixel[line][column][GREEN] = small_r;

            filter =  pixel[RED] * .272 + pixel[GREEN] * .534 + pixel[BLUE] * .131;
            small_r = (MAX_COLOR >  filter) ? filter : MAX_COLOR;
            image.pixel[line][column][BLUE] = small_r;
        }
    }

    return image;
}

Image mirroring_filter(Image image, int horizontal) {
    int width = image.width, height = image.height;

    if (horizontal == 1) width /= 2;
    else height /= 2;

    for (int line = 0; line < height; ++line) {
        for (int column = 0; column < width; ++column) {
            int coord_x = line, coord_y = column;

            if (horizontal == 1) coord_y = image.width - 1 - column;
            else coord_x = image.height - 1 - line;

            Pixel aux_image;
            aux_image.red = image.pixel[line][column][RED];
            aux_image.green = image.pixel[line][column][GREEN];
            aux_image.blue = image.pixel[line][column][BLUE];

            image.pixel[line][column][RED] = image.pixel[coord_x][coord_y][RED];
            image.pixel[line][column][GREEN] = image.pixel[coord_x][coord_y][GREEN];
            image.pixel[line][column][BLUE] = image.pixel[coord_x][coord_y][BLUE];

            image.pixel[coord_x][coord_y][RED] = aux_image.red;
            image.pixel[coord_x][coord_y][GREEN] = aux_image.green;
            image.pixel[coord_x][coord_y][BLUE] = aux_image.blue;
        }
    }

    return image;
}

Image read_image() {
    Image image;

    // read type of image
    char image_type[MAX_TYPE];
    scanf("%s", image_type);

    // read width height and color of image
    int max_color;
    scanf("%u %u %d", &image.width, &image.height, &max_color);

    // read all pixels of image
    for (unsigned int line = 0; line < image.height; ++line) {
        for (unsigned int column = 0; column < image.width; ++column) {
            scanf("%hu %hu %hu", &image.pixel[line][column][RED],
            &image.pixel[line][column][GREEN],
            &image.pixel[line][column][BLUE]);
        }
    }
    return image;
}

void write_image(Image image) {
    // print type of image
    printf("P3\n");
    // print width height and color of image
    printf("%u %u\n255\n", image.width, image.height);

    // print pixels of image
    for (unsigned int line = 0; line < image.height; ++line) {
        for (unsigned int column = 0; column < image.width; ++column) {
            printf("%hu %hu %hu ", image.pixel[line][column][RED],
                                   image.pixel[line][column][GREEN],
                                   image.pixel[line][column][BLUE]);

        }
        printf("\n");
    }
}

int main() {
    Image image;
    int option_limit;
    int option;

    image = read_image();

    scanf("%d", &option_limit);

    for(int count = 0; count < option_limit; ++count) {
        scanf("%d", &option);
        switch(option) {
            case 1: {
                image = gray_scale_filter(image);
                break;
            }
            case 2: {
                image = sepia_filter(image);
                break;
            }
            case 3: {
                int blur_size;
                scanf("%d", &blur_size);
                blur_filter(image.height, image.width, image.pixel, blur_size);
                break;
            }
            case 4: {
                int rotation_count;
                scanf("%d", &rotation_count);
                rotation_count %= 4;
                for (int count = 0; count < rotation_count; ++count)
                    image = rotate_right_90_filter(image);
                break;
            }
            case 5: {
                int horizontal;
                scanf("%d", &horizontal);
                image = mirroring_filter(image, horizontal);
                break;
            }
            case 6: {
                invert_colors_filter(image.pixel, image.width, image.height);
                break;
            }
            case 7: {
                int coord_x, coord_y, width, height;
                scanf("%d %d %d %d", &coord_x, &coord_y, &width, &height);
                image = cut_image_filter(image, coord_x, coord_y, width, height);
            }
        }
    }

    write_image(image);
    return 0;
}

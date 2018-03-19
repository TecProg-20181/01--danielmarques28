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

int pixel_equal(Pixel pixel1, Pixel pixel2) {
    if (pixel1.red == pixel2.red &&
        pixel1.green == pixel2.green &&
        pixel1.blue == pixel2.blue)
        return 1;
    return 0;
}

Image gray_scale(Image image) {
    /*for (unsigned int i = 0; i < image.h; ++i) {
        for (unsigned int j = 0; j < image.w; ++j) {
            print("%u", image.pixel[i][j][RED] + image.pixel[i][j][GREEN] + image.pixel[i][j][BLUE]);
        }
    }*/

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

void blur(unsigned int height, unsigned short int pixel[WIDTH][HEIGHT][RGB], int blur_size, unsigned int width) {
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

            // printf("%u", average.red)
            average.red /= blur_size * blur_size;
            average.green /= blur_size * blur_size;
            average.blue /= blur_size * blur_size;

            pixel[line][column][RED] = average.red;
            pixel[line][column][GREEN] = average.green;
            pixel[line][column][BLUE] = average.blue;
        }
    }
}

Image rotate_right_90(Image image) {
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

void invert_colors(unsigned short int pixel[WIDTH][HEIGHT][RGB],
                    unsigned int width, unsigned int height) {
    for (unsigned int line = 0; line < height; ++line) {
        for (unsigned int column = 0; column < width; ++column) {
            pixel[line][column][RED] = MAX_COLOR - pixel[line][column][RED];
            pixel[line][column][GREEN] = MAX_COLOR - pixel[line][column][GREEN];
            pixel[line][column][BLUE] = MAX_COLOR - pixel[line][column][BLUE];
        }
    }
}

Image cut_image(Image image, int coord_x, int coord_y, int width, int height) {
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


int main() {
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

    int option_limit;
    scanf("%d", &option_limit);

    for(int count = 0; count < option_limit; ++count) {
        int option;
        scanf("%d", &option);

        switch(option) {
            case 1: { // Escala de Cinza
                image = gray_scale(image);
                break;
            }
            case 2: { // Filtro Sepia
                for (unsigned int line = 0; line < image.height; ++line) {
                    for (unsigned int column = 0; column < image.width; ++column) {
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

                break;
            }
            case 3: { // Blur
                int blur_size = 0;
                scanf("%d", &blur_size);
                blur(image.height, image.pixel, blur_size, image.width);
                break;
            }
            case 4: { // Rotacao
                int rotation_count = 0;
                scanf("%d", &rotation_count);
                rotation_count %= 4;
                for (int count = 0; count < rotation_count; ++count) {
                    image = rotate_right_90(image);
                }
                break;
            }
            case 5: { // Espelhamento
                int horizontal = 0;
                scanf("%d", &horizontal);

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
                break;
            }
            case 6: { // Inversao de Cores
                invert_colors(image.pixel, image.width, image.height);
                break;
            }
            case 7: { // Cortar Imagem
                int coord_x, coord_y;
                scanf("%d %d", &coord_x, &coord_y);
                int width, height;
                scanf("%d %d", &width, &height);

                image = cut_image(image, coord_x, coord_y, width, height);
                break;
            }
        }

    }

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
    return 0;
}

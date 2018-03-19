#include <stdio.h>

typedef struct _pixel {
    unsigned short int red;
    unsigned short int green;
    unsigned short int blue;
} Pixel;

typedef struct _image {
    // [width][height][rgb]
    // 0 -> r
    // 1 -> g
    // 2 -> b
    unsigned short int pixel[512][512][3];
    unsigned int width;
    unsigned int height;
} Image;

int pixel_igual(Pixel pixel1, Pixel pixel2) {
    if (pixel1.red == pixel2.red &&
        pixel1.green == pixel2.green &&
        pixel1.blue == pixel2.blue)
        return 1;
    return 0;
}

Image escala_de_cinza(Image image) {
    /*for (unsigned int i = 0; i < image.h; ++i) {
        for (unsigned int j = 0; j < image.w; ++j) {
            print("%u", image.pixel[i][j][0] + image.pixel[i][j][1] + image.pixel[i][j][2]);
        }
    }*/

    for (unsigned int line = 0; line < image.height; ++line) {
        for (unsigned int column = 0; column < image.width; ++column) {
            int average = image.pixel[line][column][0] +
                        image.pixel[line][column][1] +
                        image.pixel[line][column][2];
            average /= 3;
            image.pixel[line][column][0] = average;
            image.pixel[line][column][1] = average;
            image.pixel[line][column][2] = average;
        }
    }

    return image;
}

void blur(unsigned int height, unsigned short int pixel[512][512][3], int blur_size, unsigned int width) {
    for (unsigned int line = 0; line < height; ++line) {
        for (unsigned int column = 0; column < width; ++column) {
            Pixel average = {0, 0, 0};

            int small_height = (height - 1 > line + blur_size/2) ? line + blur_size/2 : height - 1;
            int min_width = (width - 1 > column + blur_size/2) ? column + blur_size/2 : width - 1;
            for(int line2 = (0 > line - blur_size/2 ? 0 : line - blur_size/2); line2 <= small_height; ++line2) {
                for(int column2 = (0 > column - blur_size/2 ? 0 : column - blur_size/2); column2 <= min_width; ++column2) {
                    average.red += pixel[line2][column2][0];
                    average.green += pixel[line2][column2][1];
                    average.blue += pixel[line2][column2][2];
                }
            }

            // printf("%u", average.red)
            average.red /= blur_size * blur_size;
            average.green /= blur_size * blur_size;
            average.blue /= blur_size * blur_size;

            pixel[line][column][0] = average.red;
            pixel[line][column][1] = average.green;
            pixel[line][column][2] = average.blue;
        }
    }
}

Image rotacionar90direita(Image image) {
    Image rotation;

    rotation.width = image.height;
    rotation.height = image.width;

    for (unsigned int line = 0, line2 = 0; line < rotation.height; ++line, ++line2) {
        for (int column = rotation.width - 1, column2 = 0; column >= 0; --column, ++column2) {
            rotation.pixel[line][column][0] = image.pixel[line2][column2][0];
            rotation.pixel[line][column][1] = image.pixel[line2][column2][1];
            rotation.pixel[line][column][2] = image.pixel[line2][column2][2];
        }
    }

    return rotation;
}

void inverter_cores(unsigned short int pixel[512][512][3],
                    unsigned int width, unsigned int height) {
    for (unsigned int line = 0; line < height; ++line) {
        for (unsigned int column = 0; column < width; ++column) {
            pixel[line][column][0] = 255 - pixel[line][column][0];
            pixel[line][column][1] = 255 - pixel[line][column][1];
            pixel[line][column][2] = 255 - pixel[line][column][2];
        }
    }
}

Image cortar_imagem(Image image, int coord_x, int coord_y, int width, int height) {
    Image cropped;

    cropped.width = width;
    cropped.height = height;

    for(int line = 0; line < height; ++line) {
        for(int column = 0; column < width; ++column) {
            cropped.pixel[line][column][0] = image.pixel[line + coord_x][column + coord_y][0];
            cropped.pixel[line][column][1] = image.pixel[line + coord_x][column + coord_y][1];
            cropped.pixel[line][column][2] = image.pixel[line + coord_x][column + coord_y][2];
        }
    }

    return cropped;
}


int main() {
    Image image;

    // read type of image
    char image_type[4];
    scanf("%s", image_type);

    // read width height and color of image
    int max_color;
    scanf("%u %u %d", &image.width, &image.height, &max_color);

    // read all pixels of image
    for (unsigned int line = 0; line < image.height; ++line) {
        for (unsigned int column = 0; column < image.width; ++column) {
            scanf("%hu %hu %hu", &image.pixel[line][column][0],
                                 &image.pixel[line][column][1],
                                 &image.pixel[line][column][2]);

        }
    }

    int option_limit;
    scanf("%d", &option_limit);

    for(int count = 0; count < option_limit; ++count) {
        int option;
        scanf("%d", &option);

        switch(option) {
            case 1: { // Escala de Cinza
                image = escala_de_cinza(image);
                break;
            }
            case 2: { // Filtro Sepia
                for (unsigned int line = 0; line < image.height; ++line) {
                    for (unsigned int column = 0; column < image.width; ++column) {
                        unsigned short int pixel[3];
                        pixel[0] = image.pixel[line][column][0];
                        pixel[1] = image.pixel[line][column][1];
                        pixel[2] = image.pixel[line][column][2];

                        int filter =  pixel[0] * .393 + pixel[1] * .769 + pixel[2] * .189;
                        int small_r = (255 >  filter) ? filter : 255;
                        image.pixel[line][column][0] = small_r;

                        filter =  pixel[0] * .349 + pixel[1] * .686 + pixel[2] * .168;
                        small_r = (255 >  filter) ? filter : 255;
                        image.pixel[line][column][1] = small_r;

                        filter =  pixel[0] * .272 + pixel[1] * .534 + pixel[2] * .131;
                        small_r = (255 >  filter) ? filter : 255;
                        image.pixel[line][column][2] = small_r;
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
                    image = rotacionar90direita(image);
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
                        aux_image.red = image.pixel[line][column][0];
                        aux_image.green = image.pixel[line][column][1];
                        aux_image.blue = image.pixel[line][column][2];

                        image.pixel[line][column][0] = image.pixel[coord_x][coord_y][0];
                        image.pixel[line][column][1] = image.pixel[coord_x][coord_y][1];
                        image.pixel[line][column][2] = image.pixel[coord_x][coord_y][2];

                        image.pixel[coord_x][coord_y][0] = aux_image.red;
                        image.pixel[coord_x][coord_y][1] = aux_image.green;
                        image.pixel[coord_x][coord_y][2] = aux_image.blue;
                    }
                }
                break;
            }
            case 6: { // Inversao de Cores
                inverter_cores(image.pixel, image.width, image.height);
                break;
            }
            case 7: { // Cortar Imagem
                int coord_x, coord_y;
                scanf("%d %d", &coord_x, &coord_y);
                int width, height;
                scanf("%d %d", &width, &height);

                image = cortar_imagem(image, coord_x, coord_y, width, height);
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
            printf("%hu %hu %hu ", image.pixel[line][column][0],
                                   image.pixel[line][column][1],
                                   image.pixel[line][column][2]);

        }
        printf("\n");
    }
    return 0;
}

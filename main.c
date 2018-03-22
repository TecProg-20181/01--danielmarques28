#include <stdio.h>
#define WIDTH 512
#define HEIGHT 512
#define COLOR_LIMIT 255

typedef struct _pixel {
    unsigned short int red;
    unsigned short int green;
    unsigned short int blue;
} Pixel;

typedef struct _image {
    Pixel pixel[WIDTH][HEIGHT];
    unsigned int width;
    unsigned int height;
} Image;

int max(int a, int b) {
    if (a > b)
        return a;
    return b;
}

int minimal(int a, int b) {
    if (a < b)
        return a;
    return b;
}

int pixel_igual(Pixel p1, Pixel p2) {
    if (p1.red == p2.red &&
        p1.green == p2.green &&
        p1.blue == p2.blue)
        return 1;
    return 0;
}

void set_width_height(Image *image, unsigned int widht, unsigned int height) {
    image->width = widht;
    image->height = height;
}

Pixel set_colors_pixel(Pixel *self, Pixel pixel) {
    self->red = pixel.red;
    self->green = pixel.green;
    self->blue = pixel.blue;
}

Image gray_scale_filter(Image image) {
    for (unsigned int i = 0; i < image.height; ++i) {
        for (unsigned int j = 0; j < image.width; ++j) {
            int media = image.pixel[i][j].red +
                        image.pixel[i][j].green +
                        image.pixel[i][j].blue;
            media /= 3;
            image.pixel[i][j].red = media;
            image.pixel[i][j].green = media;
            image.pixel[i][j].blue = media;
        }
    }

    return image;
}

Image blur_filter(Image image, int T) {
    for (unsigned int i = 0; i < image.height; ++i) {
        for (unsigned int j = 0; j < image.width; ++j) {
            Pixel media = {0, 0, 0};

            int min_height = minimal(i + T/2, image.height - 1);
            int min_width = minimal(j + T/2, image.width - 1);
            for(int x = (0 > i - T/2 ? 0 : i - T/2); x <= min_height; ++x) {
                for(int y = (0 > j - T/2 ? 0 : j - T/2); y <= min_width; ++y) {
                    media.red += image.pixel[x][y].red;
                    media.green += image.pixel[x][y].green;
                    media.blue += image.pixel[x][y].blue;
                }
            }

            media.red /= T * T;
            media.green /= T * T;
            media.blue /= T * T;

            set_colors_pixel(&image.pixel[i][j], media);
        }
    }

    return image;
}

Image rotation90_filter(Image image) {
    Image rotated;

    set_width_height(&rotated, image.width, image.height);

    for (unsigned int i = 0, y = 0; i < rotated.height; ++i, ++y) {
        for (int j = rotated.width - 1, x = 0; j >= 0; --j, ++x) {
            set_colors_pixel(&rotated.pixel[i][j], image.pixel[x][y]);
        }
    }

    return rotated;
}

Image reverse_colors_filter(Image image) {
    for (unsigned int i = 0; i < image.height; ++i) {
        for (unsigned int j = 0; j < image.width; ++j) {
            image.pixel[i][j].red = COLOR_LIMIT - image.pixel[i][j].red;
            image.pixel[i][j].green = COLOR_LIMIT - image.pixel[i][j].green;
            image.pixel[i][j].blue = COLOR_LIMIT - image.pixel[i][j].blue;
        }
    }

    return image;
}

Image cut_filter(Image image, int x, int y, int width, int height) {
    Image cut_image;

    set_width_height(&cut_image, width, height);

    for(int i = 0; i < height; ++i) {
        for(int j = 0; j < width; ++j) {
            set_colors_pixel(&cut_image.pixel[i][j], image.pixel[i + y][j + x]);
        }
    }

    return cut_image;
}

Image sepia_filter(Image image) {
    for (unsigned int x = 0; x < image.height; ++x) {
        for (unsigned int j = 0; j < image.width; ++j) {
            unsigned short int pixel[3];
            pixel[0] = image.pixel[x][j].red;
            pixel[1] = image.pixel[x][j].green;
            pixel[2] = image.pixel[x][j].blue;

            int p =  pixel[0] * .393 + pixel[1] * .769 + pixel[2] * .189;
            int small_r = minimal(p, COLOR_LIMIT);
            image.pixel[x][j].red = small_r;

            p =  pixel[0] * .349 + pixel[1] * .686 + pixel[2] * .168;
            small_r = minimal(p, COLOR_LIMIT);
            image.pixel[x][j].green = small_r;

            p =  pixel[0] * .272 + pixel[1] * .534 + pixel[2] * .131;
            small_r = minimal(p, COLOR_LIMIT);
            image.pixel[x][j].blue = small_r;
        }
    }

    return image;
}

Image mirror_filter(Image image) {
    int horizontal = 0;
    int width = image.width, height = image.height;

    scanf("%d", &horizontal);

    if (horizontal == 1) width /= 2;
    else height /= 2;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int x = i, y = j;

            if (horizontal == 1)
                y = image.width - 1 - j;
            else
                x = image.height - 1 - i;

            Pixel temp;
            set_colors_pixel(&temp, image.pixel[i][j]);
            set_colors_pixel(&image.pixel[i][j], image.pixel[x][y]);
            set_colors_pixel(&image.pixel[x][y], temp);
        }
    }

    return image;
}

Image read_image(Image image) {
    int max_color;
    char image_type[4];

    // read type of image
    scanf("%s", image_type);

    // read width height and color of image
    scanf("%u %u %d", &image.width, &image.height, &max_color);

    // read all pixels of image
    for (unsigned int i = 0; i < image.height; ++i) {
        for (unsigned int j = 0; j < image.width; ++j) {
            scanf("%hu %hu %hu", &image.pixel[i][j].red,
                                 &image.pixel[i][j].green,
                                 &image.pixel[i][j].blue);

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
    for (unsigned int i = 0; i < image.height; ++i) {
        for (unsigned int j = 0; j < image.width; ++j) {
            printf("%hu %hu %hu ", image.pixel[i][j].red,
                                   image.pixel[i][j].green,
                                   image.pixel[i][j].blue);

        }
        printf("\n");
    }
}

int main() {
    Image image;
    int limit;

    image = read_image(image);

    scanf("%d", &limit);

    for(int i = 0; i < limit; ++i) {
        int option;
        scanf("%d", &option);

        switch(option) {
            case 1: { // Escala de Cinza
                image = gray_scale_filter(image);
                break;
            }
            case 2: { // Filtro Sepia
                image = sepia_filter(image);
                break;
            }
            case 3: { // Blur
                int tamanho = 0;
                scanf("%d", &tamanho);
                image = blur_filter(image, tamanho);
                break;
            }
            case 4: { // Rotacao
                int quantas_vezes = 0;
                scanf("%d", &quantas_vezes);
                quantas_vezes %= 4;
                for (int j = 0; j < quantas_vezes; ++j) {
                    image = rotation90_filter(image);
                }
                break;
            }
            case 5: { // Espelhamento
                image = mirror_filter(image);
                break;
            }
            case 6: { // Inversao de Cores
                image = reverse_colors_filter(image);
                break;
            }
            case 7: { // Cortar Imagem
                int x, y;
                scanf("%d %d", &x, &y);
                int width, height;
                scanf("%d %d", &width, &height);

                image = cut_filter(image, x, y, width, height);
                break;
            }
        }
    }

    write_image(image);

    return 0;
}

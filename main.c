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

Image escala_de_cinza(Image img) {
    for (unsigned int i = 0; i < img.height; ++i) {
        for (unsigned int j = 0; j < img.width; ++j) {
            int media = img.pixel[i][j].red +
                        img.pixel[i][j].green +
                        img.pixel[i][j].blue;
            media /= 3;
            img.pixel[i][j].red = media;
            img.pixel[i][j].green = media;
            img.pixel[i][j].blue = media;
        }
    }

    return img;
}

Image blur(Image image, int T) {
    for (unsigned int i = 0; i < image.height; ++i) {
        for (unsigned int j = 0; j < image.width; ++j) {
            Pixel media = {0, 0, 0};

            int menor_h = minimal(i + T/2, image.height - 1);
            int min_w = minimal(j + T/2, image.width - 1);
            for(int x = (0 > i - T/2 ? 0 : i - T/2); x <= menor_h; ++x) {
                for(int y = (0 > j - T/2 ? 0 : j - T/2); y <= min_w; ++y) {
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

Image rotacionar90direita(Image img) {
    Image rotacionada;

    set_width_height(&rotacionada, img.width, img.height);

    for (unsigned int i = 0, y = 0; i < rotacionada.height; ++i, ++y) {
        for (int j = rotacionada.width - 1, x = 0; j >= 0; --j, ++x) {
            set_colors_pixel(&rotacionada.pixel[i][j], img.pixel[x][y]);
        }
    }

    return rotacionada;
}

Image inverter_cores(Image image) {
    for (unsigned int i = 0; i < image.height; ++i) {
        for (unsigned int j = 0; j < image.width; ++j) {
            image.pixel[i][j].red = COLOR_LIMIT - image.pixel[i][j].red;
            image.pixel[i][j].green = COLOR_LIMIT - image.pixel[i][j].green;
            image.pixel[i][j].blue = COLOR_LIMIT - image.pixel[i][j].blue;
        }
    }

    return image;
}

Image cortar_imagem(Image img, int x, int y, int w, int h) {
    Image cortada;

    set_width_height(&cortada, w, h);

    for(int i = 0; i < h; ++i) {
        for(int j = 0; j < w; ++j) {
            set_colors_pixel(&cortada.pixel[i][j], img.pixel[i + y][j + x]);
        }
    }

    return cortada;
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
    int w = image.width, h = image.height;

    scanf("%d", &horizontal);

    if (horizontal == 1) w /= 2;
    else h /= 2;

    for (int i = 0; i < h; ++i) {
        for (int j = 0; j < w; ++j) {
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
    char p3[4];

    // read type of image
    scanf("%s", p3);

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
    Image img;
    int n_opcoes;

    img = read_image(img);

    scanf("%d", &n_opcoes);

    for(int i = 0; i < n_opcoes; ++i) {
        int opcao;
        scanf("%d", &opcao);

        switch(opcao) {
            case 1: { // Escala de Cinza
                img = escala_de_cinza(img);
                break;
            }
            case 2: { // Filtro Sepia
                img = sepia_filter(img);
                break;
            }
            case 3: { // Blur
                int tamanho = 0;
                scanf("%d", &tamanho);
                img = blur(img, tamanho);
                break;
            }
            case 4: { // Rotacao
                int quantas_vezes = 0;
                scanf("%d", &quantas_vezes);
                quantas_vezes %= 4;
                for (int j = 0; j < quantas_vezes; ++j) {
                    img = rotacionar90direita(img);
                }
                break;
            }
            case 5: { // Espelhamento
                img = mirror_filter(img);
                break;
            }
            case 6: { // Inversao de Cores
                img = inverter_cores(img);
                break;
            }
            case 7: { // Cortar Imagem
                int x, y;
                scanf("%d %d", &x, &y);
                int w, h;
                scanf("%d %d", &w, &h);

                img = cortar_imagem(img, x, y, w, h);
                break;
            }
        }
    }

    write_image(img);

    return 0;
}

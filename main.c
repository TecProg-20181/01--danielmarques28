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

int pixel_igual(Pixel p1, Pixel p2) {
    if (p1.red == p2.red &&
        p1.green == p2.green &&
        p1.blue == p2.blue)
        return 1;
    return 0;
}


Image escala_de_cinza(Image img) {
    /*for (unsigned int i = 0; i < img.height; ++i) {
        for (unsigned int j = 0; j < img.width; ++j) {
            print("%u", img.pixel[i][j].red + img.pixel[i][j].green + img.pixel[i][j].blue);
        }
    }*/

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

            int menor_h = (image.height - 1 > i + T/2) ? i + T/2 : image.height - 1;
            int min_w = (image.width - 1 > j + T/2) ? j + T/2 : image.width - 1;
            for(int x = (0 > i - T/2 ? 0 : i - T/2); x <= menor_h; ++x) {
                for(int y = (0 > j - T/2 ? 0 : j - T/2); y <= min_w; ++y) {
                    media.red += image.pixel[x][y].red;
                    media.green += image.pixel[x][y].green;
                    media.blue += image.pixel[x][y].blue;
                }
            }

            // printf("%u", media.red)
            media.red /= T * T;
            media.green /= T * T;
            media.blue /= T * T;

            image.pixel[i][j].red = media.red;
            image.pixel[i][j].green = media.green;
            image.pixel[i][j].blue = media.blue;
        }
    }

    return image;
}

Image rotacionar90direita(Image img) {
    Image rotacionada;

    rotacionada.width = img.height;
    rotacionada.height = img.width;

    for (unsigned int i = 0, y = 0; i < rotacionada.height; ++i, ++y) {
        for (int j = rotacionada.width - 1, x = 0; j >= 0; --j, ++x) {
            rotacionada.pixel[i][j].red = img.pixel[x][y].red;
            rotacionada.pixel[i][j].green = img.pixel[x][y].green;
            rotacionada.pixel[i][j].blue = img.pixel[x][y].blue;
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

    cortada.width = w;
    cortada.height = h;

    for(int i = 0; i < h; ++i) {
        for(int j = 0; j < w; ++j) {
            cortada.pixel[i][j].red = img.pixel[i + y][j + x].red;
            cortada.pixel[i][j].green = img.pixel[i + y][j + x].green;
            cortada.pixel[i][j].blue = img.pixel[i + y][j + x].blue;
        }
    }

    return cortada;
}


int main() {
    Image img;

    // read type of image
    char p3[4];
    scanf("%s", p3);

    // read width height and color of image
    int max_color;
    scanf("%u %u %d", &img.width, &img.height, &max_color);

    // read all pixels of image
    for (unsigned int i = 0; i < img.height; ++i) {
        for (unsigned int j = 0; j < img.width; ++j) {
            scanf("%hu %hu %hu", &img.pixel[i][j].red,
                                 &img.pixel[i][j].green,
                                 &img.pixel[i][j].blue);

        }
    }

    int n_opcoes;
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
                for (unsigned int x = 0; x < img.height; ++x) {
                    for (unsigned int j = 0; j < img.width; ++j) {
                        unsigned short int pixel[3];
                        pixel[0] = img.pixel[x][j].red;
                        pixel[1] = img.pixel[x][j].green;
                        pixel[2] = img.pixel[x][j].blue;

                        int p =  pixel[0] * .393 + pixel[1] * .769 + pixel[2] * .189;
                        int menor_r = (COLOR_LIMIT >  p) ? p : COLOR_LIMIT;
                        img.pixel[x][j].red = menor_r;

                        p =  pixel[0] * .349 + pixel[1] * .686 + pixel[2] * .168;
                        menor_r = (COLOR_LIMIT >  p) ? p : COLOR_LIMIT;
                        img.pixel[x][j].green = menor_r;

                        p =  pixel[0] * .272 + pixel[1] * .534 + pixel[2] * .131;
                        menor_r = (COLOR_LIMIT >  p) ? p : COLOR_LIMIT;
                        img.pixel[x][j].blue = menor_r;
                    }
                }

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
                int horizontal = 0;
                scanf("%d", &horizontal);

                int w = img.width, h = img.height;

                if (horizontal == 1) w /= 2;
                else h /= 2;

                for (int i2 = 0; i2 < h; ++i2) {
                    for (int j = 0; j < w; ++j) {
                        int x = i2, y = j;

                        if (horizontal == 1) y = img.width - 1 - j;
                        else x = img.height - 1 - i2;

                        Pixel aux1;
                        aux1.red = img.pixel[i2][j].red;
                        aux1.green = img.pixel[i2][j].green;
                        aux1.blue = img.pixel[i2][j].blue;

                        img.pixel[i2][j].red = img.pixel[x][y].red;
                        img.pixel[i2][j].green = img.pixel[x][y].green;
                        img.pixel[i2][j].blue = img.pixel[x][y].blue;

                        img.pixel[x][y].red = aux1.red;
                        img.pixel[x][y].green = aux1.green;
                        img.pixel[x][y].blue = aux1.blue;
                    }
                }
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

    // print type of image
    printf("P3\n");
    // print width height and color of image
    printf("%u %u\n255\n", img.width, img.height);

    // print pixels of image
    for (unsigned int i = 0; i < img.height; ++i) {
        for (unsigned int j = 0; j < img.width; ++j) {
            printf("%hu %hu %hu ", img.pixel[i][j].red,
                                   img.pixel[i][j].green,
                                   img.pixel[i][j].blue);

        }
        printf("\n");
    }
    return 0;
}

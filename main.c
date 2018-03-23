#include <stdio.h>
#define WIDTH 512
#define HEIGHT 512
#define COLOR_LIMIT 255
#define RED 0
#define GREEN 1
#define BLUE 2
#define RGB 3

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

int maximum(int a, int b) {
    if (a > b)
        return a;
    return b;
}

int minimal(int a, int b) {
    if (a < b)
        return a;
    return b;
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

Image blur_filter(Image image, unsigned int blur_size) {
    for (unsigned int i = 0; i < image.height; ++i) {
        for (unsigned int j = 0; j < image.width; ++j) {
            Pixel media = {0, 0, 0};

            int min_height = minimal(i + blur_size/2, image.height - 1);
            int min_width = minimal(j + blur_size/2, image.width - 1);
            for(unsigned int x = maximum(0, i - blur_size/2); x <= min_height; ++x) {
                for(unsigned int y = maximum(0, j - blur_size/2); y <= min_width; ++y) {
                    media.red += image.pixel[x][y].red;
                    media.green += image.pixel[x][y].green;
                    media.blue += image.pixel[x][y].blue;
                }
            }

            unsigned int square = blur_size * blur_size;
            media.red /= square;
            media.green /= square;
            media.blue /= square;

            image.pixel[i][j].red = media.red;
            image.pixel[i][j].green = media.green;
            image.pixel[i][j].blue = media.blue;
        }
    }

    return image;
}

Image rotation90_filter(Image image) {
    Image rotated;

    rotated.width = image.width;
    rotated.height = image.height;

    for (unsigned int i = 0, y = 0; i < rotated.height; ++i, ++y) {
        for (int j = rotated.width - 1, x = 0; j >= 0; --j, ++x) {
            rotated.pixel[i][j].red = image.pixel[x][y].red;
            rotated.pixel[i][j].green = image.pixel[x][y].green;
            rotated.pixel[i][j].blue = image.pixel[x][y].blue;
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

Image cut_filter(Image image, unsigned int x,
                 unsigned int y, unsigned int width,
                 unsigned int height) {
    Image cut_image;

    cut_image.width = width;
    cut_image.height = height;

    for(unsigned int i = 0; i < height; ++i) {
        for(unsigned int j = 0; j < width; ++j) {
            cut_image.pixel[i][j].red = image.pixel[i + y][j + x].red;
            cut_image.pixel[i][j].green = image.pixel[i + y][j + x].green;
            cut_image.pixel[i][j].blue = image.pixel[i + y][j + x].blue;
        }
    }

    return cut_image;
}

Image sepia_filter(Image image) {
    for (unsigned int x = 0; x < image.height; ++x) {
        for (unsigned int j = 0; j < image.width; ++j) {
            unsigned short int pixel[RGB];
            pixel[RED] = image.pixel[x][j].red;
            pixel[GREEN] = image.pixel[x][j].green;
            pixel[BLUE] = image.pixel[x][j].blue;

            unsigned int p =  pixel[RED] * .393 + pixel[GREEN] * .769 + pixel[BLUE] * .189;
            unsigned int smaller_color = minimal(p, COLOR_LIMIT);
            image.pixel[x][j].red = smaller_color;

            p =  pixel[RED] * .349 + pixel[GREEN] * .686 + pixel[BLUE] * .168;
            smaller_color = minimal(p, COLOR_LIMIT);
            image.pixel[x][j].green = smaller_color;

            p =  pixel[RED] * .272 + pixel[GREEN] * .534 + pixel[BLUE] * .131;
            smaller_color = minimal(p, COLOR_LIMIT);
            image.pixel[x][j].blue = smaller_color;
        }
    }

    return image;
}

Image mirror_filter(Image image) {
    int horizontal = 0;
    int width = image.width, height = image.height;
    Pixel temp;

    scanf("%d", &horizontal);

    if (horizontal == 1)
        width /= 2;
    else
        height /= 2;

    for (int i = 0; i < height; ++i) {
        for (int j = 0; j < width; ++j) {
            int x = i, y = j;

            if (horizontal == 1)
                y = image.width - 1 - j;
            else
                x = image.height - 1 - i;

            temp.red = image.pixel[i][j].red;
            temp.green = image.pixel[i][j].green;
            temp.blue = image.pixel[i][j].blue;

            image.pixel[i][j].red = image.pixel[x][y].red;
            image.pixel[i][j].green = image.pixel[x][y].green;
            image.pixel[i][j].blue = image.pixel[x][y].blue;

            image.pixel[x][y].red = temp.red;
            image.pixel[x][y].green = temp.green;
            image.pixel[x][y].blue = temp.blue;
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
    int limit, option, x, y, width, height;

    image = read_image(image);

    scanf("%d", &limit);
    for(unsigned int i = 0; i < limit; ++i) {
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
                unsigned blur_size;
                scanf("%u", &blur_size);
                image = blur_filter(image, blur_size);
                break;
            }
            case 4: {
                unsigned int count;
                scanf("%u", &count);
                count %= 4;
                for (unsigned int j = 0; j < count; ++j) {
                    image = rotation90_filter(image);
                }
                break;
            }
            case 5: {
                image = mirror_filter(image);
                break;
            }
            case 6: {
                image = reverse_colors_filter(image);
                break;
            }
            case 7: {
                scanf("%u %u %u %u", &x, &y, &width, &height);
                image = cut_filter(image, x, y, width, height);
            }
        }
    }

    write_image(image);

    return 0;
}

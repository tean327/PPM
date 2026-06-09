#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct
{
    unsigned char r, g, b;
} Pixel;

typedef struct
{
    int width;
    int height;
    int ppmValue;
    int maxValue;
    Pixel **data;
} Image;

void (*saveFunc)(Image *img, const char *filename);

Image *loadPPM(const char *filename);
Image *loadP3ppm(FILE *file, Image *Image);
Image *loadP6ppm(FILE *file, Image *Image);
void saveP6_PPM(Image *img, const char *filename);
void saveP3_PPM(Image *img, const char *filename);
int CheckPPM(FILE *file);
void Invert(Image *img);
void Grey(Image *Image);
void Sepia(Image *image);
void Reflect(Image *image);
void Edges(Image *image);

int main(void)
{
    printf("Enter the name of the file (without the extension): ");
    char *fileName = (char *)malloc(sizeof(char) * 25);
    fgets(fileName, 25, stdin);
    fileName[strcspn(fileName, "\n")] = 0;
    strcat(fileName, ".ppm");
    Image *ppmImage = loadPPM(fileName);

    saveFunc = ppmImage->ppmValue == 0 ? saveP3_PPM : saveP6_PPM;
    int mode = 1;
    while (mode != 0)
    {
        printf("What mode do you want to be \n Press 1 to invert \n Press 2 to put it in grey \n Press 3 to put in sepia\n Press 4 to put reflect the image horizontaly\n Press 5 to put in Green\n Press 6 to put in Blue\n Else Press 0 to quit\n");
        scanf("%d", &mode);
        getchar();
        switch (mode)
        {
        case 1:
            Invert(ppmImage);
            break;
        case 2:
            Grey(ppmImage);
            break;
        case 3:
            Sepia(ppmImage);
            break;
        case 4:
            Reflect(ppmImage);
            break;
        case 5:
            Edges(ppmImage);
            break;
        case 6:
            break;
        }
        saveFunc(ppmImage, "New.ppm");
    }

    for (int i = 0; i < ppmImage->height; i++)
    {
        free(ppmImage->data[i]);
    }

    free(fileName);
    free(ppmImage->data);
    free(ppmImage);
}

Image *loadPPM(const char *filename)
{
    Image *lImage = (Image *)malloc(sizeof(Image));
    if (!lImage)
    {
        printf("Could not allocate memory for a new image\n");
        exit(1);
    }
    FILE *file = fopen(filename, "rb");
    if (!file)
    {
        printf("File could not be oppened\n");
        exit(1);
    }

    // Vérifie si le fichier commence par P6 ou P3
    lImage->ppmValue = CheckPPM(file);
    if (lImage->ppmValue == -1)
    {
        fclose(file);
        printf("Not a ppm file\n");
        exit(1);
    }

    int c;

    while ((c = fgetc(file)) == '#')
        while (fgetc(file) != '\n')
            ;

    ungetc(c, file);

    if (fscanf(file, "%d %d", &lImage->width, &lImage->height) != 2)
    {
        printf("he2re");
        fclose(file);
        exit(1);
    }

    fscanf(file, "%d", &lImage->maxValue);
    fgetc(file);

    lImage->data = malloc(lImage->height * sizeof(Pixel *));
    if (!lImage->data)
    {
        fclose(file);
        printf("Could not allocate memory for the data\n");
        exit(1);
    }

    for (int i = 0; i < lImage->height; i++)
    {
        lImage->data[i] = malloc(lImage->width * sizeof(Pixel));
        if (!lImage->data[i])
        {
            printf("Could not allocate memory for a Pixel\n");
            exit(1);
        }
    }

    if (lImage->ppmValue == 0)
        return loadP3ppm(file, lImage);

    return loadP6ppm(file, lImage);
}

Image *loadP3ppm(FILE *file, Image *Image)
{
    for (int i = 0; i < Image->height; i++)
    {
        for (int j = 0; j < Image->width; j++)
        {
            int rgb[3];
            if (fscanf(file, "%d %d %d ", &rgb[0], &rgb[1], &rgb[2]) != 3)
            {
                fclose(file);
                printf("ERROR IN FILE\n");
                exit(1);
            }
            Image->data[i][j].r = (unsigned char)rgb[0];
            Image->data[i][j].g = (unsigned char)rgb[1];
            Image->data[i][j].b = (unsigned char)rgb[2];
        }
    }
    fclose(file);
    return Image;
}

Image *loadP6ppm(FILE *file, Image *Image)
{
    for (int i = 0; i < Image->height; i++)
    {
        for (int j = 0; j < Image->width; j++)
        {
            unsigned char rgb[3];
            if (fread(rgb, sizeof(unsigned char), 3, file) != 3)
            {
                fclose(file);
                printf("ERROR IN FILE\n");
                exit(1);
            }
            Image->data[i][j].r = rgb[0];
            Image->data[i][j].g = rgb[1];
            Image->data[i][j].b = rgb[2];
        }
    }
    fclose(file);
    return Image;
}

void Invert(Image *img)
{
    for (int i = 0; i < img->height; i++)
    {
        for (int j = 0; j < img->width; j++)
        {
            img->data[i][j].r = 255 - img->data[i][j].r;
            img->data[i][j].g = 255 - img->data[i][j].g;
            img->data[i][j].b = 255 - img->data[i][j].b;
        }
    }
}

void Sepia(Image *image)
{
    for (int i = 0; i < image->height; i++)
    {
        for (int j = 0; j < image->width; j++)
        {
            int originalRed = image->data[i][j].r;
            int originalGreen = image->data[i][j].g;
            int originalBlue = image->data[i][j].b;
            int sepiaRed = (int)((float)(.393 * originalRed + .769 * originalGreen + .189 * originalBlue));
            int sepiaGreen = (int)((float)(.349 * originalRed + .686 * originalGreen + .168 * originalBlue));
            int sepiaBlue = (int)((float)(.272 * originalRed + .534 * originalGreen + .131 * originalBlue));

            sepiaRed = sepiaRed > 255 ? 255 : sepiaRed;
            sepiaGreen = sepiaGreen > 255 ? 255 : sepiaGreen;
            sepiaBlue = sepiaBlue > 255 ? 255 : sepiaBlue;
            image->data[i][j].r = sepiaRed;
            image->data[i][j].g = sepiaGreen;
            image->data[i][j].b = sepiaBlue;
        }
    }
}

void Grey(Image *Image)
{
    int AverageRGB;
    for (int i = 0; i < Image->height; i++)
    {
        for (int j = 0; j < Image->width; j++)
        {
            AverageRGB = (int)((float)(Image->data[i][j].r + Image->data[i][j].g + Image->data[i][j].b) / 3);
            Image->data[i][j].r =
                Image->data[i][j].g =
                    Image->data[i][j].b = AverageRGB;
        }
    }
}

void Reflect(Image *image)
{
    for (int i = 0; i < image->height; i++)
    {
        for (int j = 0; j < image->width / 2; j++)
        {
            Pixel tmp = image->data[i][image->width - j - 1];
            image->data[i][image->width - j - 1] = image->data[i][j];
            image->data[i][j] = tmp;
        }
    }
}

void Edges(Image *image)
{
    // TODO
}

void saveP6_PPM(Image *img, const char *filename)
{
    FILE *file = fopen(filename, "wb");
    if (!file)
    {
        printf("File could not be oppened\n");
        return;
    }

    fprintf(file, "P6\n%d %d\n%d\n", img->width, img->height, img->maxValue);

    for (int i = 0; i < img->height; i++)
    {
        for (int j = 0; j < img->width; j++)
        {
            fwrite(&img->data[i][j], sizeof(unsigned char), 3, file);
        }
    }
    fclose(file);
}

void saveP3_PPM(Image *img, const char *filename)
{
    FILE *file = fopen(filename, "w");
    if (!file)
    {
        printf("File could not be oppened\n");
        return;
    }

    fprintf(file, "P3\n%d %d\n%d\n", img->width, img->height, img->maxValue);

    for (int i = 0; i < img->height; i++)
    {
        for (int j = 0; j < img->width; j++)
        {
            fprintf(file, "%d %d %d ", (int)img->data[i][j].r, (int)img->data[i][j].g, (int)img->data[i][j].b);
        }
    }
    fclose(file);
}

int CheckPPM(FILE *file)
{
    char buffer[3];
    fgets(buffer, 3, file);
    fgetc(file);
    if (strcmp(buffer, "P3") == 0)
        return 0;
    if (strcmp(buffer, "P6") == 0)
        return 1;
    return -1;
}
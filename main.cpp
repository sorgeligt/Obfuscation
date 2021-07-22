#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <windows.h>

int top = -1;
char img_name[30], img_crypt_name[30], txt_name[30];

void finish(const char warning[]) {
    printf("%s", warning);
    exit(0);
}

void move_input(FILE* f, int n) {
    for (int i = 0; i < n; i++) fgetc(f);
}

void dec_to_bin(unsigned char c, char* bin_text, int k) {
    if (k <= 7) {
        k++;
        dec_to_bin(c / 2, bin_text, k);
    }
    bin_text[++top] = c % 2;
}

int file_size(FILE* f) {
    if (fseek(f, 0L, SEEK_END)) finish("fseek error");
    int sz = ftell(f);
    rewind(f);
    return sz;
}

void uncode() {
    printf("Введите имя изображения:\n");
    scanf("%s", img_name);
    printf("Введите необходимое имя для изображения с \"секретным\" кодом:\n");
    scanf("%s", img_crypt_name);
    printf("Введите имя файла с текстом:\n");
    scanf("%s", txt_name);
    FILE* in_bmp = fopen(img_name, "rb");
    FILE* out_bmp = fopen(img_crypt_name, "wb+");
    FILE* in_txt = fopen(txt_name, "r");
    if (in_bmp == NULL || out_bmp == NULL || in_txt == NULL) finish("Один из файлов не может быть открыт");
    int  text_size = file_size(in_txt), level, k = 0, pointer = 0;;
    char* bin_text, c;
    if ((bin_text = (char*)malloc(8 * sizeof(char) * text_size + 70)) == NULL) finish("Не удалось выделить память");
    printf("Введите уровень шифрования от 1 до 8: \n");
    scanf("%d", &level);
    for (int i = 0; i < 6; i++) {
        c = fgetc(in_bmp);
        fputc(c, out_bmp);
    }
    c = fgetc(in_bmp); //резерв уровень
    fputc(level, out_bmp);
    c = fgetc(in_bmp);// резерв размер 1
    fputc(text_size / 256, out_bmp);
    c = fgetc(in_bmp);// резерв размер 2
    fputc(text_size % 256, out_bmp);
    c = fgetc(in_bmp); // резерв тип
    fputc(txt_name[strlen(txt_name) - 1], out_bmp);
    for (int i = 0; i < 8; i++) {
        c = fgetc(in_bmp);
        fputc(c, out_bmp);
    }
    int* width, * height;
    fread(&width, sizeof(int), 1, in_bmp);
    fwrite(&width, sizeof(int), 1, out_bmp);
    fread(&height, sizeof(int), 1, in_bmp);
    fwrite(&height, sizeof(int), 1, out_bmp);
    int marginal_sz = (int)width * (int)height * 3 + ((int)width % 4) * (int)height * level;
    if (text_size * 8 > marginal_sz || text_size > 65535) {
        printf("Максимальный размер текстового файла %d\nТекущий размер файла: %d\n", 65535 > marginal_sz / 8 ? marginal_sz / 8 : 65535, text_size);
        free(bin_text);
        finish("");
    }
    while (fscanf(in_txt, "%c", &c) != EOF) dec_to_bin(c, bin_text, 1);
    while ((top+1) % level != 0) bin_text[++top] = 0;
    while (fscanf(in_bmp, "%c", &c) != EOF) {
        if (k++ >= 28 && pointer <= top) {
            c >>= level;
            for (int i = 0; i < level; i++) {
                c <<= 1;
                c += bin_text[pointer++];
            }
        }

        fputc(c, out_bmp);
    }
    free(bin_text);
    printf("Успешно!");
}

void decode() {
    printf("Введите имя стегоконтейнера:\n");
    scanf("%s", img_name);
    FILE* in_cont = fopen(img_name, "rb");
    printf("Введите имя текстового файла для вывода:\n");
    scanf("%s", txt_name);
    FILE* out_crypt = fopen(txt_name, "wb+");
    if (in_cont == NULL || out_crypt == NULL) finish("Один из файлов не может быть открыт");
    int level, ln_crypt, point = 0, k = 0;
    move_input(in_cont, 6);
    level = fgetc(in_cont);
    ln_crypt = fgetc(in_cont) * 256 + fgetc(in_cont);
    char type = fgetc(in_cont), bin[8] = { 0 };
    move_input(in_cont, 44);
    while (1) {
        char c = fgetc(in_cont);
        for (int i = 0; i < level; i++) {
            bin[k++] = (c >> (level - i - 1)) & 1;
            if (k == 8) {
                int symb = 0;
                for (int q = 0; q < 8; q++) symb += bin[q] * (1 << (7 - q));
                if (symb == 10) {
                    fputc(13, out_crypt);
                    point++;
                }
                fputc(symb, out_crypt);
                k = symb = 0;
                point++;
                memset(bin, 0, 8);
            }
            if (point == ln_crypt) finish("Успешно!");
        }
    }
}

int main() {
    SetConsoleOutputCP(1251);
    int n;
    printf("1. Закодировать\n2. Декодировать\n");
    scanf("%d", &n);
    if (n == 1) uncode();
    else if (n == 2) decode();
    else finish("Введите допутимое значение: 1/2");
}

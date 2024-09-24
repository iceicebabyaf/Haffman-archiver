// gcc a.c heap.c -o program
// ./program
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "heap.h"
#define ALPH_SIZE 99999

// вывод кодов
void printCodes(struct HuffmanNode* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        printCodes(root->left, arr, top + 1);
    }

    if (root->right) {
        arr[top] = 1;
        printCodes(root->right, arr, top + 1);
    }

    if (!root->left && !root->right) {
        printf("%c: ", root->symbol);
        for (int i = 0; i < top; ++i) {
            printf("%d", arr[i]);
        }
        printf("\n");
    }
}
void HuffmanCodes(struct HuffmanNode* root) {
    int arr[ALPH_SIZE], top = 0;
    printCodes(root, arr, top);
}
// конец вывода

char* codes[ALPH_SIZE];

void saveCodes(struct HuffmanNode* root, int arr[], int top) {
    if (root->left) {
        arr[top] = 0;
        saveCodes(root->left, arr, top + 1);
    }

    if (root->right) {
        arr[top] = 1;
        saveCodes(root->right, arr, top + 1);
    }

    if (!root->left && !root->right) {
        codes[root->symbol] = malloc(top + 1);
        for (int i = 0; i < top; i++) {
            codes[root->symbol][i] = arr[i] ? '1' : '0';
        }
        codes[root->symbol][top] = '\0';
    }
}
void HuffmanCodesSv(struct HuffmanNode* root) {
    int arr[ALPH_SIZE], top = 0;
    memset(codes, 0, sizeof(codes));
    saveCodes(root, arr, top);
    // есть массив с кодами Хаффмана для каждого символа
}

void writeBitToFile(FILE *outputFile, unsigned char *byte, int *bitPosition, int bitValue) {
    if (bitValue) {
        *byte |= (1 << (7 - *bitPosition));
    }
    (*bitPosition)++;
    if (*bitPosition == 8) {
        fwrite(byte, 1, 1, outputFile); // байт полный => пишу
        *byte = 0;
        *bitPosition = 0;
    }
}


int writeTree(struct HuffmanNode* root, FILE *outputFile, unsigned char *byte, int *bitPosition) {
    if (!root->left && !root->right) {
        // Листовой узел => 1
        writeBitToFile(outputFile, byte, bitPosition, 1); 
        // Бобитовая запись аски
        for (int i = 7; i >= 0; --i) {
            writeBitToFile(outputFile, byte, bitPosition, (root->symbol >> i) & 1);
        }
    } else {
        // Узел с потомками => 0
        writeBitToFile(outputFile, byte, bitPosition, 0); 
        writeTree(root->left, outputFile, byte, bitPosition);
        writeTree(root->right, outputFile, byte, bitPosition);
    }
    return *bitPosition;
}



int main() {
    FILE *inputFile = fopen("input.txt", "r");
    FILE *outputFile = fopen("output.bin", "wb");
    if (inputFile == NULL) {
        perror("Error opening file\n");
        return 1;
    }
    // СЧИТЫВАНИЕ ДЛЯ СОЗДАНИЯ N ДЕРЕЬЕВ:
    // n деревьев:
    struct HuffmanNode* nodes[ALPH_SIZE] = {NULL};
    size_t length = 0;
    int ch;
    printf("Input file:\n");
    while ((ch = fgetc(inputFile)) != EOF) {
        createNodes(nodes, (unsigned char)ch);
        printf("%c", ch);
        length++;
    }
    printf("\n");

    // НАЧАЛО ЗАПИСИ
    // Запись длины текста в первых 4 байтах
    fwrite(&length, sizeof(int), 1, outputFile);
    printf("LENGTH: %zu\n", length);

    // Построение минимальной кучи и дерева Хаффмана из массива nodes
    struct HuffmanNode* root = HuffmanTree(nodes, ALPH_SIZE);

    // Вывод кодов из дерева
    printf("Huffman codes:\n");
    HuffmanCodes(root);

    // Отмотка файла к началу input
    rewind(inputFile);

    // Переменные для записи бинарных данных
    unsigned char byte = 0;
    int bitPosition = 0;

    // Запись дерева
    int bitPositionAfterTree = writeTree(root, outputFile, &byte, &bitPosition);
    // Сохранение кодов из дерева
    HuffmanCodesSv(root);
    
    if (bitPositionAfterTree == 8) {
        byte = 0;
        bitPosition = 0;
    }

    // Считывание файла посимвольно: для каждого символа - бинарный код
    while ((ch = fgetc(inputFile)) != EOF) {
        const char *code = codes[(unsigned char)ch];
        for (size_t i = 0; i < strlen(code); i++) {
            if (code[i] == '1') {
                byte |= (1 << (7 - bitPosition));
            }
            bitPosition++;
            if (bitPosition == 8) {
                fputc(byte, outputFile);
                bitPosition = 0;
                byte = 0;
            }
        }
    }

    // Если чото осталось => записываю в следующий байт
    if (bitPosition > 0) {
        fputc(byte, outputFile);
    }

    printf("------------------------------------\n");
    printf("| Encoding completed successfully. |\n");
    printf("------------------------------------\n");
    printf("Result len of input file: %zu\n", length);

    fclose(inputFile);
    fclose(outputFile);
    return 0;
}

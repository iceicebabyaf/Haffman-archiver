#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ALPH_SIZE 99999

struct HuffmanNode {
    unsigned char symbol;
    unsigned int freq;
    struct HuffmanNode *left, *right;
};

struct HuffmanNode* createNode(unsigned char symbol, unsigned frequency) {
    struct HuffmanNode *newNode = malloc(sizeof(struct HuffmanNode));
    newNode->symbol = symbol;
    newNode->freq = frequency;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}
// восстановление дерева Хаффмана
struct HuffmanNode* readTree(FILE *inputFile, unsigned char *byte, int *bitPosition) {
    // если байт заполнен => go next byte
    if (*bitPosition == 8) {
        if (fread(byte, 1, 1, inputFile) == 1) {
            *bitPosition = 0;
        } else {
            // Ошибка чтения файла
            return NULL;
        }
    }
    // рассматриваемый бит
    int currentBit = (*byte >> (7 - *bitPosition)) & 1;
    (*bitPosition)++;

    if (currentBit == 1) {
        unsigned char symbol = 0;
        for (int i = 0; i < 8; i++) {
            if (*bitPosition == 8) {
                if (fread(byte, 1, 1, inputFile) == 1) {
                    *bitPosition = 0;
                } else {
                    // Ошибка чтения файла
                    return NULL;
                }
            }
            symbol |= ((*byte >> (7 - *bitPosition)) & 1) << (7 - i);
            (*bitPosition)++;
        }
        struct HuffmanNode *leafNode = createNode(symbol, 0);
        return leafNode;
    } 
    else {
        struct HuffmanNode *leftNode = readTree(inputFile, byte, bitPosition);
        struct HuffmanNode *rightNode = readTree(inputFile, byte, bitPosition);

        struct HuffmanNode *parentNode = createNode(0, 0);
        parentNode->left = leftNode;
        parentNode->right = rightNode;
        return parentNode;
    }
}
// вывод кодов
void printCodes(struct HuffmanNode* rootEnd, int arr[], int top) {
    if (rootEnd->left) {
        arr[top] = 0;
        printCodes(rootEnd->left, arr, top + 1);
    }

    if (rootEnd->right) {
        arr[top] = 1;
        printCodes(rootEnd->right, arr, top + 1);
    }

    if (!rootEnd->left && !rootEnd->right) {
        printf("%c: ", rootEnd->symbol);
        for (int i = 0; i < top; ++i) {
            printf("%d", arr[i]);
        }
        printf("\n");
    }
}
void HuffmanCodes(struct HuffmanNode* rootEnd) {
    int arr[ALPH_SIZE], top = 0;
    printCodes(rootEnd, arr, top);
}
// конец вывода кодов

void decodeText(FILE *inputFile, struct HuffmanNode* root, unsigned char byte, int bitPosition, int originalTextLength) {
    struct HuffmanNode *currentNode = root;
    FILE *outputFile = fopen("outputArch.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening the file\n");
        exit(1);
    }

    // Если дерево Хаффмана содержит только один узел => исходный текст состоит из одного символа
    if (root->left == NULL && root->right == NULL) {
        // Вывожу символ числом из 1 4 байтов
        for (int i = 0; i < originalTextLength; ++i) {
            fputc(root->symbol, outputFile);
        }
        fclose(outputFile);
        return;
    }

    // Счетчик
    int decodedChars = 0;

    // Иду с битПоз после дерева
    do {
        for (; bitPosition < 8 && decodedChars < originalTextLength; ++bitPosition) {
            int bit = (byte >> (7 - bitPosition)) & 1;

            // Если 1 => правый узел, иначе левый
            currentNode = bit ? currentNode->right : currentNode->left;

            // Если лист
            if (currentNode->left == NULL && currentNode->right == NULL) {
                fputc(currentNode->symbol, outputFile); // Символ в файл
                currentNode = root; // обратно к корню
                ++decodedChars;
            }
        }
        bitPosition = 0;
    } while (decodedChars < originalTextLength && fread(&byte, 1, 1, inputFile) == 1); 

    fclose(outputFile);
}

int main() {
    FILE *inputFile = fopen("output.bin", "rb"); 
    if (inputFile == NULL) {
        perror("Error opening the file\n");
        exit(1);
    }

    // Дефолтная длина
    int originalTextLength;
    fread(&originalTextLength, sizeof(int), 1, inputFile);

    unsigned char byte = 0;
    int bitPosition = 8; // го некст байт

    struct HuffmanNode* rootEnd = readTree(inputFile, &byte, &bitPosition); // Читаю дерево
    printf("Huffman codes:\n");
    HuffmanCodes(rootEnd);
    
    if (bitPosition == 8) {
        bitPosition = 0;
    }

    decodeText(inputFile, rootEnd, byte, bitPosition, originalTextLength); // Читаю текст
    printf("------------------------------------\n");
    printf("| Decoding completed successfully. |\n");
    printf("------------------------------------\n");
    printf("Result len of output file: %d\n", originalTextLength);
    fclose(inputFile);
    return 0;
}

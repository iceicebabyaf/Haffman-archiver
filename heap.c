#include <stdio.h>
#include <stdlib.h>
#include "heap.h"
#define ALPH_SIZE 99999



struct HuffmanNode* createNode(unsigned char symbol, unsigned int freq) {
    struct HuffmanNode* newNode = (struct HuffmanNode*)malloc(sizeof(struct HuffmanNode));
    newNode->symbol = symbol;
    newNode->freq = freq;
    newNode->left = newNode->right = NULL;
    return newNode;
}
void createNodes(struct HuffmanNode** nodes, unsigned char ch) {
    if (!nodes[ch]) {
        nodes[ch] = malloc(sizeof(struct HuffmanNode));
        nodes[ch]->symbol = ch;
        nodes[ch]->freq = 1;
        nodes[ch]->left = NULL;
        nodes[ch]->right = NULL;
    } else {
        nodes[ch]->freq++;
    }
}
// создает кучу заданного размера
struct MinHeap* createMinHeap(unsigned int capacity) {
    struct MinHeap* minHeap = (struct MinHeap*)malloc(sizeof(struct MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (struct HuffmanNode**)malloc(minHeap->capacity * sizeof(struct HuffmanNode*));
    return minHeap;
}

void minHeapify(struct MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq) {
        smallest = left;
    }
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq) {
        smallest = right;
    }
    if (smallest != idx) {
        struct HuffmanNode* plug = minHeap->array[smallest];
        minHeap->array[smallest] = minHeap->array[idx];
        minHeap->array[idx] = plug;
        minHeapify(minHeap, smallest);
    }
}

void insertMinHeap(struct MinHeap* minHeap, struct HuffmanNode* node) {
    minHeap->size++;
    int i = minHeap->size - 1;

    while (i && node->freq < minHeap->array[(i - 1)/2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1)/2];
        i = (i - 1)/2;
    }
    minHeap->array[i] = node;
}
// минимум из минимальной кучи
struct HuffmanNode* takeMinimumPointer(struct MinHeap* minHeap) {
    struct HuffmanNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

struct MinHeap* buildMinHeap(struct HuffmanNode** nodes, int size) {
    struct MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i) {
        if (nodes[i]) {
            insertMinHeap(minHeap, nodes[i]);
        }
    }
    int n = minHeap->size - 1;
 
    return minHeap;
}

struct HuffmanNode* HuffmanTree(struct HuffmanNode** nodes, int size) {
    struct MinHeap* minHeap = buildMinHeap(nodes, size);

    while (minHeap->size != 1) {
        struct HuffmanNode* left = takeMinimumPointer(minHeap);
        struct HuffmanNode* right = takeMinimumPointer(minHeap);

        struct HuffmanNode* top = createNode(0, left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    return takeMinimumPointer(minHeap);
}
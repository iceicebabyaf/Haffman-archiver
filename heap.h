#pragma once
#define ALPH_SIZE 99999
struct HuffmanNode {
    unsigned char symbol;
    unsigned int freq;
    struct HuffmanNode *left, *right;
};
struct MinHeap {
    unsigned int 
    size;
    unsigned int capacity;
    struct HuffmanNode** array;
};
struct HuffmanNode* createNode(unsigned char symbol, unsigned int freq);
void createNodes(struct HuffmanNode** nodes, unsigned char ch);
struct MinHeap* createMinHeap(unsigned int capacity);
void minHeapify(struct MinHeap* minHeap, int idx);
void insertMinHeap(struct MinHeap* minHeap, struct HuffmanNode* node);
struct HuffmanNode* takeMinimumPointer(struct MinHeap* minHeap);
struct MinHeap* buildMinHeap(struct HuffmanNode** nodes, int size);
struct HuffmanNode* HuffmanTree(struct HuffmanNode** nodes, int size);

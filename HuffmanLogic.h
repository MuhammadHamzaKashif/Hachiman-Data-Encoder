#ifndef HUFFMANLOGIC_H
#define HUFFMANLOGIC_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm> // Required for sort/swap

// STB Image Configuration
// Ensure stb_image.h and stb_image_write.h are in your project directory
#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;

// ==========================================
// 1. CLASS DEFINITIONS
// ==========================================

class HuffNode
{
public:
    int symbol;
    int f;
    HuffNode *left;
    HuffNode *right;

    HuffNode() {
        symbol = 0; f = 0; left = nullptr; right = nullptr;
    }

    // Constructor for Leaf Nodes (Text chars or Pixel deltas)
    HuffNode(int symbol, int f) {
        this->symbol = symbol;
        this->f = f;
        this->left = nullptr;
        this->right = nullptr;
    }

    // Constructor for Internal Nodes (Parents)
    // Used by both the algorithm and the Visualizer
    HuffNode(int f, HuffNode* left, HuffNode* right) {
        this->symbol = 0;
        this->f = f;
        this->left = left;
        this->right = right;
    }

    ~HuffNode() {
        if(left) delete left;
        if(right) delete right;
    }
};

// Helper function to sort the visualizer Forest in MainWindow
inline bool compareNodes(HuffNode* a, HuffNode* b) {
    return a->f < b->f;
}

class HuffHeap
{
private:
    HuffNode **arr;

public:
    int capacity;
    int size;

    HuffHeap(int capacity) {
        this->capacity = capacity;
        this->arr = new HuffNode*[this->capacity + 1];
        this->arr[0] = new HuffNode(); // 1-based indexing
        this->size = 0;
    }

    ~HuffHeap() {
        delete[] arr;
    }

    bool isEmpty() { return this->size == 0; }
    bool isFull() { return this->size == this->capacity; }
    int getSize() { return this->size; }

    void push(HuffNode *val) {
        if (this->isFull()) return;
        int k = this->size + 1;
        this->arr[k] = val;
        this->size++;
        while (k > 1 && this->arr[k / 2]->f > this->arr[k]->f) {
            swap(this->arr[k / 2], this->arr[k]);
            k /= 2;
        }
    }

    HuffNode *pop() {
        if (isEmpty()) return nullptr;
        HuffNode* root = arr[1];
        arr[1] = arr[size--];
        int k = 1;
        while (true) {
            int left = 2 * k;
            int right = 2 * k + 1;
            if (left > size) break;
            int smallest = left;
            if (right <= size && arr[right]->f < arr[left]->f) smallest = right;
            if (arr[k]->f <= arr[smallest]->f) break;
            swap(arr[k], arr[smallest]);
            k = smallest;
        }
        return root;
    }
};

// ==========================================
// 2. TEXT COMPRESSION FUNCTIONS
// ==========================================

inline HuffNode *buildHuffmanTree(string s)
{
    int charFreqs[256] = {0};
    for (char c : s) charFreqs[(unsigned char)c]++;

    HuffHeap *h = new HuffHeap(256);
    for (int i = 0; i < 256; i++) {
        if (charFreqs[i] > 0)
            h->push(new HuffNode(i, charFreqs[i]));
    }

    // If only 1 char type exists, dummy node handling might be needed in production,
    // but standard loop works for >1 distinct chars.
    while (h->getSize() > 1) {
        HuffNode *left = h->pop();
        HuffNode *right = h->pop();
        h->push(new HuffNode(left->f + right->f, left, right));
    }

    HuffNode* root = h->pop();
    delete h;
    return root;
}

inline void generateCodes(HuffNode *h, string code, string codes[])
{
    if (!h) return;
    if (!h->left && !h->right) {
        codes[h->symbol] = code;
        return;
    }
    generateCodes(h->left, code + "0", codes);
    generateCodes(h->right, code + "1", codes);
}

inline string *getHuffmanCodes(HuffNode *h)
{
    static string codes[256];
    for (int i = 0; i < 256; i++) codes[i] = ""; // Reset
    generateCodes(h, "", codes);
    return codes;
}

inline string encode(string s, HuffNode *huffmanTree)
{
    string *codes = getHuffmanCodes(huffmanTree);
    string res = "";

    // SAFETY FIX: Cast to unsigned char to avoid negative index crash
    for (char c : s)
        res += codes[(unsigned char)c];

    return res;
}

// ==========================================
// 3. IMAGE COMPRESSION FUNCTIONS
// ==========================================

inline unsigned char *loadImage(string path, int &width, int &height, int &channels)
{
    return stbi_load(path.c_str(), &width, &height, &channels, 0);
}

inline HuffNode *buildHuffmanTreeForImage(unsigned char *img_data, long long data_size)
{
    // 511 size because "Pixel - Previous + 255" results in range 0 to 510
    int freqs[511] = {0};
    for (long long i = 0; i < data_size; i++) {
        int pixel = (int)img_data[i];
        // PREDICTIVE CODING LOGIC
        int processed_val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
        freqs[processed_val]++;
    }

    HuffHeap *h = new HuffHeap(511);
    for (int i = 0; i < 511; i++) {
        if (freqs[i] > 0)
            h->push(new HuffNode(i, freqs[i]));
    }

    while (h->getSize() > 1) {
        HuffNode *left = h->pop();
        HuffNode *right = h->pop();
        h->push(new HuffNode(left->f + right->f, left, right));
    }

    HuffNode* root = h->pop();
    delete h;
    return root;
}

inline string *getHuffmanCodesForImage(HuffNode *h)
{
    static string codes[511];
    for (int i = 0; i < 511; i++) codes[i] = "";
    generateCodes(h, "", codes);
    return codes;
}

// PARAMETER UPDATE: Accepts references to return the Tree and Size for verification
inline string encodeImage(string path, HuffNode*& outTree, long long& outDataSize)
{
    int width, height, channels;
    unsigned char *img_data = loadImage(path, width, height, channels);
    if (!img_data) {
        outDataSize = 0;
        return "";
    }

    outDataSize = width * height * channels;

    // Build Tree
    outTree = buildHuffmanTreeForImage(img_data, outDataSize);
    string *codes = getHuffmanCodesForImage(outTree);

    string res = "";
    // PERFORMANCE FIX: Reserve memory to prevent freezing during concatenation
    res.reserve(outDataSize * 8);

    for (long long i = 0; i < outDataSize; i++) {
        int pixel = (int)img_data[i];
        int processed_val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
        res += codes[processed_val];
    }

    stbi_image_free(img_data);
    return res;
}

// Decode function (Used by MainWindow to verify compression works)
inline unsigned char *decodeImage(string encodeImage, HuffNode *huffmanTree, long long data_size)
{
    if (!huffmanTree) return nullptr;

    unsigned char *img_data = new unsigned char [data_size];
    long long i = 0;

    HuffNode *ptr = huffmanTree;
    for (char bit : encodeImage) {
        if (bit == '0') ptr = ptr->left;
        else ptr = ptr->right;

        // If leaf node reached
        if (!ptr->left && !ptr->right) {
            if (i >= data_size) break;

            // Reverse Predictive Coding: Value = Symbol - 255 + Previous
            int val = ptr->symbol - 255 + (i == 0 ? 0 : img_data[i-1]);

            // Clamp to byte range just in case
            if (val < 0) val = 0;
            if (val > 255) val = 255;

            img_data[i] = (unsigned char)val;
            i++;
            ptr = huffmanTree; // Reset to root
        }
    }
    return img_data;
}

inline void saveImage(string path, unsigned char *img_data, int width, int height, int channels)
{
    stbi_write_png(path.c_str(), width, height, channels, img_data, width*channels);
}

inline double getCompressionRatio(long long encodedShiiLength, long long origLength)
{
    return (1.0 - (encodedShiiLength/((double)(origLength)*8)));
}

#endif // HUFFMANLOGIC_H

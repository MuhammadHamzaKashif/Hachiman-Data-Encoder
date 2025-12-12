#ifndef HUFFMANLOGIC_H
#define HUFFMANLOGIC_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <sstream>

#include "stb_image.h"
#include "stb_image_write.h"

using namespace std;


// Class for node of the huffman tree
class HuffNode
{
public:
    int symbol;
    int f;
    HuffNode *left;
    HuffNode *right;

    HuffNode()
    {
        this->symbol = 0;
        this->f = 0;
        this->left = nullptr;
        this->right = nullptr;
    }

    HuffNode(int symbol, int f)
    {
        this->symbol = symbol;
        this->f = f;
        this->left = nullptr;
        this->right = nullptr;
    }

    HuffNode(int f, HuffNode* left, HuffNode* right)
    {
        this->symbol = 0;
        this->f = f;
        this->left = left;
        this->right = right;
    }

    ~HuffNode()
    {
        if (this->left) delete this->left;
        if (this->right) delete this->right;
    }
};


inline bool compareNodes(HuffNode* a, HuffNode* b)
{
    return a->f < b->f;
}


// Class for heap containing huffman nodes
class HuffHeap
{
private:
    HuffNode **arr;

public:
    int capacity;
    int size;

    HuffHeap(int capacity)
    {
        this->capacity = capacity;
        this->arr = new HuffNode*[this->capacity + 1];
        this->arr[0] = new HuffNode();
        this->size = 0;
    }

    ~HuffHeap()
    {
        delete[] this->arr;
    }

    bool isEmpty()
    {
        return this->size == 0;
    }

    bool isFull()
    {
        return this->size == this->capacity;
    }

    int getSize()
    {
        return this->size;
    }

    void push(HuffNode *val)
    {
        if (this->isFull()) return;

        int k = this->size + 1;
        this->arr[k] = val;
        this->size++;

        while (k > 1 && this->arr[k / 2]->f > this->arr[k]->f)
        {
            swap(this->arr[k / 2], this->arr[k]);
            k /= 2;
        }
    }

    HuffNode *pop()
    {
        if (isEmpty()) return nullptr;

        HuffNode* root = this->arr[1];
        this->arr[1] = this->arr[size--];

        int k = 1;
        while (true)
        {
            int left = 2 * k;
            int right = 2 * k + 1;

            if (left > this->size) break;

            int smallest = left;

            if (right <= this->size && this->arr[right]->f < this->arr[left]->f)
                smallest = right;

            if (this->arr[k]->f <= this->arr[smallest]->f) break;

            swap(this->arr[k], this->arr[smallest]);
            k = smallest;
        }
        return root;
    }
};


// Tree serialization

// Recursive helper to build string from tree
// Saves Frequency in the string for reconstruction
inline void serializeRecursive(HuffNode* root, string& out)
{
    if (!root) return;

    if (!root->left && !root->right)
    {
        // Format: L<Symbol>:<Frequency>.
        out += "L" + to_string(root->symbol) + ":" + to_string(root->f) + ".";
    }
    else
    {
        out += "I";
        serializeRecursive(root->left, out);
        serializeRecursive(root->right, out);
    }
}


inline string serializeTree(HuffNode* root)
{
    string res = "";
    serializeRecursive(root, res);
    return res;
}


// Recursive helper to rebuild tree from string
// Reads Frequency and Recalculates Parents
inline HuffNode* deserializeRecursive(const string& data, int& pos)
{
    if (pos >= data.length()) return nullptr;

    char type = data[pos];
    pos++;

    if (type == 'L')
    {
        // Parse Symbol
        string numStr = "";
        while (pos < data.length() && data[pos] != ':')
        {
            numStr += data[pos];
            pos++;
        }
        pos++;

        // Parse Frequency
        string freqStr = "";
        while (pos < data.length() && data[pos] != '.')
        {
            freqStr += data[pos];
            pos++;
        }
        pos++;

        int symbol = stoi(numStr);
        int freq = stoi(freqStr);
        return new HuffNode(symbol, freq);
    }
    else if (type == 'I')
    {
        HuffNode* left = deserializeRecursive(data, pos);
        HuffNode* right = deserializeRecursive(data, pos);

        int sumFreq = (left ? left->f : 0) + (right ? right->f : 0);
        return new HuffNode(sumFreq, left, right);
    }
    return nullptr;
}


inline HuffNode* deserializeTree(string data)
{
    int pos = 0;
    return deserializeRecursive(data, pos);
}


// Text Compression

// first we initialize an array of 256 chars (for ASCII vals)
// then we find the frequency by direct addressing
// then we insert the chars to the heap
// then we pop first two elements and join them
inline HuffNode *buildHuffmanTree(string s)
{
    int charFreqs[256] = {0};

    for (char c : s)
        charFreqs[(unsigned char)c]++;

    HuffHeap *h = new HuffHeap(256);

    for (int i = 0; i < 256; i++)
    {
        if (charFreqs[i] > 0)
            h->push(new HuffNode(i, charFreqs[i]));
    }

    while (h->getSize() > 1)
    {
        HuffNode *left = h->pop();
        HuffNode *right = h->pop();
        h->push(new HuffNode(left->f + right->f, left, right));
    }

    HuffNode* root = h->pop();
    delete h;
    return root;
}


// recursively generate codes by
// diving into the huffman tree and
// mapping the codes in an array
inline void generateCodes(HuffNode *h, string code, string codes[])
{
    if (!h) return;

    if (!h->left && !h->right)
    {
        codes[h->symbol] = code;
        return;
    }

    generateCodes(h->left, code + "0", codes);
    generateCodes(h->right, code + "1", codes);
}


inline string *getHuffmanCodes(HuffNode *h)
{
    static string codes[511];

    for (int i = 0; i < 511; i++)
        codes[i] = "";

    generateCodes(h, "", codes);
    return codes;
}


inline string encode(string s, HuffNode *huffmanTree)
{
    string *codes = getHuffmanCodes(huffmanTree);
    string res = "";

    for (char c : s)
        res += codes[(unsigned char)c];

    return res;
}


inline string decode(string s, HuffNode *huffmanTree)
{
    if (!huffmanTree) return "";

    string res = "";
    HuffNode *ptr = huffmanTree;

    for (char c : s)
    {
        if (c == '0')
            ptr = ptr->left;
        else
            ptr = ptr->right;

        if (!ptr) return "Error";

        if (!ptr->left && !ptr->right)
        {
            res += (char)ptr->symbol;
            ptr = huffmanTree;
        }
    }
    return res;
}


// Image compression

inline unsigned char *loadImage(string path, int &width, int &height, int &channels)
{
    return stbi_load(path.c_str(), &width, &height, &channels, 0);
}


inline HuffNode *buildHuffmanTreeForImage(unsigned char *img_data, long long data_size)
{
    // Array size is 511 because after processing
    // the range is [0, 510]
    int freqs[511] = {0};

    for (long long i = 0; i < data_size; i++)
    {
        // Used predictive coding here
        // We subtract the left val from the current pixel val
        int pixel = (int)img_data[i];
        int processed_val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
        freqs[processed_val]++;
    }

    HuffHeap *h = new HuffHeap(511);

    for (int i = 0; i < 511; i++)
    {
        if (freqs[i] > 0)
            h->push(new HuffNode(i, freqs[i]));
    }

    while (h->getSize() > 1)
    {
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

    for (int i = 0; i < 511; i++)
        codes[i] = "";

    generateCodes(h, "", codes);
    return codes;
}


// Updated to return the tree and data size via references
// This allows the GUI to access the tree for visualization
inline string encodeImage(string path, HuffNode*& outTree, long long& outDataSize)
{
    int width, height, channels;
    unsigned char *img_data = loadImage(path, width, height, channels);

    if (!img_data)
    {
        outDataSize = 0;
        return "";
    }

    outDataSize = width * height * channels;
    outTree = buildHuffmanTreeForImage(img_data, outDataSize);

    string *codes = getHuffmanCodesForImage(outTree);
    string res = "";

    // reserve memory to avoid frequent reallocations
    res.reserve(outDataSize * 8);

    for (long long i = 0; i < outDataSize; i++)
    {
        int pixel = (int)img_data[i];
        int processed_val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
        res += codes[processed_val];
    }

    stbi_image_free(img_data);
    return res;
}


inline unsigned char *decodeImage(string encodeImage, HuffNode *huffmanTree, long long data_size)
{
    if (!huffmanTree) return nullptr;

    unsigned char *img_data = new unsigned char [data_size];
    long long i = 0;

    HuffNode *ptr = huffmanTree;

    for (char bit : encodeImage)
    {
        if (bit == '0')
            ptr = ptr->left;
        else
            ptr = ptr->right;

        if (!ptr) break;

        if (!ptr->left && !ptr->right)
        {
            if (i >= data_size) break;

            // Here we apply the reverse process of before
            // first 255 is subtracted, then the left val is added
            int val = ptr->symbol - 255 + (i == 0 ? 0 : img_data[i-1]);

            if (val < 0) val = 0;
            if (val > 255) val = 255;

            img_data[i] = (unsigned char)val;
            i++;
            ptr = huffmanTree;
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

#endif

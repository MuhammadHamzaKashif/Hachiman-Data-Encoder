#include <iostream>
#include <cmath>
#include <cstring>
#include <cstdlib>
#include <string>
#include <vector>
#include <iomanip>
#include <stdexcept>
#include <iconv.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
#include <openssl/err.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

using namespace std;

class HuffNode
{
public:
    int symbol;
    int f;
    HuffNode *left;
    HuffNode *right;
    HuffNode();
    HuffNode(int symbol, int f);
    ~HuffNode();
};

HuffNode::HuffNode()
{
    this->symbol = 0;
    this->f = 0;
    this->left = nullptr;
    this->right = nullptr;
}

HuffNode::HuffNode(int symbol, int f)
{
    this->symbol = symbol;
    this->f = f;
    this->left = nullptr;
    this->right = nullptr;
}

HuffNode::~HuffNode()
{
}


class HuffHeap
{
private:
    HuffNode **arr;

public:
    int capacity;
    int size;
    HuffHeap();
    HuffHeap(int capacity);
    HuffHeap(HuffNode **arr, int s);
    bool isEmpty();
    bool isFull();
    int getSize();
    void push(HuffNode *val);
    HuffNode *pop();

    ~HuffHeap();
};


HuffHeap::HuffHeap()
{
    this->capacity = 255;
    this->arr = new HuffNode*[256];
    this->size = 0;
}


HuffHeap::HuffHeap(int capacity)
{
    this->capacity = capacity;
    this->arr = new HuffNode*[this->capacity + 1];
    this->arr[0] = new HuffNode();
    this->size = 0;
}


HuffHeap::HuffHeap(HuffNode **arr, int s)
{
    this->capacity = 255;
    this->arr = new HuffNode*[256];
    this->arr[0] = new HuffNode();
    this->size = 0;
    for (int i = 0; i < s; i++)
    {
        this->push(arr[i]);
    }
}


HuffHeap *buildHuffHeap(HuffNode **arr, int s)
{
    return new HuffHeap(arr, s);
}


bool HuffHeap::isEmpty()
{
    return this->size == 0;
}


bool HuffHeap::isFull()
{
    return this->size == this->capacity;
}


int HuffHeap::getSize()
{
    return this->size;
}


void HuffHeap::push(HuffNode *val)
{
    if (this->isFull())
    {
        cout << "HuffHeap Full" << endl;
        return;
    }
    int k = this->size + 1;
    this->arr[k] = val;
    this->size++;
    while (k > 1 && this->arr[k / 2]->f > this->arr[k]->f)
    {
        swap(this->arr[k / 2], this->arr[k]);
        k /= 2;
    }
}


HuffNode* HuffHeap::pop() {
    if (isEmpty())
    {
        cout << "Empty" << endl;
        return nullptr;
    }

    HuffNode* root = arr[1];
    arr[1] = arr[size--];

    int k = 1;
    while (true)
    {
        int left = 2 * k;
        int right = 2 * k + 1;

        if (left > size)
            break;

        int smallest = left;

        if (right <= size && arr[right]->f < arr[left]->f)
            smallest = right;

        if (arr[k]->f <= arr[smallest]->f)
            break;

        swap(arr[k], arr[smallest]);
        k = smallest;
    }

    return root;
}

HuffHeap::~HuffHeap()
{
    delete[] arr;
}



string utf8ToAsciiIconv(const string& input) {
    iconv_t cd = iconv_open("ASCII//TRANSLIT", "UTF-8");
    if (cd == (iconv_t)-1) {
        perror("iconv_open");
        return "";
    }

    size_t inBytesLeft = input.size();
    size_t outBytesLeft = inBytesLeft * 2; // allocate enough space
    char* inBuf = const_cast<char*>(input.c_str());
    char* outBufStart = (char*)malloc(outBytesLeft);
    if (!outBufStart) {
        iconv_close(cd);
        return "";
    }
    char* outBuf = outBufStart;

    size_t result = iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft);
    if (result == (size_t)-1) {
        perror("iconv");
    }

    string output(outBufStart, outBuf - outBufStart);

    free(outBufStart);
    iconv_close(cd);

    return output;
}



HuffNode *buildHuffmanTree(string s)
{
    int charFreqs[256] = {0};
    for (char c : s)
        charFreqs[int(c)]++;
    HuffHeap *h = new HuffHeap();
    for (int i = 0; i < 256; i++)
    {
        if (charFreqs[i] > 0)
            h->push(new HuffNode(i, charFreqs[i]));
    }
    while (h->getSize() > 1)
    {
        HuffNode *left = h->pop();
        HuffNode *right = h->pop();
        HuffNode *newNode = new HuffNode();
        newNode->f = left->f + right->f;
        newNode->left = left;
        newNode->right = right;
        h->push(newNode);
    }
    HuffNode* root = h->pop();
    delete h;
    return root;
}
void generateCodes(HuffNode *h, string code, string codes[])
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
string *getHuffmanCodes(HuffNode *h)
{
    static string codes[256];
    for (int i = 0; i < 256; i++) codes[i] = "";
    generateCodes(h, "", codes);
    return codes;
}

void drawTable(string s, string *codes)
{
    int charFreqs[256] = {0};
    for (char c : s)
        charFreqs[int(c)]++;
    cout << "char | freq | code" << endl;
    for (int i = 0; i < 256; i++)
    {
        if (charFreqs[i] > 0)
            cout << "  " << char(i) << "  |   " << charFreqs[i] << "  | " << codes[i] << endl;
    }

}


string encode(string s, HuffNode *huffmanTree)
{
    string *codes = getHuffmanCodes(huffmanTree);
    string res = "";
    for (char c : s)
        res += codes[int(c)];
    return res;
}

string decode(string s, HuffNode *huffmanTree)
{
    string res = "";
    HuffNode *ptr = huffmanTree;
    for (char c : s)
    {
        if (c == '0')
            ptr = ptr->left;
        else
            ptr = ptr->right;

        if (!ptr->left && !ptr->right)
        {
            res += (char)ptr->symbol;
            ptr = huffmanTree;
        }
    }
    return res;
}

// Image compression part
unsigned char *loadImage(string path, int &width, int &height, int &channels)
{
    // Loading the image
    unsigned char *img_data = stbi_load(path.c_str(), &width, &height, &channels, 0);

    if (!img_data)
    {
        cout << "Error loading the image" << endl;
        return nullptr;
    }
    cout << "Image Loaded" << endl;
    return img_data;
}
HuffNode *buildHuffmanTreeForImage(unsigned char *img_data, long long data_size)
{
    // Array size is 511 because after processing the
    // pixel values, the range of them is [0, 510]
    int freqs[511] = {0};
    for (long long i = 0; i < data_size; i++)
    {
        // Used predictive coding here
        // As the pixel values are quite close,
        // We subtract the left val from the current pixel val
        // this creates a list of repeating vals on which
        // effective huffman encoding can be applied
        // To cater for negative vals, we add 255

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
        HuffNode *newNode = new HuffNode();
        newNode->f = left->f + right->f;
        newNode->left = left;
        newNode->right = right;
        h->push(newNode);
    }
    HuffNode* root = h->pop();
    delete h;
    return root;
}

string *getHuffmanCodesForImage(HuffNode *h)
{
    static string codes[511];
    for (int i = 0; i < 511; i++) codes[i] = "";
    generateCodes(h, "", codes);
    return codes;
}

string encodeImage(string path)
{
    // Loading the image
    int width, height, channels;
    unsigned char *img_data = loadImage(path, width, height, channels);
    long long data_size = width * height * channels;
    if (!img_data)
        return "";
    string res = "";
    HuffNode *huffmanTree = buildHuffmanTreeForImage(img_data, data_size);
    string *codes = getHuffmanCodesForImage(huffmanTree);
    for (long long i = 0; i < data_size; i++)
    {
        int pixel = (int)img_data[i];
        int processed_val = pixel - (i == 0 ? 0 : (int)img_data[i-1]) + 255;
        res += codes[processed_val];
    }

    return res;
}


// Saving the image after decoding
void saveImage(string path, unsigned char *img_data, int width, int height, int channels)
{
    stbi_write_png(path.c_str(), width, height, channels, img_data, width*channels);
    cout << "Decoded image saved" << endl;
    delete[] img_data;
}

unsigned char *decodeImage(string encodeImage, HuffNode *huffmanTree, long long data_size)
{
    unsigned char *img_data = new unsigned char [data_size];
    long long i = 0;

    HuffNode *ptr = huffmanTree;
    for (char bit : encodeImage)
    {
        if (bit == '0')
            ptr = ptr->left;
        else
            ptr = ptr->right;

        if (!ptr->left && !ptr->right)
        {
            // Here we apply the reverse process of before
            // first 255 is subtracted
            // then the left val is added
            int val = ptr->symbol - 255 + (i == 0 ? 0 : img_data[i-1]);

            img_data[i] = (unsigned char)val;
            i++;

            ptr = huffmanTree;
        }
    }

    return img_data;
}



vector<unsigned char> packBits(const string &bitstring)
{
    vector<unsigned char> bytes;
    unsigned char currByte = 0;
    int bitNum = 0;

    for (char bit : bitstring)
    {
        // Shifting curr byte left
        currByte <<= 1;

        // If char is 1, we set the least significant bit
        if (bit == '1')
            currByte |= 1;

        // Moving to next bit
        bitNum++;

        // When 8 bits are pushed, we push our
        // completed byte to result vector
        if (bitNum == 8)
        {
            bytes.push_back(currByte);
            currByte = 0;
            bitNum = 0;
        }
    }

    // Last remaining bits
    if (bitNum > 0)
    {
        currByte <<= (8 - bitNum);
        bytes.push_back(currByte);
    }

    return bytes;
}

string unpackBits(const vector<unsigned char> &bytes, size_t origBitLength)
{
    string bitString = "";

    for (unsigned char byte : bytes)
    {
        for (int i = 7; i >= 0; --i)
        {
            if (bitString.length() >= origBitLength)
                break;

            // Depending on the ith bit, add 0 or 1
            bitString += ((byte >> i) & 1) ? '1' : '0';
        }

    }
    return bitString;
}

void handleOpenSSLErrors()
{
    throw runtime_error("An OpenSSL error occured");
}
vector<unsigned char> encryptEncodedShii(const vector<unsigned char> &plaintext, const unsigned char *key, const unsigned char *iv)
{
    EVP_CIPHER_CTX *ctx;

    vector<unsigned char> ciphertext(plaintext.size() + EVP_MAX_BLOCK_LENGTH);
    int len = 0;
    int ciphertextLen = 0;

    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        handleOpenSSLErrors();
    }

    if (1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLErrors();
    }

    if (1 != EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext.data(), plaintext.size()))
    {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLErrors();
    }

    ciphertextLen = len;

    if (1 != EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len))
    {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLErrors();
    }
    ciphertextLen += len;

    EVP_CIPHER_CTX_free(ctx);
    ciphertext.resize(ciphertextLen);

    return ciphertext;
}


vector<unsigned char> decryptEncodedShii(const vector<unsigned char>& ciphertext, const unsigned char* key, const unsigned char* iv)
{
    EVP_CIPHER_CTX* ctx;
    vector<unsigned char> plaintext(ciphertext.size());
    int len = 0;
    int plaintextLen = 0;

    if (!(ctx = EVP_CIPHER_CTX_new()))
    {
        handleOpenSSLErrors();
    }

    if (1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, key, iv))
    {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLErrors();
    }

    if (1 != EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext.data(), ciphertext.size()))
    {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLErrors();
    }
    plaintextLen = len;


    if (1 != EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len))
    {
        EVP_CIPHER_CTX_free(ctx);
        handleOpenSSLErrors();
    }
    plaintextLen += len;

    EVP_CIPHER_CTX_free(ctx);

    plaintext.resize(plaintextLen);

    return plaintext;
}










double getCompressionRatio(long long encodedShiiLength, long long origLength)
{
    return (1.0 - (encodedShiiLength/((double)(origLength)*8)));
}

void print_hex(const string& label, const vector<unsigned char>& data)
{
    cout << label;
    for (const auto& byte : data)
    {
        cout << hex << setw(2) << setfill('0') << (int)byte;
    }
    cout << dec << endl;
}



int main()
{
    cout << "Enter text: ";
    string s;
    getline(cin, s);

    cout << "Normal: " << s << endl;
    HuffNode *huffmanTree = buildHuffmanTree(s);
    string *codes = getHuffmanCodes(huffmanTree);
    drawTable(s, codes);
    string encodedShii = encode(s, huffmanTree);
    cout << "Encoded: " << encodedShii << endl;

    unsigned char key[32];

    unsigned char iv[16];

    if (!RAND_bytes(key, sizeof(key)) || !RAND_bytes(iv, sizeof(iv)))
    {
        cerr << "FATAL: Could not generate random key/IV." << endl;
        return 1;
    }





    size_t original_bit_length = encodedShii.length();
    vector<unsigned char> packed_data = packBits(encodedShii);
    cout << "Packed data size: " << packed_data.size() << " bytes" << endl;

    cout << "--- Encryption ---" << endl;

    vector<unsigned char> encrypted_data = encryptEncodedShii(packed_data, key, iv);

    print_hex("Encrypted (Hex): ", encrypted_data);
    cout << "Encrypted data size: " << encrypted_data.size() << " bytes" << endl;

    cout << "--- Decryption ---" << endl;
    vector<unsigned char> decrypted_data = decryptEncodedShii(encrypted_data, key, iv);
    cout << "Decrypted data size: " << decrypted_data.size() << " bytes" << endl;

    string unpacked_bitstring = unpackBits(decrypted_data, original_bit_length);
    cout << "Unpacked Bitstring: " << unpacked_bitstring << endl;

    cout << "Decoded: " << decode(unpacked_bitstring, huffmanTree) << endl;
    cout << "Compression %age: " << getCompressionRatio(encodedShii.length(), s.length())*100.0 << "%" << endl;

    // string path = "3d-tech.jpg";
    // int width, height, channels;
    // unsigned char *img_data = loadImage(path, width, height, channels);
    // long long data_size = width * height * channels;
    // HuffNode *huffmanTree = buildHuffmanTreeForImage(img_data, data_size);
    // string encodedShii = encodeImage(path);
    // saveImage("dec_img.png", decodeImage(encodedShii, huffmanTree, data_size), width, height, channels);
    // cout << "Compression %age: " << getCompressionRatio(encodedShii.length(), data_size)*100.0 << "%" << endl;
    // delete[] img_data;
    // delete huffmanTree;

    return 0;
}

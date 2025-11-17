#include <iostream>
#include <cmath>
#include <string>
using namespace std;


class HuffNode
{
public:
    char c;
    int f;
    HuffNode *left;
    HuffNode *right;
    HuffNode();
    HuffNode(char c, int f);
    ~HuffNode();
};

HuffNode::HuffNode()
{
    this->c = ' ';
    this->f = 0;
    this->left = nullptr;
    this->right = nullptr;
}

HuffNode::HuffNode(char c, int f)
{
    this->c = c;
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






HuffNode *buildHuffmanTree(string s)
{
    int charFreqs[256] = {0};
    for (char c : s)
        charFreqs[int(c)]++;
    HuffHeap *h = new HuffHeap();
    for (int i = 0; i < 256; i++)
    {
        if (charFreqs[i] > 0)
            h->push(new HuffNode(char(i), charFreqs[i]));
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
        codes[int(h->c)] = code;
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
            res += ptr->c;
            ptr = huffmanTree;
        }
    }
    return res;
}


double getCompressionRatio(string encodedShii, string text)
{
    return (1.0 - (encodedShii.length()/((double)(text.length())*8)));
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
    cout << "Decoded: " << decode(encodedShii, huffmanTree) << endl;
    cout << "Compression %age: " << getCompressionRatio(encodedShii, s)*100.0 << "%" << endl;

    return 0;
}

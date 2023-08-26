#include <iostream>
#include <fstream>
#include <queue>
#include <map>
using namespace std;

struct HuffmanNode {
    char data;
    int freq;
    HuffmanNode* left;
    HuffmanNode* right;
    
    HuffmanNode(char data, int freq) : data(data), freq(freq), left(nullptr), right(nullptr) {}
};

struct CompareNodes {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->freq > b->freq;
    }
};

void generateHuffmanTree(priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes>& pq) {
    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();
        
        HuffmanNode* newNode = new HuffmanNode('$', left->freq + right->freq);
        newNode->left = left;
        newNode->right = right;
        
        pq.push(newNode);
    }
}

void generateHuffmanCodes(HuffmanNode* root, string code, map<char, string>& huffmanCodes) {
    if (root == nullptr)
        return;
    
    if (root->data != '$')
        huffmanCodes[root->data] = code;
    
    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

void compressFile(string inputFile, string outputFile) {
    ifstream inFile(inputFile, ios::binary);
    if (!inFile) {
        cout << "Unable to open input file.";
        return;
    }
    
    map<char, int> freqMap;
    char ch;
    while (inFile.get(ch)) {
        freqMap[ch]++;
    }
    inFile.close();
    
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, CompareNodes> pq;
    for (const auto& pair : freqMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }
    
    generateHuffmanTree(pq);
    
    HuffmanNode* root = pq.top();
    
    map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);
    
    ifstream inFileAgain(inputFile, ios::binary);
    ofstream outFile(outputFile, ios::binary);
    string encodedBits = "";
    while (inFileAgain.get(ch)) {
        encodedBits += huffmanCodes[ch];
        while (encodedBits.size() >= 8) {
            string byte = encodedBits.substr(0, 8);
            encodedBits = encodedBits.substr(8);
            char byteValue = static_cast<char>(stoi(byte, nullptr, 2));
            outFile.put(byteValue);
        }
    }
    
    int padding = 8 - encodedBits.size();
    for (int i = 0; i < padding; ++i) {
        encodedBits += "0";
    }
    char paddingChar = static_cast<char>(padding);
    outFile.put(paddingChar);
    
    while (encodedBits.size() >= 8) {
        string byte = encodedBits.substr(0, 8);
        encodedBits = encodedBits.substr(8);
        char byteValue = static_cast<char>(stoi(byte, nullptr, 2));
        outFile.put(byteValue);
    }
    
    inFileAgain.close();
    outFile.close();
    
    cout << "File compressed successfully." << endl;
}

int main() {
    string inputFile = "input.txt";
    string compressedFile = "compressed.bin";
    
    compressFile(inputFile, compressedFile);
    
    return 0;
}

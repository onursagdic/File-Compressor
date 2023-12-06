#include <iostream>
#include <fstream>
#include <unordered_map>
#include <queue>
#include <bitset>

using namespace std;

// Huffman ağacı düğümü
struct HuffmanNode {
    char data;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char data, int frequency) : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
};

// Huffman ağacı düğümünün karşılaştırıcısı
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Huffman ağacını oluşturan yardımcı fonksiyon
HuffmanNode* buildHuffmanTree(const unordered_map<char, int>& frequencies) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;

    for (const auto& pair : frequencies) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* newNode = new HuffmanNode('$', left->frequency + right->frequency);
        newNode->left = left;
        newNode->right = right;

        pq.push(newNode);
    }

    return pq.top();
}

// Huffman ağacından kodlamaları oluşturan yardımcı fonksiyon
void generateHuffmanCodes(HuffmanNode* root, string code, unordered_map<char, string>& huffmanCodes) {
    if (root == nullptr) {
        return;
    }

    if (root->left == nullptr && root->right == nullptr) {
        huffmanCodes[root->data] = code;
    }

    generateHuffmanCodes(root->left, code + "0", huffmanCodes);
    generateHuffmanCodes(root->right, code + "1", huffmanCodes);
}

// Metni Huffman algoritmasıyla sıkıştıran fonksiyon
void compressText(const string& inputFile, const string& compressedFile) {
    // Adım 1: Metinden karakter frekansları
    unordered_map<char, int> frequencies;
    ifstream ifs(inputFile);
    char ch;
    while (ifs.get(ch)) {
        frequencies[ch]++;
    }
    ifs.close();

    // Adım 2: Huffman ağacı
    HuffmanNode* root = buildHuffmanTree(frequencies);

    // Adım 3: Huffman kodlamaları
    unordered_map<char, string> huffmanCodes;
    generateHuffmanCodes(root, "", huffmanCodes);

    // Adım 4: Sıkıştırılmış veriyi okuma kısmı
    string compressedData;
    ifs.open(inputFile);
    while (ifs.get(ch)) {
        compressedData += huffmanCodes[ch];
    }
    ifs.close();

    // Adım 5: Sıkıştırılmış veriyi bir dosyaya yazma kısmı
    ofstream ofs(compressedFile, ios::binary);
    ofs << huffmanCodes.size() << endl; // Kod tablosunun boyutunu yazma kısmı

    // Kod tablosunu dosyaya yazın
    for (const auto& pair : huffmanCodes) {
        ofs << pair.first << pair.second << endl;
    }

    // Sıkıştırılmış veriyi yazma
    while (compressedData.length() % 8 != 0) {
        compressedData += '0'; // Veriyi 8 bit hanelere tamamlama
    }

    for (size_t i = 0; i < compressedData.length(); i += 8) {
        bitset<8> byte(compressedData.substr(i, 8));
        ofs << static_cast<char>(byte.to_ulong());
    }
    ofs.close();

    cout << "METIN SIKISTIRILDI VE '" << compressedFile << "' ADLI DOSYAYA KAYDEDILDI." << endl;
}

// Sıkıştırılmış veriyi Huffman algoritmasıyla çözen fonksiyon
void decompressText(const string& compressedFile, const string& outputFile) {
    // Adım 1: Kod tablosunu okuyun
    ifstream ifs(compressedFile, ios::binary);
    int tableSize;
    ifs >> tableSize;
    ifs.ignore();

    unordered_map<string, char> huffmanCodes;

    char ch;
    string code;
    while (tableSize > 0 && ifs.get(ch)) {
        if (ch != '\n') {
            code += ch;
        }
        else {
            char symbol = code[0];
            huffmanCodes[code.substr(1)] = symbol;
            code.clear();
            tableSize--;
        }
    }

    cout << "Kod Tablosu:" << endl;
    for (const auto& pair : huffmanCodes) {
        cout << pair.first << " : " << pair.second << endl;
    }
    cout << endl;

    // Adım 2: Sıkıştırılmış veriyi okuma
    string compressedData;
    while (ifs.get(ch)) {
        compressedData += bitset<8>(static_cast<unsigned char>(ch)).to_string();
    }
    ifs.close();

    cout << "SIKISTIRILMIS VERI:" << endl;
    cout << compressedData << endl << endl;

    // Adım 3: sıkıştırılmış veriyi bin dosyasından tekrar çözme
    string decompressedData;
    string currentCode;
    for (size_t i = 0; i < compressedData.length(); i++) {
        currentCode += compressedData[i];
        if (huffmanCodes.count(currentCode)) {
            decompressedData += huffmanCodes[currentCode];
            currentCode.clear();
        }
    }

    // Adım 4: çözülmüş veriyi yazdırma
    ofstream ofs(outputFile);
    ofs << decompressedData;
    ofs.close();

    cout << "SIKISTIRILMIS VERI COZULDU VE '" << outputFile << "' ADLI DOSYAYA KAYDEDILDI." << endl;
}


int main() {
    string inputFile = "inputtext.txt";
    string compressedFile = "compressedtext.bin";
    string decompressedFile = "decompressedtext.txt";

    // Metni sıkıştır
    compressText(inputFile, compressedFile);

    // Sıkıştırılmış veriyi çöz
    decompressText(compressedFile, decompressedFile);

    return 0;
}

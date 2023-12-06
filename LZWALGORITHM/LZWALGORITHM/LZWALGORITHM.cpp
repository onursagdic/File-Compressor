#include <iostream>
#include <fstream>
#include <unordered_map>
#include <vector>
#include <string>

using namespace std;

// Metni LZW algoritmasıyla sıkıştıran fonksiyon
vector<int> compressText(const string& text) {
    unordered_map<string, int> dictionary;
    int dictSize = 256; // İlk 256 karakter ASCII tablosundan

    // Sözlüğü oluşturma
    for (int i = 0; i < dictSize; i++) {
        string ch = "";
        ch += char(i);
        dictionary[ch] = i;
    }

    string current;
    vector<int> compressedData;

    for (char ch : text) {
        string next = current + ch;
        if (dictionary.find(next) != dictionary.end()) {
            // current + ch dizisi sözlükte mevcut
            current = next;
        }
        else {
            // current + ch dizisi sözlükte mevcut değil
            compressedData.push_back(dictionary[current]);
            dictionary[next] = dictSize++;
            current = string(1, ch);
        }
    }

    // Kalan kısım için sıkıştırılmış veriye ekleme yap
    if (!current.empty()) {
        compressedData.push_back(dictionary[current]);
    }

    return compressedData;
}

// Sıkıştırılmış veriyi LZW algoritmasıyla çözen fonksiyon
string decompressText(const vector<int>& compressedData) {
    unordered_map<int, string> dictionary;
    int dictSize = 256; // İlk 256 karakter ASCII tablosundan 

    // Sözlüğü oluştur
    for (int i = 0; i < dictSize; i++) {
        string ch = "";
        ch += char(i);
        dictionary[i] = ch;
    }

    string current = dictionary[compressedData[0]];
    string decompressedText = current;

    for (size_t i = 1; i < compressedData.size(); i++) {
        int code = compressedData[i];

        string entry;
        if (dictionary.find(code) != dictionary.end()) {
            entry = dictionary[code];
        }
        else if (code == dictSize) {
            entry = current + current[0];
        }
        else {
            throw runtime_error("SIKISITIRILMIS VERI BOZUK.");
        }

        decompressedText += entry;

        dictionary[dictSize++] = current + entry[0];

        current = entry;
    }

    return decompressedText;
}

int main() {
    string inputFile = "input.txt";
    string compressedFile = "compressed.bin";
    string decompressedFile = "decompressed.txt";

    // Metni okuma
    ifstream ifs(inputFile);
    string text((istreambuf_iterator<char>(ifs)), (istreambuf_iterator<char>()));
    ifs.close();

    // Metni sıkıştırma
    vector<int> compressedData = compressText(text);

    // Sıkıştırılmış veriyi dosyaya yazma
    ofstream ofs(compressedFile, ios::binary);
    for (int code : compressedData) {
        ofs.write(reinterpret_cast<const char*>(&code), sizeof(int));
    }
    ofs.close();

    cout << "METIN SIKISTIRILDI VE '" << compressedFile << "' ADLI DOSYAYA KAYDEDILDI" << endl;

    // Sıkıştırılmış veriyi dosyadan okuma
    ifstream ifs2(compressedFile, ios::binary);
    vector<int> compressedData2;
    int code;
    while (ifs2.read(reinterpret_cast<char*>(&code), sizeof(int))) {
        compressedData2.push_back(code);
    }
    ifs2.close();

    // Sıkıştırılmış veriyi çözme
    string decompressedText = decompressText(compressedData2);

    // Çözülmüş veriyi dosyaya yazma
    ofstream ofs2(decompressedFile);
    ofs2 << decompressedText;
    ofs2.close();

    cout << "SIKISTIRILMIS VERI COZULDU VE '" << decompressedFile << "' ADLI DOSYAYA KAYDEDILDI" << endl;

    cout << "OKUNAN METIN:" << endl;
    cout << text << endl;

    cout << "SIKISITIRILMIS VERI:" << endl;
    for (int code : compressedData) {
        cout << code << " ";
    }
    cout << endl;


    cout << "COZULEN METIN:" << endl;
    cout << decompressedText << endl;


    return 0;
}

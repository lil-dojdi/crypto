#include <iostream>
#include <string>

using namespace std;

const string ALPHABET = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

// Индексный шифр Цезаря (согласно псевдокоду 2.1)
string CSR_encrypt(const string& message, int key) {
    string encrypt = "";
    for (size_t index1 = 0; index1 < message.length(); index1++) {
        for (size_t index2 = 0; index2 < ALPHABET.length(); index2++) {
            if (message[index1] == ALPHABET[index2]) {
                if (index2 + key < ALPHABET.length()) {
                    encrypt += ALPHABET[index2 + key];
                } else {
                    encrypt += ALPHABET[index2 + key - ALPHABET.length()];
                }
            }
        }
    }
    return encrypt;
}

// Индексное дешифрование Цезаря
string CSR_decrypt(const string& message, int key) {
    string decrypt = "";
    for (size_t index1 = 0; index1 < message.length(); index1++) {
        for (size_t index2 = 0; index2 < ALPHABET.length(); index2++) {
            if (message[index1] == ALPHABET[index2]) {
                if ((int)index2 - key >= 0) {
                    decrypt += ALPHABET[index2 - key];
                } else {
                    decrypt += ALPHABET[index2 - key + ALPHABET.length()];
                }
            }
        }
    }
    return decrypt;
}

// Математическое шифрование Цезаря
string CSR_enc_math(const string& message, int key) {
    string encrypt = "";
    for (char m : message) {
        size_t mi = ALPHABET.find(m);
        if (mi != string::npos) {
            encrypt += ALPHABET[(mi + key) % 26];
        }
    }
    return encrypt;
}

// Математическое дешифрование Цезаря
string CSR_dec_math(const string& message, int key) {
    string decrypt = "";
    for (char m : message) {
        size_t mi = ALPHABET.find(m);
        if (mi != string::npos) {
            decrypt += ALPHABET[(mi - key + 26) % 26];
        }
    }
    return decrypt;
}

// Генерация подстановочного алфавита для CAESAR-KEY
string CSK_keygen(const string& keyword) {
    string key = keyword + ALPHABET;
    size_t n = key.length();
    
    for (size_t index1 = 0; index1 < n; index1++) {
        for (size_t index2 = index1 + 1; index2 < n; index2++) {
            if (key[index1] == key[index2]) {
                for (size_t index3 = index2; index3 < n - 1; index3++) {
                    key[index3] = key[index3 + 1];
                }
                n--;
                index2--;
            }
        }
    }
    return key.substr(0, n);
}

// Шифрование CAESAR-KEY
string CSK_encrypt(const string& message, const string& keyword) {
    string key = CSK_keygen(keyword);
    string encrypt = "";
    for (size_t index1 = 0; index1 < message.length(); index1++) {
        for (size_t index2 = 0; index2 < ALPHABET.length(); index2++) {
            if (message[index1] == ALPHABET[index2]) {
                encrypt += key[index2];
            }
        }
    }
    return encrypt;
}

// Шифрование Виженера
string VGN_encrypt(const string& message, const string& key) {
    string encrypt = "";
    size_t index3 = 0;
    for (size_t index1 = 0; index1 < message.length(); index1++) {
        size_t m1 = ALPHABET.find(message[index1]);
        size_t m2 = ALPHABET.find(key[index3]);
        
        if (m1 != string::npos && m2 != string::npos) {
            if (m1 + m2 < ALPHABET.length()) {
                encrypt += ALPHABET[m1 + m2];
            } else {
                encrypt += ALPHABET[m1 + m2 - ALPHABET.length()];
            }
        }
        
        if (index3 == key.length() - 1) {
            index3 = 0;
        } else {
            index3++;
        }
    }
    return encrypt;
}

int main() {
    string msg = "IWILLBEHEREONMONDAY";
    
    cout << "=== 1 & 4. Валидация CAESAR ===" << endl;
    cout << "CSR_encrypt (key 7):  " << CSR_encrypt(msg, 7) << endl;
    cout << "CSR_enc_math (key 7): " << CSR_enc_math(msg, 7) << endl;
    
    cout << "\n=== 6. Валидация CAESAR-KEY ===" << endl;
    cout << "CSK_encrypt (key MEETINGTIME): " << CSK_encrypt(msg, "MEETINGTIME") << endl;
    
    cout << "\n=== 10. Валидация VIGENERE ===" << endl;
    cout << "VGN_encrypt (key MTIME): " << VGN_encrypt(msg, "MTIME") << endl;

    return 0;
}
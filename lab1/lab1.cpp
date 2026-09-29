#include <iostream>
#include <string>

using namespace std;

// Функция-заглушка для процесса шифрования
void CSR_encrypt(const string& message) {
    cout << "encrypt function" << endl;
}

// Функция-заглушка для процесса дешифрования
void CSR_decrypt(const string& message) {
    cout << "decrypt function" << endl;
}

int main(int argc, char* argv[]) {
    // Проверка 1: Аргументы командной строки отсутствуют
    if (argc == 1) {
        cout << "crypto: no option and message" << endl;
        return 3;
    }
    
    // Проверка 2: Передан только один параметр из двух требуемых
    if (argc == 2) {
        cout << "crypto: no option or message" << endl;
        return 2;
    }
    
    // Проверка 3: Переданы два необходимых параметра
    if (argc == 3) {
        string option = argv[1];
        string message = argv[2];
        
        if (option == "-Ecsr") {
            CSR_encrypt(message);
        } 
        else if (option == "-Dcsr") {
            CSR_decrypt(message);
        } 
        else {
            cout << "crypto: no such option" << endl;
            return 1;
        }
        return 0;
    }
    
    // Проверка 4: Избыточные аргументы
    cout << "crypto: too many strings" << endl;
    return 4;
}
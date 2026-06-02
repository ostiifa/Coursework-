#include "Storage.h"
#include <iostream>

namespace PaymentCore {
    //запоминаем путь к файлу базы данных
    Storage::Storage(const std::string& dbPath) : filename(dbPath) {}

    // функция записи транзакции в бинарный файл
    bool Storage::writeTransaction(const TransactionRecord& record) {
        std::ofstream out(filename, std::ios::binary | std::ios::app);
        if (!out.is_open()) return false;
        
        out.write(reinterpret_cast<const char*>(&record), sizeof(TransactionRecord));
        return true;
    }

    // функция чтения транзакции по индексу (номеру)
    bool Storage::readTransaction(int index, TransactionRecord& record) {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) return false;
        
        // перемещаем указатель на нужную позицию в файле
        in.seekg(index * sizeof(TransactionRecord), std::ios::beg);
        if (in.fail()) return false;

        in.read(reinterpret_cast<char*>(&record), sizeof(TransactionRecord));
        return in.gcount() == sizeof(TransactionRecord);
    }
    // метод вычисления общего количества транзакций в файле
    int Storage::getTransactionsCount() {
        std::ifstream in(filename, std::ios::binary | std::ios::ate); // открываем и сразу перемещаем курсор в конец (ate)
        if (!in.is_open()) return 0;
        std::streampos fileSize = in.tellg(); // получаем размер файла в байтах
        in.close();
        return static_cast<int>(fileSize / sizeof(TransactionRecord));
    }
    // подсчет транзакций конкретного пользователя (для выделения точного объема памяти)
    int Storage::getUserTransactionsCount(int userId) {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) return 0;

        int count = 0;
        TransactionRecord temp;
        while (in.read(reinterpret_cast<char*>(&temp), sizeof(TransactionRecord))) {
            if (temp.senderId == userId || temp.receiverId == userId) {
                count++;
            }
        }
        return count;
    }
    bool Storage::getUserHistory(int userId, TransactionRecord*& outArray, int& outSize) {
        outSize = getUserTransactionsCount(userId);
        if (outSize == 0) {
            outArray = nullptr;
            return false;
        }

        // выделяем память в Куче под точное количество найденных транзакций
        outArray = new TransactionRecord[outSize];

        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) {
            delete[] outArray;
            outArray = nullptr;
            outSize = 0;
            return false;
        }

        TransactionRecord temp;
        int index = 0;

        while (in.read(reinterpret_cast<char*>(&temp), sizeof(TransactionRecord))) {
            if (temp.senderId == userId || temp.receiverId == userId) {
                outArray[index] = temp;
                index++;
            }
        }
        return true;
    }
    // компараторы для кастомной сортировки
    void Storage::sortTransactionsByTimestamp(TransactionRecord* arr, int size, bool ascending) {
        if (ascending) {
            // сортировка по возрастанию
            insertionSort(arr, size, [](const TransactionRecord& a, const TransactionRecord& b) {
                return a.timestamp > b.timestamp;
                });
        }
        else {
            // сортировка по убыванию
            insertionSort(arr, size, [](const TransactionRecord& a, const TransactionRecord& b) {
                return a.timestamp < b.timestamp;
                });
        }
        bool Storage::writeAccount(int index, const Account& acc) {
        std::ofstream out(filename, std::ios::binary | std::ios::in | std::ios::out);
        if (!out.is_open()) {
            // Если файла еще нет, создаем его чистым
            out.open(filename, std::ios::binary | std::ios::out);
            if (!out.is_open()) return false;
        }
        out.seekp(index * sizeof(Account), std::ios::beg);
        out.write(reinterpret_cast<const char*>(&acc), sizeof(Account));
        return out.good();
    }

    bool Storage::readAccount(int index, Account& acc) {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) return false;
        
        in.seekg(index * sizeof(Account), std::ios::beg);
        if (in.fail()) return false;
        
        in.read(reinterpret_cast<char*>(&acc), sizeof(Account));
        return in.gcount() == sizeof(Account);
    }
    double Storage::getTotalVolume() {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) return 0.0;

        double totalVolume = 0.0;
        TransactionRecord temp;
        while (in.read(reinterpret_cast<char*>(&temp), sizeof(TransactionRecord))) {
            if (temp.senderId != 0 && temp.receiverId != 0) {
                totalVolume += temp.amount;
            }
        }
        return totalVolume;
    }

    double Storage::auditUserBalance(int userId) {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) return 0.0;

        double incoming = 0.0;
        double outgoing = 0.0;
        TransactionRecord temp;

        while (in.read(reinterpret_cast<char*>(&temp), sizeof(TransactionRecord))) {
            if (temp.senderId == userId) {
                outgoing += temp.amount;
            }
            if (temp.receiverId == userId) {
                incoming += temp.amount;
            }
        }
        return incoming - outgoing;
    }

    bool Storage::validateStorageIntegrity() {
        std::ifstream in(filename, std::ios::binary | std::ios::ate);
        if (!in.is_open()) return true; 
        std::streampos fileSize = in.tellg();
        return (fileSize % sizeof(TransactionRecord)) == 0;
    }
}

#include "Storage.h"
#include <iostream>

namespace PaymentCore {
    // Конструктор: запоминаем путь к файлу базы данных
    Storage::Storage(const std::string& dbPath) : filename(dbPath) {}

    // Функция записи транзакции в бинарный файл
    bool Storage::writeTransaction(const TransactionRecord& record) {
        std::ofstream out(filename, std::ios::binary | std::ios::app);
        if (!out.is_open()) return false;
        
        out.write(reinterpret_cast<const char*>(&record), sizeof(TransactionRecord));
        return true;
    }

    // Функция чтения транзакции по индексу (номеру)
    bool Storage::readTransaction(int index, TransactionRecord& record) {
        std::ifstream in(filename, std::ios::binary);
        if (!in.is_open()) return false;
        
        // Перемещаем указатель на нужную позицию в файле
        in.seekg(index * sizeof(TransactionRecord), std::ios::beg);
        if (in.fail()) return false;

        in.read(reinterpret_cast<char*>(&record), sizeof(TransactionRecord));
        return in.gcount() == sizeof(TransactionRecord);
    }
    // Метод вычисления общего количества транзакций в файле
    int Storage::getTransactionsCount() {
        std::ifstream in(filename, std::ios::binary | std::ios::ate); // Открываем и сразу перемещаем курсор в конец (ate)
        if (!in.is_open()) return 0;
        std::streampos fileSize = in.tellg(); // Получаем размер файла в байтах
        in.close();
        return static_cast<int>(fileSize / sizeof(TransactionRecord));
    }
    // Подсчет транзакций конкретного пользователя (для выделения точного объема памяти)
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

        // Выделяем память в Куче под точное количество найденных транзакций
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
    // Компараторы для кастомной сортировки (лямбда-выражения или структуры-предикаты)
    void Storage::sortTransactionsByTimestamp(TransactionRecord* arr, int size, bool ascending) {
        if (ascending) {
            // Предикат: "левый элемент больше правого" -> меняем местами (сортировка по возрастанию)
            insertionSort(arr, size, [](const TransactionRecord& a, const TransactionRecord& b) {
                return a.timestamp > b.timestamp;
                });
        }
        else {
            // Предикат: "левый элемент меньше правого" -> меняем местами (сортировка по убыванию)
            insertionSort(arr, size, [](const TransactionRecord& a, const TransactionRecord& b) {
                return a.timestamp < b.timestamp;
                });
        }
    }
}

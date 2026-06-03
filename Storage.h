#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <fstream>
#include "Common.h"
#include "Account.h"

namespace PaymentCore {
    // структура для записи транзакции в файл
    struct TransactionRecord {
        int id;
        int senderId;
        int receiverId;
        double amount;
        long long timestamp;
        char currency[4];
    };

    // класс бинарного хранилища
    class Storage {
    private:
        std::string filename;
        // приватный шаблонный метод сортировки (сортировка вставками)
        // работает с любым типом данных и кастомным компаратором
        template<typename T, typename Compare>
        void insertionSort(T* arr, int size, Compare comp) {
            for (int i = 1; i < size; ++i) {
                T key = arr[i];
                int j = i - 1;
                while (j >= 0 && comp(arr[j], key)) {
                    arr[j + 1] = arr[j];
                    j--;
                }
                arr[j + 1] = key;
            }
        }
    public:
        explicit Storage(const std::string& dbPath);
        bool writeTransaction(const TransactionRecord& record);
        bool readTransaction(int index, TransactionRecord& record);
        int getTransactionsCount();
        int getUserTransactionsCount(int userId);
        // получение истории транзакций конкретного пользователя
        bool getUserHistory(int userId, TransactionRecord*& outArray, int& outSize);
        // публичный интерфейс для запуска сортировки истории по времени
        void sortTransactionsByTimestamp(TransactionRecord* arr, int size, bool ascending = true);
        bool writeAccount(int index, const Account& acc);
        bool readAccount(int index, Account& acc);
        double getTotalVolume();
        double auditUserBalance(int userId);
        bool validateStorageIntegrity();
    };
}

#endif

#ifndef STORAGE_H
#define STORAGE_H

#include <string>
#include <fstream>

namespace PaymentCore {
    // Структура для записи транзакции в файл
    struct TransactionRecord {
        int id;
        double amount;
        char currency[4];
    };

    // Класс бинарного хранилища
    class Storage {
    private:
        std::string filename;
    public:
        explicit Storage(const std::string& dbPath);
        bool writeTransaction(const TransactionRecord& record);
        bool readTransaction(int index, TransactionRecord& record);
    };
}

#endif

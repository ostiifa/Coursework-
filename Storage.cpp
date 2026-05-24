#include "Storage.h"

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
}

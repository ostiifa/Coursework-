#ifndef PAYMENTSYSTEM_H
#define PAYMENTSYSTEM_H

#include <string>
#include "Account.h"
#include "Storage.h"

namespace PaymentCore {

    class PaymentSystem {
    private:
        AccountNode* head;
        Storage accountStorage;    // работа с файлом accounts.bin
        Storage txStorage;         // работа с файлом transactions.bin
        int totalAccounts;         // общий счетчик записей на диске (включая неактивные)

        int getFileIndexById(int id); // поиск физического индекса в файле
        void loadFromStorage();       // загрузка активных счетов в ОЗУ при старте

    public:
        PaymentSystem(const std::string& dbPath, const std::string& txPath);
        ~PaymentSystem();

        bool registerNewAccount(int id, const char* name, double initialBalance);
        bool deposit(int id, double amount);
        bool withdraw(int id, double amount);
        bool transfer(int senderId, int receiverId, double amount);
        void displayAllActiveAccounts() const;
        bool logicalDeleteAccount(int id);
    };
}

#endif

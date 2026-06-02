#include "PaymentSystem.h"
#include <iostream>
#include <ctime>

namespace PaymentCore {

    PaymentSystem::PaymentSystem(const std::string& dbPath, const std::string& txPath)
        : head(nullptr), accountStorage(dbPath), txStorage(txPath), totalAccounts(0) {
        loadFromStorage(); 
    }

    PaymentSystem::~PaymentSystem() {
        clearList(head); 
    }

    void PaymentSystem::loadFromStorage() {
        clearList(head);
        totalAccounts = 0;
        Account temp;
        while (accountStorage.readAccount(totalAccounts, temp)) {
            if (temp.isActive) {
                addAccountNode(head, temp); // восстанавливаем список в ОЗУ
            }
            totalAccounts++;
        }
    }

    int PaymentSystem::getFileIndexById(int id) {
        Account temp;
        for (int i = 0; i < totalAccounts; ++i) {
            if (accountStorage.readAccount(i, temp)) {
                if (temp.id == id) return i;
            }
        }
        return -1; 
    }

    bool PaymentSystem::registerNewAccount(int id, const char* name, double initialBalance) {
        if (findAccountById(head, id) != nullptr || getFileIndexById(id) != -1) {
            std::cout << "[Ошибка] Аккаунт с ID " << id << " уже существует!" << std::endl;
            return false;
        }

        Account newAcc = createAccount(id, name, initialBalance);

        if (accountStorage.writeAccount(totalAccounts, newAcc)) {
            addAccountNode(head, newAcc);
            totalAccounts++;

            // логируем стартовый баланс как транзакцию пополнения
            if (initialBalance > 0) {
                TransactionRecord tx;
                tx.id = txStorage.getTransactionsCount() + 1;
                tx.senderId = 0; // 0 - маркер системы
                tx.receiverId = id;
                tx.amount = initialBalance;
                tx.timestamp = static_cast<long long>(std::time(nullptr));
                tx.currency[0] = 'B'; tx.currency[1] = 'Y'; tx.currency[2] = 'N'; tx.currency[3] = '\0';

                txStorage.writeTransaction(tx);
            }
            return true;
        }
        return false;
    }

    bool PaymentSystem::deposit(int id, double amount) {
        if (amount <= 0) return false;
        AccountNode* node = findAccountById(head, id);
        if (!node) return false;

        int idx = getFileIndexById(id);
        if (idx == -1) return false;

        node->data.balance += amount;
        accountStorage.writeAccount(idx, node->data);

        TransactionRecord tx = { txStorage.getTransactionsCount() + 1, 0, id, amount, static_cast<long long>(std::time(nullptr)), "BYN" };
        txStorage.writeTransaction(tx);
        return true;
    }

    bool PaymentSystem::withdraw(int id, double amount) {
        if (amount <= 0) return false;
        AccountNode* node = findAccountById(head, id);
        if (!node || node->data.balance < amount) return false;

        int idx = getFileIndexById(id);
        if (idx == -1) return false;

        node->data.balance -= amount;
        accountStorage.writeAccount(idx, node->data);

        TransactionRecord tx = { txStorage.getTransactionsCount() + 1, id, 0, amount, static_cast<long long>(std::time(nullptr)), "BYN" };
        txStorage.writeTransaction(tx);
        return true;
    }

    bool PaymentSystem::transfer(int senderId, int receiverId, double amount) {
        if (amount <= 0) return false;

        AccountNode* senderNode = findAccountById(head, senderId);
        AccountNode* receiverNode = findAccountById(head, receiverId);

        if (!senderNode || !receiverNode) {
            std::cout << "[Ошибка] Счет не найден в ОЗУ." << std::endl;
            return false;
        }

        if (senderNode->data.balance < amount) {
            std::cout << "[Ошибка] Недостаточно баланса." << std::endl;
            return false;
        }

        int senderIdx = getFileIndexById(senderId);
        int receiverIdx = getFileIndexById(receiverId);
        if (senderIdx == -1 || receiverIdx == -1) return false;

        senderNode->data.balance -= amount;
        receiverNode->data.balance += amount;

        accountStorage.writeAccount(senderIdx, senderNode->data);
        accountStorage.writeAccount(receiverIdx, receiverNode->data);

        TransactionRecord tx = { txStorage.getTransactionsCount() + 1, senderId, receiverId, amount, static_cast<long long>(std::time(nullptr)), "BYN" };
        txStorage.writeTransaction(tx);
        return true;
    }

    void PaymentSystem::displayAllActiveAccounts() const {
        AccountNode* current = head;
        std::cout << "\n=== СПИСОК АКТИВНЫХ СЧЕТОВ ===" << std::endl;
        if (!current) std::cout << "Система пуста." << std::endl;
        while (current != nullptr) {
            printAccount(current->data);
            current = current->next;
        }
    }

    bool PaymentSystem::logicalDeleteAccount(int id) {
        int idx = getFileIndexById(id);
        if (idx == -1) return false;

        Account temp;
        if (accountStorage.readAccount(idx, temp)) {
            temp.isActive = false;
            accountStorage.writeAccount(idx, temp);
        }
        loadFromStorage(); // пересобираем ОЗУ без этого аккаунта
        return true;
    }
}

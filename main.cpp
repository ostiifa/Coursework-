#include <iostream>
#include <clocale>
#include "PaymentSystem.h"
#include "Storage.h"

int getSafeIntInput() {
    int value;
    while (true) {
        std::cin >> value;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "[Ошибка ввода] Введите число: ";
        } else {
            std::cin.ignore(10000, '\n');
            return value;
        }
    }
}

double getSafeDoubleInput() {
    double value;
    while (true) {
        std::cin >> value;
        if (std::cin.fail() || value < 0) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "[Ошибка ввода] Введите положительное число: ";
        } else {
            std::cin.ignore(10000, '\n');
            return value;
        }
    }
}

int main() {
    std::setlocale(LC_ALL, "Russian");

    const std::string accountsDb = "accounts.bin";
    const std::string txDb = "transactions.bin";

    PaymentCore::PaymentSystem systemCore(accountsDb, txDb);
    PaymentCore::Storage txStorage(txDb);

    int choice = 0;
    while (true) {
        std::cout << "\n=== МЕНЮ ПЛАТЕЖНОЙ СИСТЕМЫ ===" << std::endl;
        std::cout << "1. Регистрация нового счета" << std::endl;
        std::cout << "2. Вывод активных счетов из ОЗУ" << std::endl;
        std::cout << "3. Пополнение баланса" << std::endl;
        std::cout << "4. Списание денежных средств" << std::endl;
        std::cout << "5. Перевод (Транзакция)" << std::endl;
        std::cout << "6. Просмотр отсортированной истории" << std::endl;
        std::cout << "7. Удаление счета" << std::endl;
        std::cout << "8. Финансовый аудит логов" << std::endl;
        std::cout << "9. Выход" << std::endl;
        std::cout << "Выберите пункт: ";
        
        choice = getSafeIntInput();
        if (choice == 9) break;

        switch (choice) {
            case 1: {
                std::cout << "Введите ID счета: "; int id = getSafeIntInput();
                std::cout << "Введите ФИО: "; char name[PaymentCore::MAX_NAME_LEN]; std::cin.getline(name, PaymentCore::MAX_NAME_LEN);
                std::cout << "Начальный баланс: "; double bal = getSafeDoubleInput();
                if (systemCore.registerNewAccount(id, name, bal)) std::cout << "[Успех] Создан.\n";
                break;
            }
            case 2:
                systemCore.displayAllActiveAccounts();
                break;
            case 3: {
                std::cout << "ID счета: "; int id = getSafeIntInput();
                std::cout << "Сумма: "; double amt = getSafeDoubleInput();
                if (systemCore.deposit(id, amt)) std::cout << "[Успех] Баланс пополнен.\n";
                break;
            }
            case 4: {
                std::cout << "ID счета: "; int id = getSafeIntInput();
                std::cout << "Сумма: "; double amt = getSafeDoubleInput();
                if (systemCore.withdraw(id, amt)) std::cout << "[Успех] Списано.\n";
                break;
            }
            case 5: {
                std::cout << "ID отправителя: "; int from = getSafeIntInput();
                std::cout << "ID получателя: "; int to = getSafeIntInput();
                std::cout << "Сумма: "; double amt = getSafeDoubleInput();
                if (systemCore.transfer(from, to, amt)) std::cout << "[Успех] Перевод выполнен.\n";
                break;
            }
            case 6: {
                std::cout << "Введите ID для выписки: "; int uid = getSafeIntInput();
                PaymentCore::TransactionRecord* history = nullptr; int size = 0;

                if (txStorage.getUserHistory(uid, history, size)) {
                    std::cout << "Сортировка: 1-Свежие сначала, 2-Старые сначала: ";
                    int sChoice = getSafeIntInput();
                    txStorage.sortTransactionsByTimestamp(history, size, (sChoice == 2));

                    for (int i = 0; i < size; ++i) {
                        std::time_t txTime = static_cast<std::time_t>(history[i].timestamp);
                        std::tm timeInfo;
                        char timeBuffer[20];
                        localtime_s(&timeInfo, &txTime); 
                        std::strftime(timeBuffer, sizeof(timeBuffer), "%d.%m.%Y %H:%M", &timeInfo);
                        std::cout << "Лог #" << history[i].id << " | Отправитель: " << history[i].senderId
                        << " -> Получатель: " << history[i].receiverId << " | Сумма: " << history[i].amount
                        << " " << history[i].currency << " | Время: " << timeBuffer << "\n";
                    }
                    delete[] history;
                }
                else {
                    std::cout << "Транзакций не найдено.\n";
                }
            break;
            }
            case 7: {
                std::cout << "ID для удаления: "; int id = getSafeIntInput();
                if (systemCore.logicalDeleteAccount(id)) std::cout << "[Успех] Удален.\n";
                break;
            }
            case 8: {
                if (!txStorage.validateStorageIntegrity()) {
                    std::cout << "[Критическая ошибка] Файл транзакций поврежден.\n";
                } else {
                    std::cout << "Общий оборот системы: " << txStorage.getTotalVolume() << " BYN\n";
                    std::cout << "Введите ID для проверки баланса по логам: "; int aid = getSafeIntInput();
                    std::cout << "Баланс строго по логам: " << txStorage.auditUserBalance(aid) << " BYN\n";
                }
                break;
            }
            default: std::cout << "Неверный пункт.\n";
        }
    }
    return 0;
}

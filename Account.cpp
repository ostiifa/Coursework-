#include "Account.h"
#include <iostream>
#include <cstring>

namespace PaymentCore {
	Account createAccount(int id, const char* name, double initialBalance) {//описание функции создания аккаунта
		Account newAcc;
		newAcc.id = id;
		newAcc.balance = initialBalance;
		newAcc.isActive = true;

		#ifdef _MSC_VER 
			strcpy_s(newAcc.ownerName, MAX_NAME_LEN, name);
		#else
			std::strcpy_s(newAcc.ownerName, name, MAX_NAME_LEN - 1);
			newAcc.ownerName[MAX_NAME_LEN - 1] = '\0';
		#endif

		return newAcc;
	}

	void printAccount(const Account& acc) {//печать аккаунта
		if (!acc.isActive) return;
		std::cout << "[Счет #" << acc.id << "] "
			<< "[Владелец: " << acc.ownerName << " | "
			<< "Баланс: " << acc.balance << " BYN" << std::endl;
	}

	void addAccountNode(AccountNode*& head, const Account& acc) {//добавление узла в кучу
		AccountNode* newNode = new AccountNode;
		newNode->data = acc;
		newNode->next = nullptr;

		if (head == nullptr) {
			head = newNode;
			return;
		}

		AccountNode* current = head;
		while (current->next != nullptr) {
			current = current->next;
		}
		current->next = newNode;
	}

	AccountNode* findAccountById(AccountNode* head, int id) {//поиск по id
		AccountNode* current = head;
		while (current != nullptr) {
			if (current->data.id == id && current->data.isActive) {
				return current;
			}
			current = current->next;
		}
		return nullptr;
	}

	void clearList(AccountNode*& head) {//очистка ОЗУ от списка
		AccountNode* current = head;
		int count = 0;
		while (current != nullptr) {
			AccountNode* nextNode = current->next;
			delete current;
			current = nextNode;
			count++;
		};
		head = nullptr;
		std::cout << "[System Log] Очищено узлов в памяти: " << count << std::endl;
	}

}
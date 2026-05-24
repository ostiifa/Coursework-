#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Common.h"

namespace PaymentCore {
	struct Account { //структура аккаунта
		int id;
		char ownerName[MAX_NAME_LEN];
		double balance;
		bool isActive;
	};

	struct AccountNode {//структура для динамического списка
		Account data;
		AccountNode* next;
	};

	Account createAccount(int id, const char* name, double initialBalance);//функция создания аккаунта
	void printAccount(const Account& acc);//вывод данных аккаунта
	void addAccountNode(AccountNode*& head, const Account& acc);//добавление аккаунта в динамический список
	AccountNode* findAccountById(AccountNode* head, int id);//поиск аккаунта по id
	void clearList(AccountNode*& head);//очистка динамической памяти

}

#endif

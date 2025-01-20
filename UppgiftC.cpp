#include <iostream>
#include <vector>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>  // För std::shuffle
#include <random>     // För slumpgenerator

class BankAccount {
    std::string accountNumber;
    float balance;
public:
    BankAccount() : balance(0) {}

    BankAccount(std::string accountNumber, float balance = 0)
        : accountNumber(accountNumber), balance(balance) {}

    std::string getAccountNumber() const {
        return accountNumber;
    }
};

// Gränssnitt för lagring
class IAccountStorage {
public:    
    virtual void addAccount(BankAccount account) = 0;
    virtual BankAccount* findAccount(std::string accountNumber) = 0;
};

// Lagringsklass för binärsökning
class BinarySearchStorage : public IAccountStorage {
    std::vector<BankAccount> accounts;

    // Binärsökningsfunktionen
    BankAccount* binarySearch(const std::string& accountNumber) {
        int left = 0;
        int right = accounts.size() - 1;

        while (left <= right) {
            int mid = left + (right - left) / 2;
            if (accounts[mid].getAccountNumber() == accountNumber) {
                return &accounts[mid];
            }
            else if (accounts[mid].getAccountNumber() < accountNumber) {
                left = mid + 1;
            }
            else {
                right = mid - 1;
            }
        }
        return nullptr;
    }

public:
    void addAccount(BankAccount account) override {
        accounts.push_back(account);
    }

    // Sortera konton innan sökningen
    void sortAccounts() {
        std::sort(accounts.begin(), accounts.end(), [](const BankAccount& a, const BankAccount& b) {
            return a.getAccountNumber() < b.getAccountNumber();
        });
    }

    BankAccount* findAccount(std::string accountNumber) override {
        return binarySearch(accountNumber);
    }
};

// Bankklass som använder lagringsgränssnittet
class Bank {
private:
    IAccountStorage* accountStorage;
public:
    Bank(IAccountStorage* storage) : accountStorage(storage) {}

    bool addAccount(std::string accountNumber) {
        BankAccount account(accountNumber);
        accountStorage->addAccount(account);
        return true;
    }

    BankAccount* getAccount(std::string accountNumber) {
        return accountStorage->findAccount(accountNumber);
    }
};

int main(int, char**) {
    BinarySearchStorage storage;
    Bank bank(&storage);

    const int AntalAccounts = 1000000;

    std::vector<std::string> accountNumbers;
    std::string sFirst = ""; 
    std::string sLast = ""; 
    std::string sNotFound = "9999999999"; // Använd ett kontonummer som inte finns

    // Skapa kontonummer och lägg till
    for (int i = 0; i < AntalAccounts; i++) {
        std::ostringstream oss;
        oss << std::setw(10) << std::setfill('0') << i;
        accountNumbers.push_back(oss.str());
    }

    // Blanda kontonumren för slumpmässigt
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(accountNumbers.begin(), accountNumbers.end(), g);

    // Spara det första och sista kontonumret för test
    sFirst = accountNumbers.front();
    sLast = accountNumbers.back();

    std::cout << "INITIALIZE: " << std::endl;
    auto startTime = std::chrono::high_resolution_clock::now();
    for (const auto& accountNumber : accountNumbers) {
        bank.addAccount(accountNumber);
    }
    auto endTime = std::chrono::high_resolution_clock::now();
    std::cout << "INIT Took: " << std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime).count() << " milliseconds" << std::endl;

    storage.sortAccounts();  // Sortera kontona för att kunna använda binärsökning

    // Testa att hämta första kontot
    startTime = std::chrono::high_resolution_clock::now();
    BankAccount* p = bank.getAccount(sFirst);
    endTime = std::chrono::high_resolution_clock::now();
    if (p != nullptr) {
        std::cout << p->getAccountNumber() << " took: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() << " nanoseconds" << std::endl;
    } else {
        std::cout << "Account not found (first)" << std::endl;
    }

    // Testa att hämta sista kontot
    startTime = std::chrono::high_resolution_clock::now();
    p = bank.getAccount(sLast);
    endTime = std::chrono::high_resolution_clock::now();
    if (p != nullptr) {
        std::cout << p->getAccountNumber() << " took: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() << " nanoseconds" << std::endl;
    } else {
        std::cout << "Account not found (last)" << std::endl;
    }

    // Testa att hämta ett konto som inte finns
    startTime = std::chrono::high_resolution_clock::now();
    p = bank.getAccount(sNotFound);
    endTime = std::chrono::high_resolution_clock::now();
    if (p == nullptr) {
        std::cout << "NOT FOUND took: " << std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime).count() << " nanoseconds" << std::endl;
    }
}
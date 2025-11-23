#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <map>
using namespace std;

#define MIN_BALANCE 500

class InsufficientFunds {};

class Account {
private:
    long accountNumber;
    string firstName;
    string lastName;
    float balance;
    static long NextAccountNumber;

public:
    Account() {}
    Account(string fname, string lname, float balance);

    long getAccNo() const { return accountNumber; }
    string getFirstName() const { return firstName; }
    string getLastName() const { return lastName; }
    float getBalance() const { return balance; }

    void Deposit(float amount);
    void Withdraw(float amount);

    static void setLastAccountNumber(long accountNumber);
    static long getLastAccountNumber();

    // File I/O operators
    friend ofstream& operator<<(ofstream& ofs, const Account& acc);
    friend ifstream& operator>>(ifstream& ifs, Account& acc);
    friend ostream& operator<<(ostream& os, const Account& acc);
};

long Account::NextAccountNumber = 0;

// ---------------- ACCOUNT IMPLEMENTATION ---------------- //

Account::Account(string fname, string lname, float balance) {
    NextAccountNumber++;
    accountNumber = NextAccountNumber;
    firstName = fname;
    lastName = lname;
    this->balance = balance;
}

void Account::Deposit(float amount) {
    balance += amount;
}

void Account::Withdraw(float amount) {
    if (balance - amount < MIN_BALANCE)
        throw InsufficientFunds();
    balance -= amount;
}

void Account::setLastAccountNumber(long accountNumber) {
    NextAccountNumber = accountNumber;
}

long Account::getLastAccountNumber() {
    return NextAccountNumber;
}

ofstream& operator<<(ofstream& ofs, const Account& acc) {
    ofs << acc.accountNumber << endl;
    ofs << acc.firstName << endl;
    ofs << acc.lastName << endl;
    ofs << acc.balance << endl;
    return ofs;
}

ifstream& operator>>(ifstream& ifs, Account& acc) {
    ifs >> acc.accountNumber;
    ifs >> acc.firstName;
    ifs >> acc.lastName;
    ifs >> acc.balance;
    return ifs;
}

ostream& operator<<(ostream& os, const Account& acc) {
    os << "First Name: " << acc.firstName << endl;
    os << "Last Name : " << acc.lastName << endl;
    os << "Account No: " << acc.accountNumber << endl;
    os << "Balance   : " << acc.balance << endl;
    return os;
}



class Bank {
private:
    map<long, Account> accounts;

public:
    Bank();
    Account OpenAccount(string fname, string lname, float balance);
    Account BalanceEnquiry(long accountNumber);
    Account Deposit(long accountNumber, float amount);
    Account Withdraw(long accountNumber, float amount);
    void CloseAccount(long accountNumber);
    void ShowAllAccounts();
    ~Bank();

private:
    void SaveAll();
};

// ---------------- BANK IMPLEMENTATION ---------------- //

Bank::Bank() {
    ifstream infile("Bank.data");
    if (!infile) return;

    Account acc;
    long lastAccNo = 0;

    while (infile >> acc) {
        accounts.insert({ acc.getAccNo(), acc });
        lastAccNo = acc.getAccNo();
    }

    Account::setLastAccountNumber(lastAccNo);
}

Account Bank::OpenAccount(string fname, string lname, float balance) {
    Account account(fname, lname, balance);
    accounts.insert({ account.getAccNo(), account });

    SaveAll();
    return account;
}

Account Bank::BalanceEnquiry(long accountNumber) {
    return accounts.at(accountNumber);
}

Account Bank::Deposit(long accountNumber, float amount) {
    accounts[accountNumber].Deposit(amount);
    SaveAll();
    return accounts[accountNumber];
}

Account Bank::Withdraw(long accountNumber, float amount) {
    accounts[accountNumber].Withdraw(amount);
    SaveAll();
    return accounts[accountNumber];
}

void Bank::CloseAccount(long accountNumber) {
    cout << "Account Deleted:\n" << accounts[accountNumber];
    accounts.erase(accountNumber);
    SaveAll();
}

void Bank::ShowAllAccounts() {
    for (auto& entry : accounts) {
        cout << "----------------------------\n";
        cout << entry.second;
    }
}

void Bank::SaveAll() {
    ofstream outfile("Bank.data", ios::trunc);
    for (auto& entry : accounts)
        outfile << entry.second;
}

Bank::~Bank() {
    SaveAll();
}



int main() {
    Bank b;
    int choice;
    string fname, lname;
    long accNo;
    float amount, balance;

    cout << "*** BANKING SYSTEM ***\n";

    do {
        cout << "\n1. Open Account"
             << "\n2. Balance Enquiry"
             << "\n3. Deposit"
             << "\n4. Withdraw"
             << "\n5. Close Account"
             << "\n6. Show All Accounts"
             << "\n7. Exit"
             << "\nEnter choice: ";
        cin >> choice;

        try {
            switch (choice) {
            case 1:
                cout << "Enter First Name: ";
                cin >> fname;
                cout << "Enter Last Name: ";
                cin >> lname;
                cout << "Enter Initial Balance: ";
                cin >> balance;
                cout << "\nAccount Created Successfully!\n";
                cout << b.OpenAccount(fname, lname, balance);
                break;

            case 2:
                cout << "Enter Account Number: ";
                cin >> accNo;
                cout << b.BalanceEnquiry(accNo);
                break;

            case 3:
                cout << "Enter Account Number: ";
                cin >> accNo;
                cout << "Enter Amount: ";
                cin >> amount;
                cout << b.Deposit(accNo, amount);
                break;

            case 4:
                cout << "Enter Account Number: ";
                cin >> accNo;
                cout << "Enter Amount: ";
                cin >> amount;
                cout << b.Withdraw(accNo, amount);
                break;

            case 5:
                cout << "Enter Account Number: ";
                cin >> accNo;
                b.CloseAccount(accNo);
                break;

            case 6:
                b.ShowAllAccounts();
                break;

            case 7:
                cout << "Exiting...\n";
                break;

            default:
                cout << "Invalid choice!\n";
            }
        }
        catch (InsufficientFunds&) {
            cout << "Error: Insufficient balance to withdraw.\n";
        }
        catch (exception& ex) {
            cout << "Error: " << ex.what() << endl;
        }

    } while (choice != 7);

    return 0;
}

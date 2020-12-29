#include <iostream>
#include <regex>
#include <atm/Controller.hpp>

using namespace std;

Controller c;

void PressEnterToContinue()
{
    cout<<"Press ENTER to continue... ";
    cin.clear();
    cin.ignore(256,'\n');
    cin.get();
}

string getCardNum() {
    string num;
    cout<<"Please insert your card or manually enter your card number: ";
    cin>>num;
    auto accounts = c.getCustomerAccounts(num);
    while(accounts.size()==0) {
        cout<<"Account not found, please try again: ";
        cin.clear();
        cin.ignore(256,'\n');
        cin >> num;
        accounts = c.getCustomerAccounts(num);
    }
    return num;
}

void validatePin(string cardNum) {
    string pin;
    cout<<"Enter PIN: ";
    cin>>pin;
    while (!c.validatePin(cardNum, pin)) {
        cout<<"Incorrect, enter PIN: ";
        cin>>pin;
    }
}

string selectAccount(string cardNum) {
    int option;
    auto accounts = c.getCustomerAccounts(cardNum);
    cout<<"Here is a list of your accounts"<<endl;
    for (int i=0;i<accounts.size();i++) {
        cout<<i+1<<". "<<accounts[i]<<endl;
    }
    cout<<"Select Account [1-"<<accounts.size()<<"]: ";
    cin>>option;
    while(cin.fail() || option<1 || option > accounts.size()) {
        cout<<"Please enter a number between 1 and "<<accounts.size()<<": ";
        cin.clear();
        cin.ignore(256,'\n');
        cin >> option;
    }

    cout<<"Account "<<accounts[option-1]<<" selected."<<endl;
    return accounts[option-1];
}

void deposit(string cardNum, string account) {
    int amount;
    cout<<"Amount: ";
    cin>>amount;
    while (cin.fail() || amount<0) {
        cout<<"Amount must be a positive integer: ";
        cin.clear();
        cin.ignore(256,'\n');
        cin >> amount;
    }
    c.deposit(cardNum, account, amount);
    cout<<"Deposited $"<<amount<<endl;
    PressEnterToContinue();
}

bool withdraw(string cardNum, string account) {
    int amount;
    cout<<"Amount: ";
    cin>>amount;
    while (cin.fail() || amount<0) {
        cout<<"Amount must be a positive integer: ";
        cin.clear();
        cin.ignore(256,'\n');
        cin >> amount;
    }
    bool success = c.withdraw(cardNum, account, amount);
    if (success) {
        cout<<"Withdrew $"<<amount<<endl;
    } else {
        cout<<"Not enough money in account."<<endl;
    }
    PressEnterToContinue();
    return success;
}

void selectAction(string cardNum, string account) {
    while (true) {
        int option;
        cout<<"1. See Balance"<<endl;
        cout<<"2. Deposit"<<endl;
        cout<<"3. Withdraw"<<endl;
        cout<<"4. Exit"<<endl;
        cout<<"Select an option [1-4]: ";
        cin>>option;
        while(cin.fail() || option<1 || option > 4) {
            cout<<"Select an option [1-4]: ";
            cin.clear();
            cin.ignore(256,'\n');
            cin >> option;
        }
        if (option==1) {
            cout<<"Balance: $"<<c.getBalance(cardNum, account)<<endl;
            PressEnterToContinue();
        } else if (option==2) {
            deposit(cardNum, account);
        } else if (option==3) {
            withdraw(cardNum, account);
        } else if (option==4) {
            break;
        }
    }
}

int main() {
	string cardNum;
    vector<string> accounts;
    cout<<"Welcome to the ATM user interface!"<<endl;

    // insert card
    cardNum=getCardNum();
    // enter pin
    validatePin(cardNum);
    cout<<"Welcome "<<c.getName(cardNum)<<"!"<<endl;
    // select account
    string account=selectAccount(cardNum);
    // select action
    selectAction(cardNum, account);

    return 0;
}
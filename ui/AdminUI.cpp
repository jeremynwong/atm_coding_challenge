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

void addCustomer() {
	unordered_map<string, unordered_map<string, string>> custFieldsInfo;
	vector<string> custFields=c.getCustomerFields(custFieldsInfo);
	unordered_map<string, string> custInfo;
	for (auto field : custFields) {
		string input;
		do {
			cout<<field;
			if (custFieldsInfo[field]["hint"]!="")
				cout<<" ("+custFieldsInfo[field]["hint"]+")";
			cout<<": ";
			cin>>input;
		} while (!regex_match (input, regex(custFieldsInfo[field]["regex"])));
		custInfo[field]=input;
	}
	c.addCustomer(custInfo);
    PressEnterToContinue();
}

string getCardNum() {
    string num;
    cout<<"Please insert your card or manually enter your card number: ";
    cin>>num;
    while(!c.customerExists(num)) {
        cout<<"Account not found, please try again: ";
        cin.clear();
        cin.ignore(256,'\n');
        cin >> num;
    }
    return num;
}

void listAccounts(string cardNum) {
    auto accounts = c.getCustomerAccounts(cardNum);
    cout<<"List of accounts"<<endl;
    for (int i=0;i<accounts.size();i++) {
        cout<<i+1<<". "<<accounts[i]<<endl;
    }
    cout<<"Add new account? [y/n] ";
    char character;
    cin>>character;
    if (character=='y' || character=='Y') {
        c.addAccount(cardNum);
        PressEnterToContinue();
    }
}

void listCustomers() {
    auto customers = c.getCustomerList();
    if (customers.size()==0) {
        cout<<"No customers!"<<endl;
    } else {
        for (int i=0;i<customers.size();i++) {
            cout<<i+1<<". "<<customers[i]<<endl;
        }
    }
    PressEnterToContinue();
}

int main() {
    cout<<"Welcome to the banking admin interface!"<<endl;
    while (true) {
    	int option;
        cout<<"Please select one of the following options."<<endl;
        cout<<"1. Add new customer"<<endl;
        cout<<"2. Customer list"<<endl;
        cout<<"3. Access customer"<<endl;
        cout<<"4. Exit"<<endl;
        cout<<"Select a number [1-4]: ";
        cin>>option;
        while(cin.fail() || option<1 || option > 4) {
            cout<<"Select a number [1-4]: ";
            cin.clear();
            cin.ignore(256,'\n');
            cin >> option;
        }
        if (option==1)
        	addCustomer();
        else if (option==2) {
            listCustomers();
        }
        else if (option==3) {
            string cardNum=getCardNum();
            listAccounts(cardNum);
        }
        else if (option==4) {
            break;
        }
    }
    return 0;
}
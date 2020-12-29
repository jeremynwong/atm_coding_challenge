#include <atm/Controller.hpp>
#include <iostream>

Controller::Controller() {
	redis = new Redis("tcp://127.0.0.1:6379"); // default local redis server
	setup();
}

Controller::Controller(std::string ip, int port) {
	redis = new Redis("tcp://"+ip+":"+std::to_string(port));
	setup();
}

void Controller::setup() {
	if(!keyExists("custFields")) {
		// these are the fields that are required for each customer along with a hint and the format
		// more can be added as desired such as address, date of birth, etc.
		std::vector<std::string> custFields = {"First Name", "Last Name", "Pin"};
		std::unordered_map<std::string, std::unordered_map<std::string, std::string>> custFieldsInfo = {
			{"First Name", {{"hint", ""},{"regex",".*"}}},
			{"Last Name", {{"hint", ""},{"regex",".*"}}},
			{"Pin", {{"hint", "4 digits"},{"regex","^[0-9]{4}$"}}},
		};

		redis->rpush("custFields", custFields.begin(), custFields.end());
		for (auto field : custFields) {
			std::unordered_map<std::string, std::string> m = {
				{"regex", custFieldsInfo[field]["regex"]},
				{"hint", custFieldsInfo[field]["hint"]}
			};
			redis->hmset("custFields:"+field, m.begin(), m.end());
		}
	}
	if (!keyExists("accountFields")) {
		// these are the fields that are required for an account along with the default value at account openning
		// more can be added such as daily withdrawal limit, etc.

		// {field, default val}
		std::unordered_map<std::string, std::string> m = {
	        {"Balance", "0"}
	    };
		redis->hmset("accountFields", m.begin(), m.end());
	}
}

bool Controller::keyExists(std::string key) {
	bool exists = redis->command<long long>("exists", key);
	return exists;
}

std::string Controller::genNewCustId() {
	int min=100000000;
	int max=999999999;
	std::string id;
	srand((unsigned) time(0));
	do {
		id=std::to_string(rand()%(max-min + 1) + min);
	} while (keyExists("customer:"+id+":info"));
	return id;
}

std::string Controller::genNewAccountId(std::string custId) {
	int min=100;
	int max=999;
	std::string id;
	srand((unsigned) time(0));
	do {
		id=std::to_string(rand()%(max-min + 1) + min);
	} while (keyExists("customer:"+custId+":"+id));
	return id;
}

void Controller::addCustomer(std::unordered_map<std::string, std::string> custInfo) {
	std::string custId=genNewCustId();
	redis->hmset("customer:"+custId+":info", custInfo.begin(), custInfo.end());
	redis->rpush("customerList", {custId});
	std::cout<<"New Customer Card Number: "<<custId<<std::endl;
	addAccount(custId);
}

void Controller::addAccount(std::string custId) {
	std::string accountId=genNewAccountId(custId);
	redis->rpush("customer:"+custId+":accounts", {accountId});

	std::unordered_map<std::string, std::string> m;
	redis->hgetall("accountFields", std::inserter(m, m.begin()));
	redis->hmset("account:"+custId+":"+accountId, m.begin(), m.end());
	std::cout<<"New Account ID: "<<accountId<<std::endl;
}

std::vector<std::string> Controller::getCustomerFields(std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& custFieldsInfo) {
	std::vector<std::string> custFields;

	redis->lrange("custFields", 0, -1, std::back_inserter(custFields));
	for (auto field : custFields) {
		std::unordered_map<std::string, std::string> m;
		redis->hgetall("custFields:"+field,std::inserter(m, m.begin()));
		custFieldsInfo[field]=m;
	}
	return custFields;
}

std::unordered_map<std::string, std::string> Controller::getAccountFields() {
	std::unordered_map<std::string, std::string> m;
	redis->hgetall("accountFields",std::inserter(m, m.begin()));
	return m;
}

std::vector<std::string> Controller::getCustomerAccounts(std::string custId) {
	std::vector<std::string> accounts;
	if (!keyExists("customer:"+custId+":accounts"))
		return accounts;
	redis->lrange("customer:"+custId+":accounts", 0, -1,std::back_inserter(accounts));
	return accounts;
}

std::vector<std::string> Controller::getCustomerList() {
	std::vector<std::string> customers;
	redis->lrange("customerList",0,-1,std::back_inserter(customers));
	return customers;
}

bool Controller::validatePin(std::string custId, std::string pin) {
	auto actualPin = redis->hget("customer:"+custId+":info", "Pin");
	return *actualPin==pin;
}

std::string Controller::getName(std::string custId) {
	auto first = redis->hget("customer:"+custId+":info", "First Name");
	auto last = redis->hget("customer:"+custId+":info", "Last Name");
	return *first+" "+*last;
}

int Controller::getBalance(std::string custId, std::string account) {
	auto bal = redis->hget("account:"+custId+":"+account,"Balance");
	return std::stoi(*bal);
}

void Controller::deposit(std::string custId, std::string account, int amount) {
	redis->hincrby("account:"+custId+":"+account,"Balance",(long long)amount);
}

bool Controller::withdraw(std::string custId, std::string account, int amount) {
	int bal = getBalance(custId, account);
	if (amount>bal || amount<0) return false;
	redis->hincrby("account:"+custId+":"+account,"Balance",(long long)(-amount));
	return true;
}
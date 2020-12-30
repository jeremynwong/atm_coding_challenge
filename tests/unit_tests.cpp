#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <atm/Controller.hpp>

using namespace sw::redis;

bool keyExists(Redis& redis, std::string key) {
	bool exists = redis.command<long long>("exists", key);
	return exists;
}

TEST_CASE( "Database is setup correctly", "[setup]" ) {
	Redis redis("tcp://127.0.0.1:6379/1"); // test on database 1
	redis.flushdb();

	SECTION( "Database gets setup correctly if settings don't exist" ) {
		Controller c("127.0.0.1",6379, 1);

		REQUIRE(keyExists(redis, "custFields"));
		REQUIRE(keyExists(redis, "accountFields"));
	}

	SECTION("Existing settings in database aren't overwritten") {
		redis.set("custFields", "test1");
		redis.set("accountFields", "test2");

		Controller c("127.0.0.1",6379, 1); // test on database 1

		REQUIRE(*redis.get("custFields")=="test1");
		REQUIRE(*redis.get("accountFields")=="test2");
		redis.flushdb();
	}
}

TEST_CASE( "Function customerExists returns true if customer exists, false otherwise", "[customerExists]" ) {
	Redis redis("tcp://127.0.0.1:6379/1");
	redis.flushdb();
	Controller c("127.0.0.1",6379, 1);

	std::string custId="123456789";
	
	SECTION("Customer doesn't exist") {
		REQUIRE(!c.customerExists(custId));
	}
	
	SECTION("Customer exists") {
		redis.set("customer:"+custId+":info", "customerInfo");
		REQUIRE(c.customerExists(custId));
	}
}

TEST_CASE( "Get list of customers", "[getCustomerList]" ) {
	Redis redis("tcp://127.0.0.1:6379/1");
	Controller c("127.0.0.1",6379, 1);

	std::string custId="123456789";
	redis.rpush("customerList", {custId});
	REQUIRE(c.getCustomerList().back()==custId);
}

TEST_CASE( "Get list of accounts", "[getCustomerAccounts]" ) {
	Redis redis("tcp://127.0.0.1:6379/1");
	redis.flushdb();
	Controller c("127.0.0.1",6379, 1);

	std::string custId="123456789";
	std::string accountId="123";
	REQUIRE(c.getCustomerAccounts(custId).size()==0);
	redis.rpush("customer:"+custId+":accounts", {accountId});
	REQUIRE(c.getCustomerAccounts(custId).size()==1);
	REQUIRE(c.getCustomerAccounts(custId).back()==accountId);
}

TEST_CASE( "New customer is created succesfully with 1 account", "[addCustomer]" ) {
	Redis redis("tcp://127.0.0.1:6379/1");
	Controller c("127.0.0.1",6379, 1);

	std::unordered_map<std::string, std::string> custInfo = {{"First Name", "First"}, {"Last Name", "Last"}, {"Pin", "1234"}};
	int numCust=c.getCustomerList().size();
	std::string custId=c.addCustomer(custInfo);
	REQUIRE(c.getCustomerList().size()==numCust+1);
	REQUIRE(c.customerExists(custId));
	REQUIRE(c.getCustomerAccounts(custId).size()==1);
}

TEST_CASE( "New account is succesfully added", "[addAccount]" ) {
	Redis redis("tcp://127.0.0.1:6379/1");
	Controller c("127.0.0.1",6379, 1);

	std::unordered_map<std::string, std::string> custInfo = {{"First Name", "First"}, {"Last Name", "Last"}, {"Pin", "1234"}};
	std::string custId=c.addCustomer(custInfo);
	std::string account = c.addAccount(custId);
	REQUIRE(c.getCustomerAccounts(custId).size()==2);
	REQUIRE(c.getCustomerAccounts(custId).back()==account);
}

TEST_CASE( "Function validatePin returns true when pin correct, false otherwise", "[validatePin]" ) {
	Redis redis("tcp://127.0.0.1:6379/1");
	Controller c("127.0.0.1",6379, 1);

	std::string pin="1234";
	std::unordered_map<std::string, std::string> custInfo = {{"First Name", "First"}, {"Last Name", "Last"}, {"Pin", pin}};
	std::string custId=c.addCustomer(custInfo);
	REQUIRE(c.validatePin(custId, pin));
	REQUIRE(!c.validatePin(custId, "4321"));
}

TEST_CASE( "Name is correctly retrieved", "[getName]" ) {
	Redis redis("tcp://127.0.0.1:6379/1");
	Controller c("127.0.0.1",6379, 1);

	std::string firstName="First";
	std::string lastName="Last";
	std::unordered_map<std::string, std::string> custInfo = {{"First Name", firstName}, {"Last Name", lastName}, {"Pin", "1234"}};
	std::string custId=c.addCustomer(custInfo);
	REQUIRE(c.getName(custId)==firstName+" "+lastName);
}

TEST_CASE( "Check balance returns correct balance", "[getBalance]" ) {
	Redis redis("tcp://127.0.0.1:6379/1");
	Controller c("127.0.0.1",6379, 1);

	std::unordered_map<std::string, std::string> custInfo = {{"First Name", "First"}, {"Last Name", "Last"}, {"Pin", "1234"}};
	std::string custId=c.addCustomer(custInfo);
	std::string accountId=c.getCustomerAccounts(custId).back();
	REQUIRE(c.getBalance(custId, accountId)==0);
	redis.hset("account:"+custId+":"+accountId,"Balance","100");
	REQUIRE(c.getBalance(custId, accountId)==100);
}

TEST_CASE( "Deposit and withdraw money updates balance", "[deposit, withdraw]" ) {
	Redis redis("tcp://127.0.0.1:6379/1");
	Controller c("127.0.0.1",6379, 1);

	std::unordered_map<std::string, std::string> custInfo = {{"First Name", "First"}, {"Last Name", "Last"}, {"Pin", "1234"}};
	std::string custId=c.addCustomer(custInfo);
	std::string accountId=c.getCustomerAccounts(custId).back();

	// can't deposit negative amount
	REQUIRE(!c.deposit(custId, accountId,-1));
	REQUIRE(c.getBalance(custId, accountId)==0);

	// deposit postive amount updates balance
	REQUIRE(c.deposit(custId, accountId,10));
	REQUIRE(c.getBalance(custId, accountId)==10);

	// can't withdraw negative amount or more than available balance
	REQUIRE(!c.withdraw(custId, accountId,-1));
	REQUIRE(!c.withdraw(custId, accountId,11));
	REQUIRE(c.getBalance(custId, accountId)==10);

	// withdraw amount less than balance updates balance
	REQUIRE(c.withdraw(custId, accountId,5));
	REQUIRE(c.getBalance(custId, accountId)==5);
}
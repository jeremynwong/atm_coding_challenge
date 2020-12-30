# ATM Controller
ATM controller for the Bear Robotics coding challenge

## Setup
### Install Dependencies
#### Redis
[Redis](https://redis.io/) is used as the database for this project. Start by installing Redis using these [instructions](https://redis.io/download) or install from the official Ubuntu PPA using the steps below.
```
sudo add-apt-repository ppa:redislabs/redis
sudo apt-get update
sudo apt-get install redis
```
#### redis-plus-plus
The redis-plus-plus library is used to interface with the Redis database using C++. You can follow these [installation instructions](https://github.com/sewenew/redis-plus-plus#installation) or see the summary below.

First, install hiredis. This is the Redis library for C that redis-plus-plus depends on.
```
git clone https://github.com/redis/hiredis.git
cd hiredis
make
make install
```
Next, install redis-plus-plus.
```
git clone https://github.com/sewenew/redis-plus-plus.git
cd redis-plus-plus
mkdir compile
cd compile
cmake -DCMAKE_BUILD_TYPE=Release ..
make
make install
```

### Clone and Build Project
Navigate to the location you would like to clone the project.
```
git clone https://github.com/jeremynwong/atm_coding_challenge.git
cd atm_coding_challenge
mkdir build && cd build
cmake ..
make
```

## How to Use
A Redis server must first be started. In the UI executables and unit tests, we assume that the server is started using the default settings on the host computer. If you wish to use another server, these settings can be passed as parameters when constructing the Controller object.
```
redis-server
```

### User Interface
There are two executables. The admin executable is used by bank employees to add new customers and create new accounts for existing customers. When a new customer is created, remember the card number as this will be needed later on. The list of customers (card numbers) can also be accessed from the admin UI. The second is the user executable which is what the user will interface with to do their banking.

To run the user interface, from the build folder, run
```
./ui/admin
```
or
```
./ui/user
```

### Run Tests
Tests are automatically run using database 1 whereas the actual live data is on database 0 so that they do not affect each other.
To run tests using CMake, in the build directory, run
```
make test
```
For a more detailed report, you can also directly run the unit test executable from the build directory.
```
./tests/atm_unit_tests
```


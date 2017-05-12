# Server Setup #

Set Up
======
Before we can compile the server, we need to get the dependencies

First, ssh into your account on code.engineering.nyu.edu -- your server will be [your_username_].code.engineering.edu

Proceeed to follow the rest of these instructions

Installing CMake
----------------
The server uses cmake to compile C++ files so we will need to install CMake. 

* Log in to your nyu server `____@code.engineering.nyu.edu`

* Download the cmake 3.8.0 binary with `wget https://cmake.org/files/v3.8/cmake-3.8.0-rc2-Linux-x86_64.sh`

* Add permissions: `chmod +x cmake-3.8.0-rc2-Linux-x86_64.sh`

* Run it: `bash cmake-3.8.0-rc2-Linux-x86_64.sh`

* It'll show you the license agreement, scroll down with space and enter `y`

* Enter `y` when Cmake asks about installation path

* By default, you bash shell will use the cmake installed in the system. 

* Change it to the most updated one by appending the following commands:

		* 'cd ~'

		* `nano .bashrc`

		* Append the line at the bottom `export PATH=$HOME/cmake-3.8.0-rc2-Linux-x86_64/bin:$PATH`

* Ensure you are using the right version by running `cmake --version` and that it says `cmake version 3.8.0`


Installing Boost
----------------
One of the dependencies that the HTTP Server uses is Boost. To install boost, follow the boost guide under the helpful links section or follow this:

* Make sure you're in your home directory: `cd ~`

* Run command: `wget https://sourceforge.net/projects/boost/files/boost/1.63.0/boost_1_63_0.tar.gz`

* Unzip file: `tar xzvf __file__` where file is file downloaded

* Go to file folder `cd __folder__` where folder is the unzipped file's folder

* Run `./bootstrap.sh --prefix=~/boost_1_63_0`

* Run `./b2 install`

* You will receive some errors at the end, ignore these for now


Generating an SSL certificate
-----------------------------
* `cd ~`

* `mkdir certificate && cd certificate`

* Generate a private key (with password) `openssl genrsa -des3 -out server.key 1024`

* Generate a CSR `openssl req -new -key server.key -out server.csr`

* Remove password `cp server.key server.key.org && openssl rsa -in server.key.org -out server.key`

* Generate certificate `openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt`

We now have server.key and server.crt ready to use


Mongo C Driver Installation
--------------------
This is required for the MongoCXX Driver

* Change to your home directory: `cd ~`

* Download Mongo C Driver: `git clone https://github.com/mongodb/mongo-c-driver.git`

* Switch to the directory: `cd mongo-c-driver`

* Specify the version: `git checkout 1.6.1`

* Configure: `./autogen.sh --with-libbson=bundled --disable-automatic-init-and-cleanup --prefix=/home/SPRING2017/[user]/mongo-c-driver` where [user] is your username

* Run: `make && make install`


MongoCXX Driver Installation
---------------------

* Change to home directory: `cd ~`

* Download MongoCXX Driver: `wget https://github.com/mongodb/mongo-cxx-driver/archive/r3.0.3.tar.gz`

* Unpack tarball: `tar xvzf r3.0.3.tar.gz`

* Change directory: `cd mongo-cxx-driver-r3.0.3/build`

* Run cmake: `cmake -DCMAKE_INSTALL_PREFIX=~/mongo-cxx-driver-r3.0.3 -DLIBBSON_DIR=~/mongo-c-driver -DLIBMONGOC_DIR=~/mongo-c-driver -DBOOST_ROOT=~/boost_1_63_0 ..`

* Run: `make && make install`


Server Setup
------------
A few notes:

* When working with the server, please refer to [Simple Web Server](https://github.com/eidheim/Simple-Web-Server) for documentation and details. 

To begin setup:

* Clone the server from Bitbucket

* Connect to NYU's server using Filezilla or Sublime SFTP. Filezilla is better if you just want to be on the safe side and you're new. Check out `http://code.engineering.nyu.edu/ssh.html` for further instructions.

* Transfer our `Server` folder into NYU's server.

* Connect to NYU's server via terminal: `ssh __name__@code.engineering.nyu.edu`

* Change directory: `cd Server`

* Delete the current build directory: `rm -rf build/`

* Run `mkdir build` 

* Change directory: `cd build`

* Run cmake: `cmake ..`

* Make file: `make`

* Change directory again: `cd ..`

* Copy the certificate over: `cp ~/certificate/server.crt .`

* Copy the certificate over: `cp ~/certificate/server.key .`

* Run the server `./build/HTTPServer`

* It'll ask for your password if you set one - as of 3/19/17, there is no "indication" that the server is running but if you haven't encountered any errors and the process is just running, you're good.



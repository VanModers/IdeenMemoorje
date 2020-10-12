#!/bin/sh

set -e
# Any subsequent(*) commands which fail will cause the shell script to exit immediately

sudo -n true
test $? -eq 0 || exit 1 "you should have sudo priveledge to run this script"

echo "###Installing necessary libraries..."

sudo apt-get update

echo ''
echo '###Installing Git...'
sudo apt-get install git -y

echo ''
echo '###Installing libssl...'
sudo apt install libssl1.0-dev -y

echo ''
echo '###Installing libharu...'
wget https://github.com/libharu/libharu/archive/RELEASE_2_3_0.tar.gz
tar -xvzf RELEASE_2_3_0.tar.gz
cd libharu-RELEASE_2_3_0
set +e
sudo ./buildconf.sh --force
set -e
sudo ./buildconf.sh --force
./configure
make clean
make
sudo make install
cd ..

echo ''
echo '###Installing GCC...'
sudo apt-get install gcc

echo ''
echo '###Compiling IdeenMemoorje...'
cd Header/src
gcc -Wall -c "civetweb.c" 
g++ -Wall -c "CivetServer.cpp" -ldl -lrt -lcrypto -lssl -lpthread
cd ..
cd ..
g++ -Wall -c "CodeGenerator.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "LoginHandler.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "GarbageCollector.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "FormGenerator.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "PDFCreator.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "FileHandler.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "ConsoleHandler.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "FormHandler.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "AdminCommunicationHandler.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "AccountCommunicationHandler.cpp" -ldl -lrt -lcrypto -lssl -lpthread
g++ -Wall -c "main.cpp" -ldl -lrt -lcrypto -lssl -lpthread

echo ''
echo '###Linking IdeenMemoorje...'
g++ Header/src/civetweb.o Header/src/CivetServer.o CodeGenerator.o LoginHandler.o GarbageCollector.o FormGenerator.o PDFCreator.o FileHandler.o ConsoleHandler.o FormHandler.o AdminCommunicationHandler.o AccountCommunicationHandler.o main.o -o IdeenMemoorje -ldl -lrt -lcrypto -lssl -lpthread -lstdc++fs -lhpdf -Wl,-rpath -Wl,/usr/local/lib

echo ''
echo '###Finished: IdeenMemoorje can be started by typing sudo ./IdeenMemoorje [seed for authorisation link] [1 == http only]'

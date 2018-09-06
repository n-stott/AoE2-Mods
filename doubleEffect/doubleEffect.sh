rm doubleEffect
cd ..
g++  doubleEffect/main.cpp -o doubleEffect/doubleEffect -Iinclude -std=c++11 -L. -lgenieutils -lboost_iostreams 
cd doubleEffect
./doubleEffect

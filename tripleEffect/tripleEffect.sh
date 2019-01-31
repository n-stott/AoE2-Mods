rm tripleEffect
cd ..
g++  tripleEffect/main.cpp -o tripleEffect/tripleEffect -Iinclude -std=c++11 -L. -lgenieutils -lboost_iostreams 
cd tripleEffect
./tripleEffect

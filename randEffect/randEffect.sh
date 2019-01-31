rm randEffect
cd ..
g++  randEffect/main.cpp -o randEffect/randEffect -Iinclude -std=c++11 -L. -lgenieutils -lboost_iostreams 
cd randEffect
./randEffect

Option 1: Precompiled Binary
Download the latest Crypto++ release from the official GitHub repository:
Crypto++ Releases [!github][https://github.com/weidai11/cryptopp/releases]
Extract the files to a directory, e.g., C:\cryptopp.

inside C:\cryptopp 
g++ -std=c++17 -DCRYPTOPP_DISABLE_ASM -O2 -c *.cpp

ar rcs libcryptlib.a *.o


run 
g++ -std=c++11 -I C:\cryptopp -L C:\cryptopp -o main main.cpp -lcryptlib

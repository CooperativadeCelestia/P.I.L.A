//author: Cooperativa de Celestia
#ifndef _HASH_H_
#define _HASH_H_
#include <iostream>
#include <sstream>
#include <string>
#include <iomanip>

#include <openssl/sha.h>
#include <openssl/ripemd.h>
using namespace std;

string chaotic_encryption(const string& data) {
    string chaotic_data = data;
    srand('P'); // Semente aleatória com base no primeiro caractere do nome "PILA"

    for (int i = 0; i < 10; ++i) {
        random_shuffle(chaotic_data.begin(), chaotic_data.end());
    }

    return chaotic_data;
}

string sha256(const string str) {
    string chaotic_data = chaotic_encryption(str);

    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, chaotic_data.c_str(), chaotic_data.size());
    SHA256_Final(hash, &sha256);

    stringstream ss;
    for(int i = 0; i < SHA256_DIGEST_LENGTH; i++)
    {
        ss << hex << setw(2) << setfill('0') << (int)hash[i];
    }

    return ss.str();
}

#endif

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>
#include <openssl/sha.h>
#include <openssl/bio.h>
#include <openssl/rand.h>
#include <qrencode.h>

struct Transaction {
    std::string sender;
    std::string receiver;
    int amount;
};

class PILAWallet {
private:
    RSA* keyPair;

    void generate_key_pair() {
        keyPair = RSA_generate_key(2048, RSA_F4, nullptr, nullptr);
    }

    std::string get_public_key_string() {
        BIO* bio = BIO_new(BIO_s_mem());
        PEM_write_bio_RSA_PUBKEY(bio, keyPair);
        int keyLen = BIO_pending(bio);
        char* publicKeyBuffer = new char[keyLen + 1];
        BIO_read(bio, publicKeyBuffer, keyLen);
        publicKeyBuffer[keyLen] = '\0';
        std::string publicKeyString(publicKeyBuffer);
        delete[] publicKeyBuffer;
        BIO_free_all(bio);
        return publicKeyString;
    }

    std::string get_private_key_string() {
        BIO* bio = BIO_new(BIO_s_mem());
        PEM_write_bio_RSAPrivateKey(bio, keyPair, nullptr, nullptr, 0, nullptr, nullptr);
        int keyLen = BIO_pending(bio);
        char* privateKeyBuffer = new char[keyLen + 1];
        BIO_read(bio, privateKeyBuffer, keyLen);
        privateKeyBuffer[keyLen] = '\0';
        std::string privateKeyString(privateKeyBuffer);
        delete[] privateKeyBuffer;
        BIO_free_all(bio);
        return privateKeyString;
    }

public:
    PILAWallet() {
        keyPair = nullptr;
        load_wallet();

        if (!keyPair) {
            generate_key_pair();
            save_wallet();
        }
    }

    ~PILAWallet() {
        if (keyPair) {
            RSA_free(keyPair);
        }
    }

    void save_wallet() {
        std::ofstream file("wallet.txt");
        if (file.is_open()) {
            std::string publicKeyString = get_public_key_string();
            std::string privateKeyString = get_private_key_string();
            file << "Public Key: " << publicKeyString << std::endl;
            file << "Private Key: " << privateKeyString << std::endl;
            file.close();
        }
    }

    void load_wallet() {
        std::ifstream file("wallet.txt");
        if (file.is_open()) {
            std::string line;
            std::getline(file, line);
            if (line.find("Public Key: ") != std::string::npos) {
                std::string publicKeyString = line.substr(line.find("Public Key: ") + 12);
                std::getline(file, line);
                if (line.find("Private Key: ") != std::string::npos) {
                    std::string privateKeyString = line.substr(line.find("Private Key: ") + 13);
                    keyPair = RSA_new();
                    BIO* bio = BIO_new_mem_buf(publicKeyString.c_str(), -1);
                    PEM_read_bio_RSA_PUBKEY(bio, &keyPair, nullptr, nullptr);
                    BIO_free_all(bio);
                    bio = BIO_new_mem_buf(privateKeyString.c_str(), -1);
                    PEM_read_bio_RSAPrivateKey(bio, &keyPair, nullptr, nullptr);
                    BIO_free_all(bio);
                }
            }
            file.close();
        }
    }

    std::string sign_transaction(const std::string& transactionData) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, transactionData.c_str(), transactionData.size());
        SHA256_Final(hash, &sha256);

        unsigned char signature[RSA_size(keyPair)];
        unsigned int sigLen;
        RSA_sign(NID_sha256, hash, SHA256_DIGEST_LENGTH, signature, &sigLen, keyPair);

        std::string signatureString(reinterpret_cast<char*>(signature), sigLen);
        return signatureString;
    }

    bool verify_transaction(const std::string& transactionData, const std::string& signature) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, transactionData.c_str(), transactionData.size());
        SHA256_Final(hash, &sha256);

        int result = RSA_verify(NID_sha256, hash, SHA256_DIGEST_LENGTH, reinterpret_cast<const unsigned char*>(signature.c_str()), signature.size(), keyPair);
        return result == 1;
    }

    std::string generate_transaction_qrcode(const std::string& sender, const std::string& receiver, int amount) {
        std::stringstream transactionData;
        transactionData << "Sender: " << sender << ", Receiver: " << receiver << ", Amount: " << amount;

        std::string signature = sign_transaction(transactionData.str());
        std::string qrCodeData = transactionData.str() + ", Signature: " + signature;

        QRcode* qrCode = QRcode_encodeString(qrCodeData.c_str(), 1, QR_ECLEVEL_L, QR_MODE_8, 1);
        std::stringstream qrCodePngName;
        qrCodePngName << "transaction_" << sender << "_" << receiver << "_" << amount << ".png";
        QRcode_writePNG(qrCode, qrCodePngName.str().c_str());

        std::string qrCodePngPath = qrCodePngName.str();
        QRcode_free(qrCode);

        return qrCodePngPath;
    }
};

int main() {
    PILAWallet wallet;

    // Test
    std::string sender = "SenderAddress";
    std::string receiver = "ReceiverAddress";
    int amount = 50;

    std::string qrCodePath = wallet.generate_transaction_qrcode(sender, receiver, amount);
    std::cout << "QR Code generated: " << qrCodePath << std::endl;

    return 0;
}

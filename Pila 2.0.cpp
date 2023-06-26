#include <iostream>
#include <vector>
#include <ctime>
#include <random>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

struct Transaction {
    std::string sender;
    std::string receiver;
    int amount;
};

struct Block {
    int index;
    std::time_t timestamp;
    std::vector<Transaction> transactions;
    std::string previous_hash;
    std::string hash;
    std::string miner;
    int nonce;
};

class PILA {
private:
    std::vector<Block> chain;
    std::vector<Transaction> pending_transactions;
    int difficulty;
    double reward;

    void create_genesis_block() {
        std::time_t timestamp = std::time(nullptr);
        Block block = {
            0,
            timestamp,
            std::vector<Transaction>(),
            "",
            "",
            "",
            0
        };
        chain.push_back(block);
    }

    Block get_last_block() {
        return chain.back();
    }

    std::string sha256(const std::string& str) {
        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, str.c_str(), str.size());
        SHA256_Final(hash, &sha256);

        std::stringstream ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
        }

        return ss.str();
    }

    std::string chaotic_encryption(const std::string& data) {
        std::string chaotic_data = data;
        std::srand('P'); // Semente aleatória com base no primeiro caractere do nome "PILA"

        for (int i = 0; i < 10; ++i) {
            std::random_shuffle(chaotic_data.begin(), chaotic_data.end());
        }

        return chaotic_data;
    }

public:
    PILA(int difficulty = 4, double reward = 2.5) : difficulty(difficulty), reward(reward) {
        create_genesis_block();
    }

    void add_transaction(const std::string& sender, const std::string& receiver, int amount) {
        Transaction transaction = {
            sender,
            receiver,
            amount
        };
        pending_transactions.push_back(transaction);
    }

    void mine_pending_transactions(const std::string& miner_address) {
        std::time_t timestamp = std::time(nullptr);
        std::string previous_hash = get_last_block().hash;
        int nonce = 0;

        while (true) {
            Block block = {
                static_cast<int>(chain.size()),
                timestamp,
                pending_transactions,
                previous_hash,
                "",
                miner_address,
                nonce
            };

            std::stringstream block_data;
            block_data << "timestamp: " << block.timestamp << ", transactions: [";
            for (const auto& transaction : block.transactions) {
                block_data << "{sender: " << transaction.sender << ", receiver: " << transaction.receiver << ", amount: " << transaction.amount << "}, ";
            }
            block_data << "], previous_hash: " << block.previous_hash << ", nonce: " << block.nonce;

            std::string block_data_encoded = block_data.str();
            std::string encrypted_data = chaotic_encryption(block_data_encoded);

            std::string block_hash = sha256(encrypted_data);
            if (block_hash.substr(0, difficulty) == std::string(difficulty, '0')) {
                block.hash = block_hash;
                chain.push_back(block);
                pending_transactions.clear();

                add_transaction("Sistema", miner_address, reward);

                break;
            }

            ++nonce;
        }
    }

    bool is_chain_valid() {
        for (int i = 1; i < chain.size(); ++i) {
            const Block& previous_block = chain[i - 1];
            const Block& current_block = chain[i];

            std::stringstream block_data;
            block_data << "timestamp: " << current_block.timestamp << ", transactions: [";
            for (const auto& transaction : current_block.transactions) {
                block_data << "{sender: " << transaction.sender << ", receiver: " << transaction.receiver << ", amount: " << transaction.amount << "}, ";
            }
            block_data << "], previous_hash: " << previous_block.hash << ", nonce: " << current_block.nonce;

            std::string block_data_encoded = block_data.str();
            std::string encrypted_data = chaotic_encryption(block_data_encoded);

            std::string block_hash = sha256(encrypted_data);
            if (block_hash != current_block.hash || block_hash.substr(0, difficulty) != std::string(difficulty, '0')) {
                return false;
            }
        }

        return true;
    }
};

int main() {
    PILA pila(16, 2.5);

    pila.add_transaction("Endereco1", "Endereco2", 5);
    pila.add_transaction("Endereco2", "Endereco3", 2);

    std::string miner_address = "EnderecoMinerador";
    pila.mine_pending_transactions(miner_address);

    std::cout << "Blockchain valida? " << (pila.is_chain_valid() ? "Sim" : "Nao") << std::endl;
    for (const auto& block : pila.chain) {
        std::cout << "Bloco " << block.index << ": " << block.hash << std::endl;
    }

    return 0;
}

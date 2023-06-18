#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <cstdlib>
#include <sstream>
#include <iomanip>
#include <openssl/sha.h>

class Block {
private:
    int index;
    std::time_t timestamp;
    std::vector<std::string> transactions;
    std::string previous_hash;
    int nonce;
    std::string hash;

public:
    Block(int index, std::time_t timestamp, const std::vector<std::string>& transactions, const std::string& previous_hash)
        : index(index), timestamp(timestamp), transactions(transactions), previous_hash(previous_hash), nonce(0) {
        hash = calculate_hash();
    }

    std::string calculate_hash() {
        std::stringstream ss;
        ss << index << timestamp;
        for (const std::string& transaction : transactions) {
            ss << transaction;
        }
        ss << previous_hash << nonce;

        unsigned char digest[SHA256_DIGEST_LENGTH];
        SHA256_CTX sha256;
        SHA256_Init(&sha256);
        SHA256_Update(&sha256, ss.str().c_str(), ss.str().length());
        SHA256_Final(digest, &sha256);

        std::stringstream hash_ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
            hash_ss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
        }

        return hash_ss.str();
    }

    void mine_block(int difficulty) {
        std::string target(difficulty, '0');
        while (hash.substr(0, difficulty) != target) {
            ++nonce;
            hash = calculate_hash();
        }
    }

    friend std::ostream& operator<<(std::ostream& os, const Block& block) {
        os << "Block #" << block.index << " [Timestamp: " << block.timestamp
           << ", Nonce: " << block.nonce << ", Hash: " << block.hash << "]";
        return os;
    }
};

class Blockchain {
private:
    int difficulty;
    int reward;
    std::vector<Block> chain;
    std::vector<std::string> pending_transactions;

public:
    Blockchain(int difficulty = 2, int reward = 10) : difficulty(difficulty), reward(reward) {
        chain.push_back(create_genesis_block());
    }

    Block create_genesis_block() {
        std::time_t current_time = std::time(nullptr);
        return Block(0, current_time, {"Genesis Block"}, "0");
    }

    Block get_last_block() {
        return chain.back();
    }

    void add_transaction(const std::string& sender, const std::string& recipient, int amount) {
        std::stringstream ss;
        ss << sender << recipient << amount;
        pending_transactions.push_back(ss.str());
    }

    void mine_pending_transactions(const std::string& miner_address) {
        std::time_t current_time = std::time(nullptr);
        Block block(chain.size(), current_time, pending_transactions, get_last_block().hash);
        block.mine_block(difficulty);
        chain.push_back(block);

        std::stringstream ss;
        ss << "None" << miner_address << reward;
        pending_transactions.clear();
        pending_transactions.push_back(ss.str());
    }

    bool is_chain_valid() {
        for (int i = 1; i < chain.size(); ++i) {
            const Block& current_block = chain[i];
            const Block& previous_block = chain[i - 1];

            if (current_block.hash != current_block.calculate_hash()) {
                return false;
            }

            if (current_block.previous_hash != previous_block.hash) {
                return false;
            }
        }

        return true;
    }

    void print_chain() {
        for (const Block& block : chain) {
            std::cout << block << std::endl;
        }
    }
};

int main() {
    Blockchain pila(3, 10);

    pila.add_transaction("Endereco1", "Endereco2", 5);
    pila.add_transaction("Endereco2", "Endereco3", 2);

    std::string miner_address = "EnderecoMinerador";
    pila.mine_pending_transactions(miner_address);

    std::cout << "Blockchain válida? " << (pila.is_chain_valid() ? "Sim" : "Não") << std::endl;
    std::cout << "Blockchain:" << std::endl;
    pila.print_chain();

    return 0;
}

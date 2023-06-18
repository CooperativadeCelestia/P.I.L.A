#include <iostream>
#include <string>
#include <sstream>
#include <iomanip>
#include <vector>
#include <openssl/sha.h>

struct Transaction {
    std::string sender;
    std::string recipient;
    double amount;
};

class Block {
private:
    int index;
    std::time_t timestamp;
    std::vector<Transaction> transactions;
    std::string previous_hash;
    std::string hash;
    int nonce;

public:
    Block(int idx, std::time_t time, const std::vector<Transaction>& trans, const std::string& prevHash)
        : index(idx), timestamp(time), transactions(trans), previous_hash(prevHash), nonce(0) {
        hash = calculate_hash();
    }

    std::string calculate_hash() {
        std::stringstream ss;
        ss << index << timestamp << previous_hash << nonce;

        for (const auto& transaction : transactions) {
            ss << transaction.sender << transaction.recipient << transaction.amount;
        }

        std::string data = ss.str();

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256((const unsigned char*)data.c_str(), data.size(), hash);

        std::stringstream hashed_ss;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            hashed_ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
        }

        return hashed_ss.str();
    }

    void mine_block(int difficulty) {
        std::string target(difficulty, '0');

        while (hash.substr(0, difficulty) != target) {
            nonce++;
            hash = calculate_hash();
        }

        std::cout << "Block mined: #" << index << " [Hash: " << hash << "]" << std::endl;
    }

    std::string get_hash() const {
        return hash;
    }
};

class Blockchain {
private:
    int difficulty;
    double reward;
    std::vector<Block> chain;
    std::vector<Transaction> pending_transactions;

public:
    Blockchain(int diff, double rew)
        : difficulty(diff), reward(rew) {
        chain.push_back(create_genesis_block());
    }

    Block create_genesis_block() {
        std::vector<Transaction> transactions;
        return Block(0, std::time(nullptr), transactions, "0");
    }

    Block get_last_block() const {
        return chain.back();
    }

    void add_transaction(const std::string& sender, const std::string& recipient, double amount) {
        Transaction transaction{sender, recipient, amount};
        pending_transactions.push_back(transaction);
    }

    void mine_pending_transactions(const std::string& miner_address) {
        std::vector<Transaction> transactions = pending_transactions;

        Transaction reward_transaction{"None", miner_address, reward};
        transactions.push_back(reward_transaction);

        Block block(chain.size(), std::time(nullptr), transactions, get_last_block().get_hash());
        block.mine_block(difficulty);

        chain.push_back(block);

        pending_transactions.clear();
    }

    bool is_chain_valid() const {
        for (int i = 1; i < chain.size(); i++) {
            const Block& current_block = chain[i];
            const Block& previous_block = chain[i - 1];

            if (current_block.get_hash() != current_block.calculate_hash()) {
                return false;
            }

            if (current_block.get_previous_hash() != previous_block.get_hash()) {
                return false;
            }
        }

        return true;
    }
};

int main() {
    int difficulty = 3;
    double reward = 10.0;
    std::string miner_address = "MinerAddress";

    Blockchain pilaBlockchain(difficulty, reward);

    // Adicionar transações pendentes
    pilaBlockchain.add_transaction("Address1", "Address2", 5.0);
    pilaBlockchain.add_transaction("Address2", "Address3", 2.0);

    pilaBlockchain.mine_pending_transactions(miner_address);

    std::cout << "Blockchain is valid? " << (pilaBlockchain.is_chain_valid() ? "Yes" : "No") << std::endl;
    std::cout << "Blockchain:" << std::endl;

    for (const auto& block : pilaBlockchain.get_chain()) {
        std::cout << "Block #" << block.get_index() << " [Hash: " << block.get_hash() << "]" << std::endl;
    }

    return 0;
}

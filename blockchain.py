#include <iostream>
#include <vector>
#include <string>
#include <ctime>
#include <jsoncpp/json/json.h>
#include <openssl/sha.h>

class Blockchain {
public:
    struct Transaction {
        std::string sender;
        std::string recipient;
        double amount;
    };

    struct Block {
        int index;
        time_t timestamp;
        std::vector<Transaction> transactions;
        int nonce;
        std::string previous_hash;
    };

private:
    std::vector<Block> chain;
    std::vector<Transaction> pending_transactions;

public:
    Blockchain() {
        chain = std::vector<Block>();
        pending_transactions = std::vector<Transaction>();
    }

    void create_block(int nonce, const std::string& previous_hash) {
        Block block;
        block.index = chain.size() + 1;
        block.timestamp = time(nullptr);
        block.transactions = pending_transactions;
        block.nonce = nonce;
        block.previous_hash = previous_hash;

        pending_transactions.clear();

        chain.push_back(block);
    }

    void add_transaction(const Transaction& transaction) {
        pending_transactions.push_back(transaction);
    }

    Block get_last_block() {
        return chain.back();
    }

    Block mine_block(const std::string& miner_address) {
        Block last_block = get_last_block();
        std::string previous_hash = hash_block(last_block);

        int nonce = proof_of_work(previous_hash);

        create_block(nonce, previous_hash);

        Transaction reward_transaction;
        reward_transaction.sender = "Sistema";
        reward_transaction.recipient = miner_address;
        reward_transaction.amount = 10;  // Ajuste a recompensa conforme necessário

        add_transaction(reward_transaction);

        return get_last_block();
    }

    std::string hash_block(const Block& block) {
        Json::Value json_block;
        json_block["index"] = block.index;
        json_block["timestamp"] = static_cast<Json::UInt64>(block.timestamp);
        json_block["nonce"] = block.nonce;
        json_block["previous_hash"] = block.previous_hash;

        Json::Value json_transactions(Json::arrayValue);
        for (const auto& transaction : block.transactions) {
            Json::Value json_transaction;
            json_transaction["sender"] = transaction.sender;
            json_transaction["recipient"] = transaction.recipient;
            json_transaction["amount"] = transaction.amount;

            json_transactions.append(json_transaction);
        }
        json_block["transactions"] = json_transactions;

        Json::StreamWriterBuilder builder;
        builder["indentation"] = "";
        std::string block_string = Json::writeString(builder, json_block);

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(block_string.c_str()), block_string.size(), hash);

        std::stringstream ss;
        for (unsigned char i : hash) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);
        }

        return ss.str();
    }

    int proof_of_work(const std::string& previous_hash) {
        int nonce = 0;
        while (!valid_proof(previous_hash, nonce)) {
            nonce++;
        }
        return nonce;
    }

    bool valid_proof(const std::string& previous_hash, int nonce) {
        std::string guess = previous_hash + std::to_string(nonce);

        unsigned char hash[SHA256_DIGEST_LENGTH];
        SHA256(reinterpret_cast<const unsigned char*>(guess.c_str()), guess.size(), hash);

        std::stringstream ss;
        for (unsigned char i : hash) {
            ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(i);
        }

        std::string guess_hash = ss.str();

        // Ajuste o critério de dificuldade conforme necessário
        // Por exemplo, aqui verificamos se os primeiros 4 caracteres do hash são zeros
        return guess_hash.substr(0, 4) == "0000";
    }
};

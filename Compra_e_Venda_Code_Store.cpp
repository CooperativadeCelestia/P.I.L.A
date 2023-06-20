#include <iostream>
#include <vector>
#include <string>

// Definição da estrutura de um código de computação complexo
struct Code {
    std::string name;
    std::string description;
    double price;
    bool sold;

    Code(std::string _name, std::string _description, double _price)
        : name(_name), description(_description), price(_price), sold(false) {}
};

// Definição da estrutura de uma transação
struct Transaction {
    std::string seller;
    std::string buyer;
    Code code;

    Transaction(std::string _seller, std::string _buyer, Code _code)
        : seller(_seller), buyer(_buyer), code(_code) {}
};

// Classe para gerenciar a compra e venda de códigos
class CodeMarketplace {
private:
    std::vector<Code> codes;
    std::vector<Transaction> transactions;

public:
    // Função para adicionar um novo código ao mercado
    void addCode(Code code) {
        codes.push_back(code);
    }

    // Função para listar os códigos disponíveis para venda
    void listAvailableCodes() {
        for (const auto& code : codes) {
            if (!code.sold) {
                std::cout << "Name: " << code.name << std::endl;
                std::cout << "Description: " << code.description << std::endl;
                std::cout << "Price: " << code.price << " P.I.L.A" << std::endl;
                std::cout << "---------------------" << std::endl;
            }
        }
    }

    // Função para realizar uma compra de código
    void purchaseCode(std::string buyer, std::string codeName) {
        for (auto& code : codes) {
            if (code.name == codeName && !code.sold) {
                code.sold = true;
                Transaction transaction("", buyer, code);
                transactions.push_back(transaction);
                std::cout << "Purchase successful!" << std::endl;
                return;
            }
        }

        std::cout << "Code not found or already sold." << std::endl;
    }

    // Função para listar as transações realizadas
    void listTransactions() {
        for (const auto& transaction : transactions) {
            std::cout << "Seller: " << transaction.seller << std::endl;
            std::cout << "Buyer: " << transaction.buyer << std::endl;
            std::cout << "Code Name: " << transaction.code.name << std::endl;
            std::cout << "Price: " << transaction.code.price << " P.I.L.A" << std::endl;
            std::cout << "---------------------" << std::endl;
        }
    }
};

int main() {
    CodeMarketplace marketplace;

    // Adicionando alguns códigos ao mercado
    marketplace.addCode(Code("Code1", "Description for Code1", 10.0));
    marketplace.addCode(Code("Code2", "Description for Code2", 15.0));
    marketplace.addCode(Code("Code3", "Description for Code3", 20.0));

    // Listando os códigos disponíveis
    std::cout << "Available Codes:" << std::endl;
    marketplace.listAvailableCodes();

    // Realizando uma compra de código
    marketplace.purchaseCode("Buyer1", "Code2");

    // Listando as transações realizadas
    std::cout << "Transactions:" << std::endl;
    marketplace.listTransactions();

    return 0;
}

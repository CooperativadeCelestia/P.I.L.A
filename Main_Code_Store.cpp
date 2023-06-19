#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

// Definição da estrutura de um código de programação complexo
struct Code {
    std::string name;
    std::string description;
    double price;
};

// Definição da estrutura de uma transação na blockchain
struct Transaction {
    std::string sender;
    std::string receiver;
    double amount;
};

// Definição da classe da loja de códigos
class CodeStore {
private:
    std::vector<Code> codes; // Lista de códigos disponíveis na loja
    std::unordered_map<std::string, double> balances; // Saldo de cada cliente
    std::vector<Transaction> blockchain; // Blockchain para registrar as transações

public:
    // Adicionar um novo código à loja
    void addCode(const std::string& name, const std::string& description, double price) {
        Code newCode;
        newCode.name = name;
        newCode.description = description;
        newCode.price = price;
        codes.push_back(newCode);
    }

    // Criar uma nova conta para um cliente
    void createAccount(const std::string& client) {
        balances[client] = 0.0;
    }

    // Realizar uma transação de compra de código
    bool purchaseCode(const std::string& buyer, const std::string& codeName) {
        // Verificar se o comprador e o código estão cadastrados
        if (balances.find(buyer) == balances.end()) {
            std::cout << "Cliente não encontrado. Crie uma conta antes de fazer uma compra.\n";
            return false;
        }

        auto it = std::find_if(codes.begin(), codes.end(), [&](const Code& code) {
            return code.name == codeName;
        });

        if (it != codes.end()) {
            Code code = *it;

            // Verificar se o comprador tem saldo suficiente
            if (balances[buyer] >= code.price) {
                // Atualizar os saldos do comprador e da loja
                balances[buyer] -= code.price;
                balances["store"] += code.price;

                // Registrar a transação na blockchain
                Transaction transaction;
                transaction.sender = buyer;
                transaction.receiver = "store";
                transaction.amount = code.price;
                blockchain.push_back(transaction);

                std::cout << "Compra realizada com sucesso! O código " << codeName << " foi adicionado à sua biblioteca.\n";
                return true;
            }
        }

        std::cout << "Erro na compra. Verifique se o código está disponível ou se você tem saldo suficiente.\n";
        return false;
    }

    // Obter o saldo de um cliente
    double getBalance(const std::string& client) {
        return balances[client];
    }

    // Exibir o histórico de transações na blockchain
    void showTransactionHistory() {
        std::cout << "Histórico de transações:\n";
        for (const auto& transaction : blockchain) {
            std::cout << "Remetente: " << transaction.sender << ", Destinatário: " << transaction.receiver
                      << ", Valor: " << transaction.amount << " P.I.L.A\n";
        }
    }
};

int main() {
    // Criar a loja de códigos
    CodeStore store;

    // Adicionar códigos à loja
    store.addCode("code1", "Código de exemplo 1", 10.0);
    store.addCode("code2", "Código de exemplo 2", 15.0);
    store.addCode("code3", "Código de exemplo 3", 20.0);

    // Criar contas para os clientes
    store.createAccount("Cliente1");
    store.createAccount("Cliente2");

    // Realizar transações de compra de códigos
    store.purchaseCode("Cliente1", "code2");
    store.purchaseCode("Cliente2", "code1");
    store.purchaseCode("Cliente1", "code3");

    // Exibir saldos dos clientes
    std::cout << "Saldo do Cliente1: " << store.getBalance("Cliente1") << " P.I.L.A\n";
    std::cout << "Saldo do Cliente2: " << store.getBalance("Cliente2") << " P.I.L.A\n";

    // Exibir histórico de transações
    store.showTransactionHistory();

    return 0;
}

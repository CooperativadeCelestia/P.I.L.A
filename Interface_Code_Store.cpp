#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>

struct Code {
    std::string name;
    std::string description;
    double price;
};

struct Transaction {
    std::string sender;
    std::string receiver;
    double amount;
};

class CodeStore {
private:
    std::vector<Code> codes;
    std::unordered_map<std::string, double> balances;
    std::vector<Transaction> blockchain;

public:
    void addCode(const std::string& name, const std::string& description, double price) {
        Code newCode;
        newCode.name = name;
        newCode.description = description;
        newCode.price = price;
        codes.push_back(newCode);
    }

    void createAccount(const std::string& client) {
        balances[client] = 0.0;
    }

    bool purchaseCode(const std::string& buyer, const std::string& codeName) {
        if (balances.find(buyer) == balances.end()) {
            std::cout << "Cliente não encontrado. Crie uma conta antes de fazer uma compra.\n";
            return false;
        }

        auto it = std::find_if(codes.begin(), codes.end(), [&](const Code& code) {
            return code.name == codeName;
        });

        if (it != codes.end()) {
            Code code = *it;

            if (balances[buyer] >= code.price) {
                balances[buyer] -= code.price;
                balances["store"] += code.price;

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

    double getBalance(const std::string& client) {
        return balances[client];
    }

    void showTransactionHistory() {
        std::cout << "Histórico de transações:\n";
        for (const auto& transaction : blockchain) {
            std::cout << "Remetente: " << transaction.sender << ", Destinatário: " << transaction.receiver
                      << ", Valor: " << transaction.amount << " P.I.L.A\n";
        }
    }
};

int main() {
    CodeStore store;

    // Adicionar códigos à loja
    store.addCode("code1", "Código de exemplo 1", 10.0);
    store.addCode("code2", "Código de exemplo 2", 15.0);
    store.addCode("code3", "Código de exemplo 3", 20.0);

    std::string clientName;
    std::string codeName;

    while (true) {
        std::cout << "\n----- Loja de Códigos -----\n";
        std::cout << "1. Criar uma nova conta\n";
        std::cout << "2. Verificar saldo\n";
        std::cout << "3. Comprar um código\n";
        std::cout << "4. Ver histórico de transações\n";
        std::cout << "5. Sair\n";
        std::cout << "Digite o número da opção desejada: ";

        int option;
        std::cin >> option;

        switch (option) {
            case 1:
                std::cout << "Digite o nome do cliente: ";
                std::cin >> clientName;
                store.createAccount(clientName);
                std::cout << "Conta criada com sucesso para " << clientName << ".\n";
                break;
            case 2:
                std::cout << "Digite o nome do cliente: ";
                std::cin >> clientName;
                std::cout << "Saldo do cliente " << clientName << ": " << store.getBalance(clientName) << " P.I.L.A\n";
                break;
            case 3:
                std::cout << "Digite o nome do cliente: ";
                std::cin >> clientName;
                std::cout << "Digite o nome do código que deseja comprar: ";
                std::cin >> codeName;
                store.purchaseCode(clientName, codeName);
                break;
            case 4:
                store.showTransactionHistory();
                break;
            case 5:
                std::cout << "Encerrando o programa...\n";
                return 0;
            default:
                std::cout << "Opção inválida. Digite um número válido.\n";
                break;
        }
    }

    return 0;
}

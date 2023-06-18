#include <iostream>
#include <vector>
#include <string>

class Transacoes {
private:
    std::vector<std::map<std::string, std::string>> transacoes;

public:
    Transacoes() {
        // Construtor vazio
    }

    void adicionar_transacao(const std::string& remetente, const std::string& destinatario, const std::string& valor) {
        std::map<std::string, std::string> transacao = {
            {"remetente", remetente},
            {"destinatario", destinatario},
            {"valor", valor}
        };
        transacoes.push_back(transacao);
    }

    std::vector<std::map<std::string, std::string>> obter_transacoes_pendentes() {
        return transacoes;
    }

    void limpar_transacoes_pendentes() {
        transacoes.clear();
    }
};

int main() {
    Transacoes transacoes;

    transacoes.adicionar_transacao("Remetente1", "Destinatario1", "10");
    transacoes.adicionar_transacao("Remetente2", "Destinatario2", "5");

    std::vector<std::map<std::string, std::string>> transacoesPendentes = transacoes.obter_transacoes_pendentes();
    for (const auto& transacao : transacoesPendentes) {
        std::cout << "Remetente: " << transacao.at("remetente") << std::endl;
        std::cout << "Destinatario: " << transacao.at("destinatario") << std::endl;
        std::cout << "Valor: " << transacao.at("valor") << std::endl;
        std::cout << std::endl;
    }

    transacoes.limpar_transacoes_pendentes();

    return 0;
}

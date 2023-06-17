#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Network.hpp>

#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <ctime>

#include "Blockchain.h"
#include "Transacoes.h"
#include "Minerador.h"

int main()
{
    sf::RenderWindow window(sf::VideoMode(600, 400), "Criptomoeda PILA");

    // Criação da instância da Blockchain e das transações
    Blockchain blockchain;
    Transacoes transacoes;

    // Criação da instância do minerador
    Minerador minerador(blockchain, transacoes, 10, "EndereçoMinerador");

    sf::Font font;
    if (!font.loadFromFile("arial.ttf"))
    {
        // error handling
    }

    // Criação dos elementos de texto
    sf::Text remetenteText("Remetente:", font, 20);
    remetenteText.setPosition(10, 10);

    sf::Text destinatarioText("Destinatário:", font, 20);
    destinatarioText.setPosition(10, 100);

    sf::Text valorText("Valor:", font, 20);
    valorText.setPosition(10, 190);

    sf::Text blocoAtualText("Bloco Atual: -", font, 20);
    blocoAtualText.setPosition(10, 300);

    sf::Text hashBlocoAtualText("Hash: -", font, 20);
    hashBlocoAtualText.setPosition(10, 340);

    sf::Text recompensaMineradorText("Recompensa: -", font, 20);
    recompensaMineradorText.setPosition(10, 380);

    // Criação dos campos de entrada
    sf::RectangleShape remetenteInput(sf::Vector2f(200, 40));
    remetenteInput.setPosition(150, 10);
    remetenteInput.setOutlineThickness(2);
    remetenteInput.setOutlineColor(sf::Color::Black);

    sf::RectangleShape destinatarioInput(sf::Vector2f(200, 40));
    destinatarioInput.setPosition(150, 100);
    destinatarioInput.setOutlineThickness(2);
    destinatarioInput.setOutlineColor(sf::Color::Black);

    sf::RectangleShape valorInput(sf::Vector2f(200, 40));
    valorInput.setPosition(150, 190);
    valorInput.setOutlineThickness(2);
    valorInput.setOutlineColor(sf::Color::Black);

    // Criação dos botões
    sf::RectangleShape adicionarTransacaoButton(sf::Vector2f(200, 40));
    adicionarTransacaoButton.setPosition(10, 250);
    adicionarTransacaoButton.setOutlineThickness(2);
    adicionarTransacaoButton.setOutlineColor(sf::Color::Black);

    sf::RectangleShape minerarBlocoButton(sf::Vector2f(200, 40));
    minerarBlocoButton.setPosition(220, 250);
    minerarBlocoButton.setOutlineThickness(2);
    minerarBlocoButton.setOutlineColor(sf::Color::Black);

    // Loop principal da janela
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            // Adicionar transação ao clicar no botão "Adicionar Transação"
            if (event.type == sf::Event::MouseButtonReleased)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    sf::Vector2i mousePosition = sf::Mouse::getPosition(window);

                    if (adicionarTransacaoButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        std::string remetente = ""; // Preencha com o valor do campo de entrada remetente
                        std::string destinatario = ""; // Preencha com o valor do campo de entrada destinatario
                        std::string valor = ""; // Preencha com o valor do campo de entrada valor

                        transacoes.adicionarTransacao(remetente, destinatario, valor);

                        // Limpar campos de entrada
                        // Remetente
                        // Destinatario
                        // Valor
                    }

                    if (minerarBlocoButton.getGlobalBounds().contains(mousePosition.x, mousePosition.y))
                    {
                        // Minerar um novo bloco

                        // Atualizar a interface com o novo bloco minerado
                        std::string blocoAtual = "Bloco Atual: "; // Preencha com o índice do novo bloco minerado
                        std::string hashBlocoAtual = "Hash: "; // Preencha com o hash do novo bloco minerado
                        std::string recompensaMinerador = "Recompensa: "; // Preencha com a recompensa do minerador

                        blocoAtualText.setString(blocoAtual);
                        hashBlocoAtualText.setString(hashBlocoAtual);
                        recompensaMineradorText.setString(recompensaMinerador);
                    }
                }
            }
        }

        window.clear(sf::Color::White);

        window.draw(remetenteText);
        window.draw(destinatarioText);
        window.draw(valorText);
        window.draw(blocoAtualText);
        window.draw(hashBlocoAtualText);
        window.draw(recompensaMineradorText);

        window.draw(remetenteInput);
        window.draw(destinatarioInput);
        window.draw(valorInput);

        window.draw(adicionarTransacaoButton);
        window.draw(minerarBlocoButton);

        window.display();
    }

    return 0;
}

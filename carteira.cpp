#include <iostream>
#include <string>
#include <vector>
#include <gtkmm.h>
#include <gdkmm/pixbuf.h>

#include "blockchain.h"
#include "transacoes.h"
#include "minerador.h"
#include "carteira.h"

int main(int argc, char* argv[]) {
    // Inicialização do GTKmm
    auto app = Gtk::Application::create(argc, argv);

    // Criação da instância da Blockchain e das transações
    Blockchain blockchain;
    Transacoes transacoes;

    // Criação da instância do minerador
    Minerador minerador(blockchain, transacoes, 10, "EndereçoMinerador");

    // Criação da instância da carteira
    Carteira carteira(blockchain, transacoes);

    // Criação da janela principal
    Gtk::Window window;
    window.set_title("Criptomoeda PILA");
    window.set_default_size(400, 400);

    // Carregamento do logo PILA
    Glib::RefPtr<Gdk::Pixbuf> logo_pixbuf = Gdk::Pixbuf::create_from_file("pila_logo.png");
    Gtk::Image logo_image(logo_pixbuf);

    // Criação dos frames da interface
    Gtk::Frame frame_top;
    Gtk::Frame frame_center;
    Gtk::Frame frame_bottom;

    window.add(frame_top);
    window.add(frame_center);
    window.add(frame_bottom);

    // Criação dos widgets da interface
    Gtk::Label lbl_remetente("Remetente:");
    Gtk::Entry entry_remetente;

    frame_top.add(lbl_remetente);
    frame_top.add(entry_remetente);

    Gtk::Label lbl_destinatario("Destinatário:");
    Gtk::Entry entry_destinatario;

    frame_center.add(lbl_destinatario);
    frame_center.add(entry_destinatario);

    Gtk::Label lbl_valor("Valor:");
    Gtk::Entry entry_valor;

    frame_bottom.add(lbl_valor);
    frame_bottom.add(entry_valor);

    Gtk::Button btn_adicionar_transacao("Adicionar Transação");
    btn_adicionar_transacao.signal_clicked().connect([&](){
        std::string remetente = entry_remetente.get_text();
        std::string destinatario = entry_destinatario.get_text();
        double valor = std::stod(entry_valor.get_text());

        carteira.adicionar_transacao(remetente, destinatario, valor);

        entry_remetente.set_text("");
        entry_destinatario.set_text("");
        entry_valor.set_text("");
    });

    Gtk::Button btn_minerar_bloco("Minerar Bloco");
    btn_minerar_bloco.signal_clicked().connect([&](){
        Block novo_bloco = minerador.minerar();

        // Atualizar a interface com o novo bloco minerado
        lbl_bloco_atual.set_text("Bloco Atual: " + std::to_string(novo_bloco.index));
        lbl_hash_bloco_atual.set_text("Hash: " + novo_bloco.hash);
        lbl_recompensa_minerador.set_text("Recompensa: " + std::to_string(novo_bloco.transacoes[0].valor) + " PILA");
    });

    Gtk::Label lbl_bloco_atual("Bloco Atual: -");
    Gtk::Label lbl_hash_bloco_atual("Hash: -");
    Gtk::Label lbl_recompensa_minerador("Recompensa: -");

    Gtk::Box vbox(Gtk::ORIENTATION_VERTICAL);
    vbox.pack_start(logo_image, Gtk::PACK_SHRINK);
    vbox.pack_start(frame_top, Gtk::PACK_SHRINK);
    vbox.pack_start(frame_center, Gtk::PACK_SHRINK);
    vbox.pack_start(frame_bottom, Gtk::PACK_SHRINK);
    vbox.pack_start(btn_adicionar_transacao, Gtk::PACK_SHRINK);
    vbox.pack_start(btn_minerar_bloco, Gtk::PACK_SHRINK);
    vbox.pack_start(lbl_bloco_atual, Gtk::PACK_SHRINK);
    vbox.pack_start(lbl_hash_bloco_atual, Gtk::PACK_SHRINK);
    vbox.pack_start(lbl_recompensa_minerador, Gtk::PACK_SHRINK);

    window.add(vbox);
    window.show_all();

    return app->run(window);
}

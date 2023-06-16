import tkinter as tk
from PIL import ImageTk, Image
from blockchain import Blockchain
from transacoes import Transacoes
from minerador import Minerador

# Criação da instância da Blockchain e das transações
blockchain = Blockchain()
transacoes = Transacoes()

# Criação da instância do minerador
minerador = Minerador(blockchain, transacoes, recompensa=10, nome="EndereçoMinerador")

# Função para adicionar uma transação
def adicionar_transacao():
    remetente = entry_remetente.get()
    destinatario = entry_destinatario.get()
    valor = entry_valor.get()

    transacoes.adicionar_transacao(remetente, destinatario, valor)

    entry_remetente.delete(0, tk.END)
    entry_destinatario.delete(0, tk.END)
    entry_valor.delete(0, tk.END)

# Função para minerar um bloco
def minerar_bloco():
    novo_bloco = minerador.minerar()

    # Atualizar a interface com o novo bloco minerado
    lbl_bloco_atual.config(text=f"Bloco Atual: {novo_bloco['index']}")
    lbl_hash_bloco_atual.config(text=f"Hash: {novo_bloco['hash']}")
    lbl_recompensa_minerador.config(text=f"Recompensa: {novo_bloco['transacoes'][0]['valor']} PILA")

# Configuração da janela principal
window = tk.Tk()
window.title("Criptomoeda PILA")
window.geometry("600x400")

# Criação dos frames da interface
frame_top = tk.Frame(window)
frame_top.pack(pady=10)

frame_center = tk.Frame(window)
frame_center.pack(pady=10)

frame_bottom = tk.Frame(window)
frame_bottom.pack(pady=10)

frame_info = tk.Frame(window)
frame_info.pack(pady=10)

# Criação dos widgets da interface
lbl_remetente = tk.Label(frame_top, text="Remetente:")
lbl_remetente.pack(side=tk.LEFT)

entry_remetente = tk.Entry(frame_top)
entry_remetente.pack(side=tk.LEFT)

lbl_destinatario = tk.Label(frame_center, text="Destinatário:")
lbl_destinatario.pack(side=tk.LEFT)

entry_destinatario = tk.Entry(frame_center)
entry_destinatario.pack(side=tk.LEFT)

lbl_valor = tk.Label(frame_bottom, text="Valor:")
lbl_valor.pack(side=tk.LEFT)

entry_valor = tk.Entry(frame_bottom)
entry_valor.pack(side=tk.LEFT)

btn_adicionar_transacao = tk.Button(window, text="Adicionar Transação", command=adicionar_transacao)
btn_adicionar_transacao.pack()

btn_minerar_bloco = tk.Button(window, text="Minerar Bloco", command=minerar_bloco)
btn_minerar_bloco.pack()

lbl_bloco_atual = tk.Label(frame_info, text="Bloco Atual: -", font=("Arial", 12))
lbl_bloco_atual.pack()

lbl_hash_bloco_atual = tk.Label(frame_info, text="Hash: -", font=("Arial", 12))
lbl_hash_bloco_atual.pack()

lbl_recompensa_minerador = tk.Label(frame_info, text="Recompensa: -", font=("Arial", 12))
lbl_recompensa_minerador.pack()

# Carregar o logo da PILA
logo = Image.open("pila_logo.png")
logo = logo.resize((150, 150), Image.ANTIALIAS)
logo = ImageTk.PhotoImage(logo)

lbl_logo = tk.Label(window, image=logo)
lbl_logo.pack()

window.mainloop()

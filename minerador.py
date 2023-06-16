class Minerador:
    def __init__(self, blockchain, transacoes, recompensa, nome):
        self.blockchain = blockchain
        self.transacoes = transacoes
        self.recompensa = recompensa
        self.nome = nome

    def minerar(self):
        # Obtenha as transações pendentes da lista de transações
        transacoes_pendentes = self.transacoes.obter_transacoes_pendentes()

        # Adicione uma transação de recompensa ao minerador
        self.transacoes.adicionar_transacao("Sistema", self.nome, self.recompensa)

        # Mine um novo bloco
        novo_bloco = self.blockchain.mine_block(self.nome)

        # Retorne o novo bloco minerado
        return novo_bloco

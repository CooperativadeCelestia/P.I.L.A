class Transacoes:
    def __init__(self):
        self.transacoes = []

    def adicionar_transacao(self, remetente, destinatario, valor):
        # Implemente a lógica para adicionar uma transação à lista de transações
        transacao = {
            "remetente": remetente,
            "destinatario": destinatario,
            "valor": valor
        }
        self.transacoes.append(transacao)

    def obter_transacoes_pendentes(self):
        # Implemente a lógica para retornar a lista de transações pendentes
        return self.transacoes

    def limpar_transacoes_pendentes(self):
        # Implemente a lógica para limpar a lista de transações pendentes após a mineração de um bloco
        self.transacoes = []

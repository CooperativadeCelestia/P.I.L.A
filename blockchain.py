class Blockchain:
    def __init__(self):
        self.chain = []
        self.pending_transactions = []

    def create_block(self, nonce, previous_hash):
        # Implemente a lógica para criar um bloco e adicionar à cadeia
        # O bloco pode conter informações como nonce, hash, transações, timestamp, etc.

        block = {
            "index": len(self.chain) + 1,
            "timestamp": time(),
            "transactions": self.pending_transactions,
            "nonce": nonce,
            "previous_hash": previous_hash,
        }

        # Limpe as transações pendentes após adicionar ao bloco
        self.pending_transactions = []

        self.chain.append(block)

    def add_transaction(self, transaction):
        # Implemente a lógica para adicionar uma transação às transações pendentes
        self.pending_transactions.append(transaction)

    def get_last_block(self):
        # Implemente a lógica para retornar o último bloco da cadeia
        return self.chain[-1]

    def mine_block(self, miner_address):
        # Implemente a lógica para minerar um novo bloco
        # Isso pode incluir a validação de transações pendentes, a prova de trabalho, etc.
        last_block = self.get_last_block()
        previous_hash = self.hash_block(last_block)

        # Simule a prova de trabalho encontrando um nonce válido
        nonce = self.proof_of_work(previous_hash)

        # Crie um novo bloco na cadeia
        self.create_block(nonce, previous_hash)

        # Recompense o minerador adicionando uma transação especial
        reward_transaction = {
            "sender": "Sistema",
            "recipient": miner_address,
            "amount": 10,  # Ajuste a recompensa conforme necessário
        }
        self.add_transaction(reward_transaction)

        # Retorne o novo bloco minerado
        return self.get_last_block()

    def hash_block(self, block):
        # Implemente a lógica para calcular o hash de um bloco
        # Você pode usar a função de hash SHA-256 para isso
        block_string = json.dumps(block, sort_keys=True).encode()
        return hashlib.sha256(block_string).hexdigest()

    def proof_of_work(self, previous_hash):
        # Implemente a lógica para encontrar um nonce válido usando prova de trabalho
        # Isso pode envolver o cálculo repetido de hashes até encontrar um nonce que satisfaça certos critérios
        # A dificuldade da prova de trabalho pode ser ajustada para controlar a taxa de mineração

        nonce = 0
        while self.valid_proof(previous_hash, nonce) is False:
            nonce += 1

        return nonce

    def valid_proof(self, previous_hash, nonce):
        # Implemente a lógica para verificar se um nonce é válido
        # Isso pode envolver o cálculo de hashes e a comparação com um critério de dificuldade

        guess = f"{previous_hash}{nonce}".encode()
        guess_hash = hashlib.sha256(guess).hexdigest()

        # Ajuste o critério de dificuldade conforme necessário
        # Por exemplo, aqui verificamos se os primeiros 4 caracteres do hash são zeros
        return guess_hash[:4] == "0000"

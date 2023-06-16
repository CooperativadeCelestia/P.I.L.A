import hashlib
import time

class Block:
    def __init__(self, index, timestamp, transactions, previous_hash):
        self.index = index
        self.timestamp = timestamp
        self.transactions = transactions
        self.previous_hash = previous_hash
        self.nonce = 0
        self.hash = self.calculate_hash()

    def calculate_hash(self):
        data = str(self.index) + str(self.timestamp) + str(self.transactions) + str(self.previous_hash) + str(self.nonce)
        return hashlib.sha256(data.encode()).hexdigest()

    def mine_block(self, difficulty):
        target = "0" * difficulty
        while self.hash[:difficulty] != target:
            self.nonce += 1
            self.hash = self.calculate_hash()

    def __str__(self):
        return f"Block #{self.index} [Timestamp: {self.timestamp}, Nonce: {self.nonce}, Hash: {self.hash}]"


class Blockchain:
    def __init__(self, difficulty=2, reward=10):
        self.difficulty = difficulty
        self.reward = reward
        self.chain = [self.create_genesis_block()]
        self.pending_transactions = []

    def create_genesis_block(self):
        return Block(0, time.time(), "Genesis Block", "0")

    def get_last_block(self):
        return self.chain[-1]

    def add_transaction(self, sender, recipient, amount):
        self.pending_transactions.append({"sender": sender, "recipient": recipient, "amount": amount})

    def mine_pending_transactions(self, miner_address):
        block = Block(len(self.chain), time.time(), self.pending_transactions, self.get_last_block().hash)
        block.mine_block(self.difficulty)
        self.chain.append(block)

        self.pending_transactions = [{"sender": None, "recipient": miner_address, "amount": self.reward}]

    def is_chain_valid(self):
        for i in range(1, len(self.chain)):
            current_block = self.chain[i]
            previous_block = self.chain[i - 1]

            if current_block.hash != current_block.calculate_hash():
                return False

            if current_block.previous_hash != previous_block.hash:
                return False

        return True


# Exemplo de uso
pila = Blockchain(difficulty=3, reward=10)

pila.add_transaction("Endereco1", "Endereco2", 5)
pila.add_transaction("Endereco2", "Endereco3", 2)

miner_address = "EnderecoMinerador"
pila.mine_pending_transactions(miner_address)

print("Blockchain válida?", pila.is_chain_valid())
print("Blockchain:")
for block in pila.chain:
    print(block)

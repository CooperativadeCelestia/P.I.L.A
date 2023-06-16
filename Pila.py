import hashlib
import datetime
import random
import string

class PILA:
    def __init__(self, difficulty=4, reward=10):
        self.chain = []
        self.pending_transactions = []
        self.difficulty = difficulty
        self.reward = reward

        self.create_genesis_block()

    def create_genesis_block(self):
        timestamp = datetime.datetime.now()
        block = {
            "index": 0,
            "timestamp": timestamp,
            "data": "Bloco Genesis",
            "previous_hash": "",
            "hash": "",
            "nonce": 0
        }
        self.chain.append(block)

    def get_last_block(self):
        return self.chain[-1]

    def add_transaction(self, sender, receiver, amount):
        transaction = {
            "sender": sender,
            "receiver": receiver,
            "amount": amount
        }
        self.pending_transactions.append(transaction)

    def mine_pending_transactions(self, miner_address):
        timestamp = datetime.datetime.now()
        previous_hash = self.get_last_block()["hash"]
        nonce = 0

        while True:
            block_data = {
                "timestamp": timestamp,
                "transactions": self.pending_transactions,
                "previous_hash": previous_hash,
                "nonce": nonce
            }
            block_data_encoded = str(block_data).encode()

            # Criptografia caótica dos dados do bloco
            encrypted_data = self.chaotic_encryption(block_data_encoded)

            block_hash = hashlib.sha256(encrypted_data).hexdigest()
            if block_hash[:self.difficulty] == "0" * self.difficulty:
                block = {
                    "index": len(self.chain),
                    "timestamp": timestamp,
                    "data": self.pending_transactions,
                    "previous_hash": previous_hash,
                    "hash": block_hash,
                    "miner": miner_address,
                    "nonce": nonce
                }
                self.chain.append(block)
                self.pending_transactions = []

                # Adicionar recompensa para o minerador
                self.add_transaction("Sistema", miner_address, self.reward)

                break

            nonce += 1

    def is_chain_valid(self):
        for i in range(1, len(self.chain)):
            previous_block = self.chain[i-1]
            current_block = self.chain[i]

            # Verificar o hash do bloco atual
            block_data = {
                "timestamp": current_block["timestamp"],
                "transactions": current_block["data"],
                "previous_hash": previous_block["hash"],
                "nonce": current_block["nonce"]
            }
            block_data_encoded = str(block_data).encode()

            # Criptografia caótica dos dados do bloco
            encrypted_data = self.chaotic_encryption(block_data_encoded)

            block_hash = hashlib.sha256(encrypted_data).hexdigest()
            if block_hash != current_block["hash"]:
                return False

            # Verificar a prova de trabalho
            if block_hash[:self.difficulty] != "0" * self.difficulty:
                return False

        return True

    def chaotic_encryption(self, data):
        random.seed(ord("P"))  # Semente aleatória com base no primeiro caractere do nome "PILA"
        chaotic_data = bytearray(data)

        for _ in range(10):
            random.shuffle(chaotic_data)

        return bytes(chaotic_data)

    def generate_wallet(self):
        alphabet = string.ascii_letters + string.digits
        private_key = ''.join(random.choice(alphabet) for _ in range(64))
        public_key = hashlib.sha256(private_key.encode()).hexdigest()

        return {
            "private_key": private_key,
            "public_key": public_key
        }

# Exemplo de uso
pila = PILA(difficulty=5, reward=10)

pila.add_transaction("Endereço1", "Endereço2", 5)
pila.add_transaction("Endereço2", "Endereço3", 2)

miner_address = "EndereçoMinerador"
pila.mine_pending_transactions(miner_address)

print("Blockchain válida?", pila.is_chain_valid())
print("Blockchain:", pila.chain)

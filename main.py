from nodo.nodo import Node
from cryptocurrency.pila import PILA
from database.db import Database

def main():
    # Create an instance of the PILA object
    pila = PILA()

    # Create an instance of the Database object
    db = Database()

    # Create an instance of the Node object and provide PILA and Database objects
    node = Node(pila, db)

    # Initialize the node
    node.initialize()

    # Run the node
    node.run()

if __name__ == "__main__":
    main()

#include "nodo/nodo.h"
#include "cryptocurrency/pila.h"
#include "database/db.h"

int main() {
    // Create an instance of the PILA object
    PILA pila;

    // Create an instance of the Database object
    Database db;

    // Create an instance of the Node object and provide PILA and Database objects
    Node node(pila, db);

    // Initialize the node
    node.initialize();

    // Run the node
    node.run();

    return 0;
}

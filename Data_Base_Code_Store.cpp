#include <iostream>
#include <sqlite3.h>

int main() {
    sqlite3* db;
    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao abrir o banco de dados: " << sqlite3_errmsg(db) << std::endl;
        return 1;
    }

    // Criar a tabela se ela não existir
    const char* createTableQuery = "CREATE TABLE IF NOT EXISTS codigos (id INTEGER PRIMARY KEY, codigo TEXT);";
    rc = sqlite3_exec(db, createTableQuery, 0, 0, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao criar a tabela: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    // Inserir um código na tabela
    const char* insertQuery = "INSERT INTO codigos (codigo) VALUES ('exemplo_codigo');";
    rc = sqlite3_exec(db, insertQuery, 0, 0, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao inserir o código: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    // Consultar os códigos na tabela
    const char* selectQuery = "SELECT * FROM codigos;";
    rc = sqlite3_exec(db, selectQuery, [](void* data, int argc, char** argv, char** /*azColName*/) -> int {
        for (int i = 0; i < argc; ++i) {
            std::cout << argv[i] << " ";
        }
        std::cout << std::endl;
        return 0;
    }, 0, 0);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao consultar os códigos: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return 1;
    }

    // Fechar o banco de dados
    sqlite3_close(db);

    return 0;
}

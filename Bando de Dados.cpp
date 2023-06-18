#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <sqlite3.h>

#include "flask.h"
#include "sha256.h"

// Função para calcular o hash SHA256 de uma string
std::string calculate_hash(const std::string& string) {
    return sha256(string);
}

// Função para criptografar dados sensíveis
std::string encrypt_data(const std::string& data) {
    // Implemente aqui o algoritmo de criptografia de sua escolha
    // Esta função é apenas um placeholder
    return data;
}

// Função para descriptografar dados sensíveis
std::string decrypt_data(const std::string& data) {
    // Implemente aqui o algoritmo de descriptografia correspondente à criptografia utilizada
    // Esta função é apenas um placeholder
    return data;
}

// Função de callback para executar consultas SQL
int sql_callback(void* data, int argc, char** argv, char** azColName) {
    std::vector<std::map<std::string, std::string>>* records = reinterpret_cast<std::vector<std::map<std::string, std::string>>*>(data);

    std::map<std::string, std::string> record;
    for (int i = 0; i < argc; i++) {
        std::string columnName(azColName[i]);
        std::string columnValue(argv[i] ? argv[i] : "");
        record[columnName] = columnValue;
    }

    records->push_back(record);
    return 0;
}

// Função para criar a tabela de registros no banco de dados
void create_table() {
    sqlite3* db;
    char* errMsg = nullptr;

    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao abrir o banco de dados: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }

    std::string sql = "CREATE TABLE IF NOT EXISTS records ("
                      "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                      "registration_number TEXT,"
                      "full_name TEXT,"
                      "sex TEXT,"
                      "date_of_birth TEXT,"
                      "marital_status TEXT,"
                      "nickname TEXT,"
                      "mother_name TEXT,"
                      "father_name TEXT,"
                      "nationality TEXT,"
                      "state TEXT,"
                      "country TEXT,"
                      "address TEXT,"
                      "house_number TEXT,"
                      "neighborhood TEXT,"
                      "city TEXT,"
                      "postal_code TEXT,"
                      "phone TEXT,"
                      "email TEXT"
                      ")";
    rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao criar a tabela: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);
}

// Rota para exibir a lista de registros
std::string index() {
    std::vector<std::map<std::string, std::string>> records;

    sqlite3* db;
    char* errMsg = nullptr;

    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao abrir o banco de dados: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return "";
    }

    std::string sql = "SELECT * FROM records";
    rc = sqlite3_exec(db, sql.c_str(), sql_callback, &records, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao executar a consulta: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);

    // Renderizar o template com os registros
    // ...

    return "";
}

// Rota para exibir detalhes de um registro específico
std::string details(int record_id) {
    std::vector<std::map<std::string, std::string>> records;

    sqlite3* db;
    char* errMsg = nullptr;

    int rc = sqlite3_open("database.db", &db);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao abrir o banco de dados: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return "";
    }

    std::string sql = "SELECT * FROM records WHERE id = " + std::to_string(record_id);
    rc = sqlite3_exec(db, sql.c_str(), sql_callback, &records, &errMsg);
    if (rc != SQLITE_OK) {
        std::cerr << "Erro ao executar a consulta: " << errMsg << std::endl;
        sqlite3_free(errMsg);
    }

    sqlite3_close(db);

    if (!records.empty()) {
        std::map<std::string, std::string>& record = records.front();

        // Descriptografar dados sensíveis
        // ...

        // Renderizar o template com os detalhes do registro
        // ...
    }

    return "";
}

// Rota para exibir o formulário de registro
std::string show_register_form() {
    // Renderizar o template do formulário de registro
    // ...

    return "";
}

// Rota para processar o registro
std::string register(const std::map<std::string, std::string>& form_data) {
    try {
        // Obter os dados do formulário
        std::string registration_number = form_data.at("registration_number");
        std::string full_name = form_data.at("full_name");
        std::string sex = form_data.at("sex");
        std::string date_of_birth = form_data.at("date_of_birth");
        std::string marital_status = form_data.at("marital_status");
        std::string nickname = form_data.at("nickname");
        std::string mother_name = form_data.at("mother_name");
        std::string father_name = form_data.at("father_name");
        std::string nationality = form_data.at("nationality");
        std::string state = form_data.at("state");
        std::string country = form_data.at("country");
        std::string address = form_data.at("address");
        std::string house_number = form_data.at("house_number");
        std::string neighborhood = form_data.at("neighborhood");
        std::string city = form_data.at("city");
        std::string postal_code = form_data.at("postal_code");
        std::string phone = form_data.at("phone");
        std::string email = form_data.at("email");

        // Validar os dados recebidos
        if (registration_number.empty() || full_name.empty() || email.empty()) {
            throw std::runtime_error("Preencha todos os campos obrigatórios.");
        }

        // Calcular o hash SHA256 do número de matrícula
        std::string registration_hash = calculate_hash(registration_number);

        // Criptografar dados sensíveis
        std::string encrypted_full_name = encrypt_data(full_name);
        std::string encrypted_mother_name = encrypt_data(mother_name);
        std::string encrypted_father_name = encrypt_data(father_name);
        std::string encrypted_phone = encrypt_data(phone);

        // Inserir o registro no banco de dados
        sqlite3* db;
        char* errMsg = nullptr;

        int rc = sqlite3_open("database.db", &db);
        if (rc != SQLITE_OK) {
            std::cerr << "Erro ao abrir o banco de dados: " << sqlite3_errmsg(db) << std::endl;
            sqlite3_close(db);
            return "";
        }

        std::string sql = "INSERT INTO records (registration_number, full_name, sex, date_of_birth, marital_status, nickname, "
                          "mother_name, father_name, nationality, state, country, address, house_number, neighborhood, city, "
                          "postal_code, phone, email) VALUES ('" + registration_hash + "', '" + encrypted_full_name + "', '"
                          + sex + "', '" + date_of_birth + "', '" + marital_status + "', '" + nickname + "', '"
                          + encrypted_mother_name + "', '" + encrypted_father_name + "', '" + nationality + "', '" + state + "', '"
                          + country + "', '" + address + "', '" + house_number + "', '" + neighborhood + "', '" + city + "', '"
                          + postal_code + "', '" + encrypted_phone + "', '" + email + "')";

        rc = sqlite3_exec(db, sql.c_str(), nullptr, nullptr, &errMsg);
        if (rc != SQLITE_OK) {
            std::cerr << "Erro ao inserir o registro: " << errMsg << std::endl;
            sqlite3_free(errMsg);
        }

        sqlite3_close(db);

        // Redirecionar para a página inicial
        // ...
    } catch (const std::exception& e) {
        std::cerr << "Erro durante o registro: " << e.what() << std::endl;

        // Exibir mensagem de erro
        // ...
    }

    return "";
}

int main() {
    create_table();

    // Configurar rotas
    Flask app;

    app.route("/", "GET", index);
    app.route("/details/<int:record_id>", "GET", details);
    app.route("/register", "GET", show_register_form);
    app.route("/register", "POST", register);

    // Executar o servidor
    app.run("localhost", 5000);

    return 0;
}

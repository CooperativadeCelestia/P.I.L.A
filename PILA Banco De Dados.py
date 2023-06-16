from flask import Flask, render_template, request, redirect, flash
from hashlib import sha256
import sqlite3

app = Flask(__name__)
app.secret_key = 'your_secret_key'

# Função para calcular o hash SHA256 de uma string
def calculate_hash(string):
    return sha256(string.encode()).hexdigest()

# Função para criptografar dados sensíveis
def encrypt_data(data):
    # Implemente aqui o algoritmo de criptografia de sua escolha
    # Esta função é apenas um placeholder
    return data

# Função para descriptografar dados sensíveis
def decrypt_data(data):
    # Implemente aqui o algoritmo de descriptografia correspondente à criptografia utilizada
    # Esta função é apenas um placeholder
    return data

# Função para criar a tabela de registros no banco de dados
def create_table():
    connection = sqlite3.connect('database.db')
    cursor = connection.cursor()
    cursor.execute('''CREATE TABLE IF NOT EXISTS records (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        registration_number TEXT,
        full_name TEXT,
        sex TEXT,
        date_of_birth TEXT,
        marital_status TEXT,
        nickname TEXT,
        mother_name TEXT,
        father_name TEXT,
        nationality TEXT,
        state TEXT,
        country TEXT,
        address TEXT,
        house_number TEXT,
        neighborhood TEXT,
        city TEXT,
        postal_code TEXT,
        phone TEXT,
        email TEXT
    )''')
    connection.commit()
    connection.close()

# Rota para exibir a lista de registros
@app.route('/', methods=['GET'])
def index():
    connection = sqlite3.connect('database.db')
    cursor = connection.cursor()
    cursor.execute('''SELECT * FROM records''')
    records = cursor.fetchall()
    connection.close()
    return render_template('index.html', records=records)

# Rota para exibir detalhes de um registro específico
@app.route('/details/<int:record_id>', methods=['GET'])
def details(record_id):
    connection = sqlite3.connect('database.db')
    cursor = connection.cursor()
    cursor.execute('''SELECT * FROM records WHERE id = ?''', (record_id,))
    record = cursor.fetchone()
    connection.close()

    # Descriptografar dados sensíveis
    record = list(record)
    record[2] = decrypt_data(record[2])
    record[7] = decrypt_data(record[7])
    record[8] = decrypt_data(record[8])
    record[16] = decrypt_data(record[16])

    return render_template('details.html', record=record)

# Rota para exibir o formulário de registro
@app.route('/register', methods=['GET'])
def show_register_form():
    return render_template('register.html')

# Rota para processar o registro
@app.route('/register', methods=['POST'])
def register():
    try:
        # Obter os dados do formulário
        registration_number = request.form['registration_number']
        full_name = request.form['full_name']
        sex = request.form['sex']
        date_of_birth = request.form['date_of_birth']
        marital_status = request.form['marital_status']
        nickname = request.form['nickname']
        mother_name = request.form['mother_name']
        father_name = request.form['father_name']
        nationality = request.form['nationality']
        state = request.form['state']
        country = request.form['country']
        address = request.form['address']
        house_number = request.form['house_number']
        neighborhood = request.form['neighborhood']
        city = request.form['city']
        postal_code = request.form['postal_code']
        phone = request.form['phone']
        email = request.form['email']

        # Validar os dados recebidos
        if not registration_number or not full_name or not email:
            raise Exception('Preencha todos os campos obrigatórios.')

        # Calcular o hash SHA256 do número de matrícula
        registration_hash = calculate_hash(registration_number)

        # Criptografar dados sensíveis
        encrypted_full_name = encrypt_data(full_name)
        encrypted_mother_name = encrypt_data(mother_name)
        encrypted_father_name = encrypt_data(father_name)
        encrypted_phone = encrypt_data(phone)

        # Inserir o registro no banco de dados
        connection = sqlite3.connect('database.db')
        cursor = connection.cursor()
        cursor.execute('''INSERT INTO records (
            registration_number, full_name, sex, date_of_birth, marital_status, nickname,
            mother_name, father_name, nationality, state, country, address, house_number,
            neighborhood, city, postal_code, phone, email
        ) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?)''', (
            registration_hash, encrypted_full_name, sex, date_of_birth, marital_status, nickname,
            encrypted_mother_name, encrypted_father_name, nationality, state, country, address, house_number,
            neighborhood, city, postal_code, encrypted_phone, email
        ))
        connection.commit()
        connection.close()

        flash('Registro inserido com sucesso!', 'success')
    except Exception as e:
        flash(str(e), 'error')

    return redirect('/')

if __name__ == '__main__':
    create_table()
    app.run(host='localhost', port=5000)

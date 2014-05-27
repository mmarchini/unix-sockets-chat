Unix Sockets Chat
==============

# Introdução

Uma aplicação Cliente/Servidor desenvolvida utilizando `Unix Sockets`, `pthreads` e `ncurses`.

# Preparação

Primeiro, é necessário ter as bibliotecas `pthread` e `ncurses` instaladas no sistema. Caso não tenha, você pode instalar utilizando algum dos comandos abaixo:

## Ubuntu:

    sudo apt-get install libncurses-dev libpthread-dev

## Fedora:

    sudo yum install libncurses-dev libpthread-dev

# Compilação

Para compilar, basta executar o comando:

    make

# Executando

## Servidor

Para executar o servidor, basta executar o comando:

    ./bin/server

Isso inicializará o servidor na porta 4000. Opcionalmente, é possível passar um parâmetro para o servidor, indicando em qual porta você gostaria de inicializar ele. Por exemplo, caso queira inicializar o servidor na porta 8080, basta executar o comando:

    ./bin/server 8080

# Cliente

Para executar o cliente, é necessário passar 3 parâmetros, `ip_servidor porta nome_usuario`. Por exemplo, caso queira se conectar ao servidor `localhost`, na porta `8080`, com o nome de usuário `João`, basta executar:

    ./bin/client localhost 8080 João

### Lista de Comandos

    \create Nome_da_Sala : Cria uma nova sala e se conecta a ela.
    \join Nome_da_Sala   : Se conecta a uma sala, caso ela exista.
    \leave               : Sai da sala que o usuário está conectado.
    \rooms               : Lista as salas abertas atualmente no servidor.
    \users               :  Lista os usuários presentes na sala.
    \exit                : Desconecta o cliente do servidor.


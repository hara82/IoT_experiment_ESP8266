#! /usr/bin/env python3
# coding:utf-8
# tcp_server
import socket
import threading
import re
import datetime

# True when BZ is set on.
buzzar_status = False

def user_buzzar_input_handler():
    global buzzar_status
    while True:
        user_input = input()
        print(f'[*] You typed:{user_input}')
        if (user_input == "on"):
            buzzar_status = True
        elif(user_input == "off"):
            buzzar_status = False
        if (user_input == "deactivate"):
            buzzar_status = False
            return


def validate_message(message):
    # 正規表現でCSVフォーマットをチェック
    pattern = r"^(0|1|2|3),\d{4}-\d{2}-\d{2}T\d{2}:\d{2}:\d{2},\d+.?\d+,(0|1)$"
    return re.match(pattern, message)

def handle_client(client_socket):
    # global variable for checking buzzar status
    global buzzar_status
    bufsize = 1024
    
    # クライアントソケットに10sのタイムアウトを設定
    client_socket.settimeout(10.0)
    try:
        request = client_socket.recv(bufsize)
        request_str = request.decode().strip()
        print('[*] Received:', request_str)

        # メッセージのフォーマットをチェック
        if validate_message(request_str):
            response = "OK\r\n"
            # ファイルに追記
            with open("IoT_exercise1_received_data.csv", "a") as file:
                file.write(request_str + "\n")
        else:
            response = "ERROR\r\n"

        # クライアントに返信
        client_socket.send(response.encode())
        print(f'[*] Sent {response}')
        # ブザーON/OFF信号を送信
        buzzar_message = "$buzzar:on" if buzzar_status else "$buzzar:off"
        client_socket.send(buzzar_message.encode())
        print(f'[*] {buzzar_message}')

    except socket.timeout:
        print("[*] Timeout")

    finally:
        #　コネクションを切断
        client_socket.close()

def start_server(bind_ip, bind_port):
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((bind_ip, bind_port))
    server.listen(5)
    print('[*] Listening on %s:%d' % (bind_ip, bind_port))


    # For controlling buzzar
    buzzar_handler = threading.Thread(target=user_buzzar_input_handler)
    buzzar_handler.daemon = True # メインプログラムが終了したら、スレッドも終了させる。
    buzzar_handler.start();

    while True:
        client, addr = server.accept()
        print('[*] Accepted connection from: %s:%d' % (addr[0], addr[1]))
        client_handler = threading.Thread(target=handle_client, args=(client,))
        client_handler.start()

bind_ip = '0.0.0.0'
bind_port = 10211  # ** 割り当てられたポート番号を使用せよ **
start_server(bind_ip, bind_port)
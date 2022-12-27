#!/usr/bin/python3
'''
docstring
'''

import subprocess
import os
from http import HTTPStatus
import mimetypes
import time
import socket
import datetime

DEBUG = 1
DEFAULT_RUNTIME_DIR = '.'
ADD_HANDLER = ['.cgi','.py']
DIRECTORY_INDEX = '/index.html' # if /
HTTP_VERSION = 'HTTP/1.0'
HTTP_ACCEPT_VERSION = 'HTTP/1.1'

IF404SEND = b'''\
Content-Type: text/html; charset=utf-8


<html>
<body>
<a href="./">Up</a>
<p>Requesting file Not Found!</p>
</body>
</html>
'''

def parse_headers(httpdata):
    '''
    Parsing HTTP Headers
    '''
    env = {}
    httpdata = httpdata.replace('\r', '')
    httpdata = httpdata.split('\n')
    method,url,version = httpdata[0].split(' ')
    if version not in [HTTP_ACCEPT_VERSION, HTTP_VERSION]:
        print(version.encode())
        return HTTPStatus.HTTP_VERSION_NOT_SUPPORTED, b"\r\n\r\n n"
    os.environ["REQUEST_METHOD"] = method
    if '?' in url:
        path,getparams = url.split('?')
    else:
        path = url
        getparams = ''
    if path == '/exit':
        print("Server stopped")
        exit()
    os.environ['QUERY_STRING'] = getparams
    postdata = ''
    flag_post = 0
    lenthOfHttpData = len(httpdata)
    for iter, dataInHttp in enumerate(httpdata):
        if flag_post:
            if iter == lenthOfHttpData - 1:
                postdata += dataInHttp
            else:
                postdata += dataInHttp+'\n'
        elif dataInHttp == '':
            flag_post=1
        else:
            if ':' in dataInHttp:
                env_key, env_value = dataInHttp.split(":", maxsplit = 1)
                env_key = env_key.upper().replace("-","_")
                env_value = env_value[1:]
                if env_key == "COOKIE":
                    env_key = "HTTP_COOKIE"

                env[env_key] = env_value
                os.environ[env_key] = env_value

    if os.path.isdir(DEFAULT_RUNTIME_DIR+path):
        path += DIRECTORY_INDEX
    if DEBUG:
        print('REQUESTING FILE: ', path.encode())
    else:
        print(method, url, end=' ',flush=True)

    if not os.path.exists('./'+path):
        return HTTPStatus.NOT_FOUND, IF404SEND
    ext = os.path.splitext(path)[-1].lower()
    if ext in ADD_HANDLER:
        if DEBUG:
            print("POST to send: ", postdata)
        code, result_data = run_cgi(ext, DEFAULT_RUNTIME_DIR+path, postdata)
    else:
        code, result_data = send_file(DEFAULT_RUNTIME_DIR+path)
    return code, result_data

def run_cgi(ext, path, postdata):
    if DEBUG:
        print("ext ",ext)
    my_env = os.environ.copy()
    # Отправка данных в процесс
    server_subprocess = subprocess.Popen(path,stdout=subprocess.PIPE,stdin=subprocess.PIPE,
                                         stderr=subprocess.PIPE, env=my_env)
    out, err = server_subprocess.communicate(input=bytes(postdata,encoding='utf8'))
    print("CGI", end='')
    if server_subprocess.returncode == 0:
        ret = out
        return HTTPStatus.OK, ret
    else:
        print("\nerror! log:", err)
        return HTTPStatus.INTERNAL_SERVER_ERROR, b' '+bytes(server_subprocess.returncode)


def send_file(path):
    mime = mimetypes.guess_type(path)[0]
    if not mime:
        mime = 'text/html'
    if DEBUG:
        print("SEND FILE "+mime)
    try:
        with(open(path,"rb")) as f:
            filedata = f.read()
    except FileNotFoundError:
        return HTTPStatus.NOT_FOUND, IF404SEND
    return HTTPStatus.OK, b'Content-Type: '+bytes(mime,encoding='utf8')+b'; charset=UTF-8\r\n\r\n'\
           +filedata

def main():
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_address = ('0.0.0.0', 5000)
    print(f'Старт сервера на {server_address[0]} порт {server_address[1]}')
    # привязка содержащий два элемента: хост и порт
    sock.bind(server_address)
    # прослушка (максимальное число соединений)
    sock.listen(1)

    while True:
        print('waiting connection...', end='', flush=True)
        # принимаем соединение и возвращает связанный с ним сокет и адрес подключившегося
        conn, client_address = sock.accept()
        try:
            now = datetime.datetime.now()
            print(f'\r            \r>{now} ', client_address, end=' ',flush=True)
            while True:
                datamas = []
                data = b''
                while True:
                    time.sleep(2)
                    # чтение данных(кол-во получаемых байт)
                    drecv = conn.recv(2048)
                    data += drecv
                    if DEBUG:
                        print("d", drecv)
                    if len(drecv) < 2048:
                        break
                if DEBUG:
                    print('Получено: ')
                    for i in datamas:
                        print(i)
                    print('END\n',flush=True)
                if not data:
                    print("No data")
                    conn.close()
                if data:
                    print('...',end=' ',flush=True)
                    # декодирование данных
                    code, senddata = parse_headers(data.decode())
                    print('>>>>',end=' ',flush=True)
                    # HTTP/1.0 200 OK
                    retdata =  HTTP_VERSION+' '+str(code.value)+' '+code.phrase+'\n'\
                        +"Content-Length: "+str(len(senddata))+'\n' + "Connection: close\n"
                    # кодирование
                    retdata = retdata.encode('utf8')
                    retdata += senddata
                    print(code.value,code.phrase, datetime.datetime.now() - now ,end=' ',flush=True)
                    # отправка данных в сокет
                    conn.sendall(retdata)
                    if DEBUG:
                        print(f'SEND: {len(retdata)}',flush=True)
                    print()
                    break
                else:
                    print('Нет данных от:', client_address,flush=True)
                    break
        # закрытие сокета
        except KeyboardInterrupt:
            conn.close()
            sock.close()
            print("\nconnection close")
            print("stopped")
            exit()
        finally:
            conn.close()
if __name__ == "__main__":
    while True:
        try:
            main()
        except KeyboardInterrupt:
            print("\nstopped")
            exit()

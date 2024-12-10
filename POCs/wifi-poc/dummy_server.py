import math
import time
import threading
import socket

from enum import Enum, auto

#import select

_HOSTNAME: str = "localhost"
_PORT: int = 9999
_SERVER_SOCK: socket.socket = None
_CLIENT_SOCK: socket.socket = None

_DEVICE_ID = "ESP32"
_WIDTH = 10
_HEIGHT = 5

class MESSAGE_TYPE(Enum):
    CMD_LED = 0,
    ANIMATION_CTRL = 1,
    CONNECT_REQUEST = 2,
    CONNECT_REPLY = 3,
    LAST_MSG_TYPE = 4,


def parseMessage(data: bytes):
    match(int.from_bytes(data[:4])):
        case CONNECT_REQUEST:
            arr = bytearray([
                MESSAGE_TYPE.CONNECT_REPLY.value[0], 0, 0, 0,
                len(_DEVICE_ID) + 1 + 8, 0, 0, 0,
                _HEIGHT, 0, 0, 0,
                _WIDTH, 0, 0, 0
            ])
            arr += bytearray(_DEVICE_ID.encode(encoding="ascii"))
            arr += bytearray([0])
            
            _CLIENT_SOCK.send(arr)


def print_header(len: int) -> None:
    print("     ", end="")
    for i in range(0, len):
        print(f"{i:02x} ", end="")
    
    print()


def print_bytes(data: bytes, line_length: int) -> None:
    print_header(line_length)
    
    data_arr = bytearray(data)
    
    for y in range(0, math.ceil(len(data_arr)/line_length)):
        line = data_arr[y * line_length: (y+1) * line_length]
        print(f"{y*line_length:04x} ", end="")
        
        for b in line:
            print(f"{b:02x} ", end="")
            
        if(len(line) < line_length):
            print("   " * (line_length - len(line)), end="")
            
        print("\t", end="")
            
        for b in line:
            val = int.to_bytes(b)
            if 32 < b < 127:
                print(val.decode(encoding="ascii"), end="")
            else:
                print(".", end="")
        
        print()
    print()
        

def configure_socket():
    global _SERVER_SOCK
    
    _SERVER_SOCK = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    _SERVER_SOCK.bind((_HOSTNAME, _PORT))
    _SERVER_SOCK.listen(1)
    
    
def main():
    global _CLIENT_SOCK
    while True:
        client_address: socket._RetAddress = None 
        
        print("Waiting for connection...")
        _CLIENT_SOCK, client_address = _SERVER_SOCK.accept()
        
        print(f"Connection established from {client_address[0]}:{client_address[1]}")
        
        while True:
            try:
                data: bytes = _CLIENT_SOCK.recv(8196)
            except socket.error as e:
                break
            
            if len(data) > 0:
                print_bytes(data, 16)
                parseMessage(data)
            else:
                break;
        

if __name__ == '__main__':
    configure_socket()
    thread: threading.Thread = threading.Thread(
        target=main, 
        args=()
    )
    
    thread.daemon = True
    thread.start()
    
    while(True):
        time.sleep(1)

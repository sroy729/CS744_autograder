import socket
HOST = "127.0.0.1"
PORT = 2002

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
    s.connect((HOST, PORT))
    print(f"Conneted by {HOST}:{PORT}")
    for i in range(3):
        by = int.to_bytes(4146, 4, 'little')
        s.send(by)
        f = open("test_files/program_runs.cpp", "rb")
        s.send(f.read())
        data = s.recv(1024)
        s.recv(1024)
        sleep(2)
    

from __future__ import annotations
from collections import namedtuple
from multiprocessing import Process
import socket
import time


Address = namedtuple('Address', 'ip port')


def send_dummy_messages(address: Address, delay_sec: int, duration_sec: int,
                        rate_per_sec: int = 2) -> None:
    proc = Process(target=send_messages_handler, args=(address, rate_per_sec))
    time.sleep(delay_sec)
    proc.start()
    time.sleep(duration_sec)
    proc.kill()


def send_messages_handler(address: Address, rate_per_sec: int):
    while True:
        with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sock:
            for i in range(0, rate_per_sec - 1):
                sock.sendto(bytes('Dummy Message', 'utf-8'),
                            (address.ip, address.port))

        time.sleep(1)

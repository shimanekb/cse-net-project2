import argparse

import client.client as client


def main():
    parser = argparse.ArgumentParser(description="""Client for sending dummy
                                                    messages to an ip""")

    parser.add_argument('ip', type=str, help='Target ip to send to')
    parser.add_argument('port', type=int, help='Target port to send to')
    parser.add_argument('delay', type=int, help="""Delay (seconds)
                                                   before sending messages""")

    parser.add_argument('duration', type=int, help="""Duration (seconds)
                                                      of sending messages""")

    args = parser.parse_args()
    ip = args.ip
    port = args.port
    address = client.Address(ip, port)
    delay = args.delay
    duration = args.duration

    client.send_dummy_messages(address, delay, duration)


if __name__ == '__main__':
    main()

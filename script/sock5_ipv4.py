#!/bin/python3
import requests


def main():
    proxies = {
        'http': "socks5://127.0.0.1:7448",
        'https': "sock5://127.0.0.1:7448"
    }
    res = requests.get('http://www.baidu.com', proxies=proxies, timeout=1)
    if res.status_code != '200':
        return False
    else:
        return True


main()

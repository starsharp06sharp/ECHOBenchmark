#!/usr/bin/env python
# -*- coding: utf-8 -*-
import fire
import time
import sys
import socket
from multiprocessing import Pool

if sys.platform == 'win32':
    timefun = time.clock
else:
    timefun = time.time


def measure_time(fun, funargs):
    stime = timefun() * 1000  # translate seconds to milliseconds
    fun(*funargs)
    etime = timefun() * 1000  # translate seconds to milliseconds
    return etime - stime


def echo_request(host, port, msg, repeat_times):
    try:
        s = socket.socket()
        s.connect((host, port))
        for i in range(repeat_times):
            s.send(msg)
            recv_data = s.recv(4096)
            if recv_data != msg:
                raise Exception('recv error')
    finally:
        s.close()


def main(conn_num=1024, host='localhost', port=7, echo_msg="Hello, this is a test", repeat_times=16):
    p = Pool(4)
    res = []
    for i in range(conn_num):
        res.append(
            p.apply_async(measure_time, (echo_request, (host, port, echo_msg, repeat_times)))
        )
        # measure_time(echo_request, (port, echo_msg, repeat_times))
    p.close()
    p.join()
    tot_time = 0
    count = 0
    for i in range(conn_num):
        try:
            t = res[i].get(timeout=1)
        except Exception, e:
            print 'Error when do request!'
        else:
            tot_time += t
            count += 1
    print 'Total time: %.1d ms, succeed connection num: %d' % (tot_time, count)

if __name__ == '__main__':
    fire.Fire(main)

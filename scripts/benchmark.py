#!/usr/bin/env python3
import socket
import time
import concurrent.futures

HOST = "127.0.0.1"
PORT = 8080
NUM_REQUESTS = 1000
CONCURRENCY = 50

def send_request(_):
    start = time.perf_counter()
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.settimeout(2.0)
        s.connect((HOST, PORT))
        s.sendall(b"GET / HTTP/1.1\r\nHost: localhost\r\nConnection: close\r\n\r\n")
        
        response = b""
        while True:
            data = s.recv(1024)
            if not data:
                break
            response += data
        s.close()
        latency = time.perf_counter() - start
        return True, latency
    except Exception:
        latency = time.perf_counter() - start
        return False, latency

def run_benchmark():
    print(f"[+] Benchmarking sysmon_v2 on {HOST}:{PORT}")
    print(f"[+] Total Requests: {NUM_REQUESTS} | Concurrency Level: {CONCURRENCY}")
    print("-" * 55)

    start_time = time.perf_counter()
    successes = 0
    failures = 0
    latencies = []

    with concurrent.futures.ThreadPoolExecutor(max_workers=CONCURRENCY) as executor:
        results = list(executor.map(send_request, range(NUM_REQUESTS)))

    total_time = time.perf_counter() - start_time

    for ok, lat in results:
        if ok:
            successes += 1
            latencies.append(lat)
        else:
            failures += 1

    avg_latency_ms = (sum(latencies) / len(latencies)) * 1000 if latencies else 0
    rps = NUM_REQUESTS / total_time

    print("\n=== Benchmark Results ===")
    print(f"Total Time Elapsed:    {total_time:.3f} s")
    print(f"Successful Requests:   {successes}")
    print(f"Failed Requests:       {failures}")
    print(f"Requests Per Second:   {rps:.2f} req/s")
    print(f"Average Latency:       {avg_latency_ms:.2f} ms")
    print("=========================")

if __name__ == "__main__":
    run_benchmark()

# epoll c server with blocking client

Running 30s test @ http://127.0.0.1:3000/test
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    23.52ms  121.16ms   1.79s    95.29%
    Req/Sec     2.58k     1.87k   13.66k    68.05%
  Latency Distribution
     50%  343.00us
     75%  399.00us
     90%  484.00us
     99%  647.81ms
  852049 requests in 30.10s, 108.89MB read
  Socket errors: connect 0, read 0, write 0, timeout 69
Requests/sec:  28307.79
Transfer/sec:      3.62MB

# epoll c server with non blocking client
Running 30s test @ http://127.0.0.1:3000/test
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    18.62ms  100.35ms   1.79s    95.34%
    Req/Sec     2.67k     2.24k   17.74k    71.27%
  Latency Distribution
     50%  332.00us
     75%  408.00us
     90%  553.00us
     99%  454.52ms
  868705 requests in 30.10s, 111.01MB read
  Socket errors: connect 0, read 0, write 0, timeout 40
Requests/sec:  28860.53
Transfer/sec:      3.69MB

# bun + express
Running 30s test @ http://127.0.0.1:3000/test
  12 threads and 400 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    13.66ms    2.08ms  31.39ms   78.63%
    Req/Sec     2.43k   315.07     3.93k    76.30%
  Latency Distribution
     50%   13.12ms
     75%   14.33ms
     90%   16.78ms
     99%   20.50ms
  871012 requests in 30.10s, 192.71MB read
Requests/sec:  28941.51
Transfer/sec:      6.40MB


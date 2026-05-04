membench
=====
Benchmark to evaluate *memset* and *memcpy* performance.

Usage
-----
```
membench OPERATION [MIN_SIZE] [MAX_SIZE]
```

Supported options:
 * -h, --help: Show this help message and exit.

OPERATION must be either "memset" or "memcpy".  
MIN_SIZE and MAX_SIZE are optional parameters that specify the range of buffer sizes to test.
They are interpreted as powers of 2.
For example, if MIN_SIZE is 4 and MAX_SIZE is 8, the benchmark will test buffer sizes of 2^4 (16 bytes), 2^5 (32 bytes), 2^6 (64 bytes), 2^7 (128 bytes) and 2^8 (256 bytes).  
If not provided, the benchmark will use a default range of 2^10 (1 KiB) to 2^24 (16 MiB).

For each buffer size, the application will first perform a warmup run to ensure that the buffer is fully mapped and any necessary page faults have been handled.  
Then, it will perform the actual benchmark run 10 times and report the average time taken and corresponding throughput.

Examples
--------
```
[/]> membench memset 10 15
memset 1 KiB:	0.000004443s (230.47 MB/s)
memset 2 KiB:	0.000005048s (405.70 MB/s)
memset 4 KiB:	0.000006438s (636.22 MB/s)
memset 8 KiB:	0.000009217s (888.79 MB/s)
memset 16 KiB:	0.000015027s (1090.30 MB/s)
memset 32 KiB:	0.000040694s (805.22 MB/s)
[/]> membench memcpy 15 20
memcpy 32 KiB:	0.000047279s (693.07 MB/s)
memcpy 64 KiB:	0.000076125s (860.89 MB/s)
memcpy 128 KiB:	0.000134996s (970.93 MB/s)
memcpy 256 KiB:	0.000256173s (1023.30 MB/s)
memcpy 512 KiB:	0.000524359s (999.86 MB/s)
memcpy 1 MiB:	0.00098234s (1067.42 MB/s)
```
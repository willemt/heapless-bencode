What?
-----
A bencode reader that doesn't use the heap (ie. malloc/free).

Written in C with a BSD license.

How does it work?
-----------------

See bencode.h for documentation.

To see the module in action check out:

* Unit tests within test_bencode.c

* github.com:willemt/CTorrentFileReader

* github.com:willemt/CBTTrackerClient

Building
--------
$make

Tradeoffs
---------
If you've got the entire bencoded string in memory, CHeaplessBencodeReader is amazing - it'll do the job great!

Otherwise, if your best access is via a stream (eg. a massive bencoded string that you don't want in memory; or a TCP stream), then https://github.com/willemt/CStreamingBencodeReader is your poison! (Tradeoff: it uses the heap)

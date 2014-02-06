CHeaplessBencodeReader
==========
.. image:: https://travis-ci.org/willemt/CHeaplessBencodeReader.png
   :target: https://travis-ci.org/willemt/CHeaplessBencodeReader

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

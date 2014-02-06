CHeaplessBencodeReader
==========
.. image:: https://travis-ci.org/willemt/CHeaplessBencodeReader.png
   :target: https://travis-ci.org/willemt/CHeaplessBencodeReader

.. image:: https://coveralls.io/repos/willemt/CHeaplessBencodeReader/badge.png?branch=master
  :target: https://coveralls.io/r/willemt/CHeaplessBencodeReader?branch=master

Whatk
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

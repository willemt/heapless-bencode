import bencode
from cffi import FFI
from hypothesis import given
import collections
import hypothesis.strategies as st
import unittest


ffi = FFI()
C = ffi.dlopen('../libbencode.dylib')
ffi.cdef("""
    int bencode_validate(char* buf, int len);
""")

json_strat = st.recursive(
    st.integers() | st.binary(),
    lambda children: st.lists(children) | st.dictionaries(st.text(), children))


def convert(data):
    if isinstance(data, basestring):
        try:
            return data.encode('utf-8')
        except UnicodeDecodeError:
            return data
    elif isinstance(data, collections.Mapping):
        return dict(map(convert, data.iteritems()))
    elif isinstance(data, collections.Iterable):
        return type(data)(map(convert, data))
    else:
        return data


class TestEncoding(unittest.TestCase):
    @given(json_strat)
    def test_decode(self, items):
        print items

        if items == '':
            return

        items = convert(items)
        strs = bencode.bencode(items)
        strs = str(strs)
        ret = C.bencode_validate(ffi.new("char[]", strs), len(strs))
        self.assertEqual(ret, 0)


if __name__ == '__main__':
    unittest.main()

/*
 
Copyright (c) 2011, Willem-Hendrik Thiart
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * The names of its contributors may not be used to endorse or promote
      products derived from this software without specific prior written
      permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL WILLEM-HENDRIK THIART BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

*/

#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "CuTest.h"

#include "bencode.h"

void TestBencodeWontDoShortExpectedLength(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("4:test");

    const char *ren;

    int len;

    bencode_init(&ben, str, 3);
    CuAssertTrue(tc, 0 == bencode_string_value(&ben, &ren, &len));
    bencode_done(&ben);
    free(str);
}

void TestBencodeWontDoShortExpectedLength2(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("4:test");

    const char *ren;

    int len;

    bencode_init(&ben, str, 1);
    CuAssertTrue(tc, 0 == bencode_string_value(&ben, &ren, &len));
    bencode_done(&ben);
    free(str);
}

void TestBencodeIsInt(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("i666e");

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_is_int(&ben));
    free(str);
}

void TestBencodeIntValue(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("i666e");

    long int val;

    bencode_init(&ben, str, strlen(str));

    bencode_int_value(&ben, &val);
    CuAssertTrue(tc, 666 == val);
    free(str);
}

void TestBencodeIntValue2(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("i102030e");

    long int val;

    bencode_init(&ben, str, strlen(str));

    bencode_int_value(&ben, &val);
    CuAssertTrue(tc, 102030 == val);
    free(str);
}

void TestBencodeIntValueLarge(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("i2502875232e");

    long int val;

    bencode_init(&ben, str, strlen(str));

    bencode_int_value(&ben, &val);
    CuAssertTrue(tc, 2502875232 == val);
    free(str);
}

void TestBencodeIsIntEmpty(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup(" ");

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 0 == bencode_is_int(&ben));
    free(str);
}

void TestBencodeIsString(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("4:test");

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_is_string(&ben));
    free(str);
}

void TestBencodeStringValue(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("4:test");

    const char *ren;

    int len;

    //*ptr = 0;
    bencode_init(&ben, str, strlen(str));
    bencode_string_value(&ben, &ren, &len);
    CuAssertTrue(tc, !strncmp("test", ren, len));
    bencode_done(&ben);
    free(str);
}

void TestBencodeStringValue2(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("12:flyinganimal");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));
    bencode_string_value(&ben, &ren, &len);
    CuAssertTrue(tc, !strncmp("flyinganimal", ren, len));
    bencode_done(&ben);
    free(str);
}

/**
 * The string value function errors when the string is of insuficient length
 * */
void TestBencodeStringInvalid(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("5:test");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));
    bencode_string_value(&ben, &ren, &len);
    CuAssertTrue(tc, !ren);
    bencode_done(&ben);
    free(str);
}

void TestBencodeIsStringEmpty(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup(" ");

    bencode_init(&ben, str, strlen(str));
    CuAssertTrue(tc, 0 == bencode_is_string(&ben));
    free(str);
}

void TestBencodeStringHandlesNonAscii0(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("6:xxxxxx");

    const char *ren;

    int len;

    /*  127.0.0.1:80 */
    str[2] = 127;
    str[3] = 0;
    str[4] = 0;
    str[5] = 1;
    str[6] = 0;
    str[7] = 80;

    bencode_init(&ben, str, 8);

    bencode_string_value(&ben, &ren, &len);
    CuAssertTrue(tc, ren[0] == 127);
    CuAssertTrue(tc, ren[1] == 0);
    CuAssertTrue(tc, ren[2] == 0);
    CuAssertTrue(tc, ren[3] == 1);
    CuAssertTrue(tc, ren[4] == 0);
    CuAssertTrue(tc, ren[5] == 80);
    bencode_done(&ben);
    free(str);
}

void TestBencodeIsList(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("l4:test3:fooe");

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_is_list(&ben));
    free(str);
}

void TestBencodeIsListEmpty(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup(" ");

    bencode_init(&ben, str, strlen(str));
    CuAssertTrue(tc, 0 == bencode_is_list(&ben));
    free(str);
}

void TestBencodeListHasNext(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("l4:test3:fooe");

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_list_has_next(&ben));
    free(str);
}

void TestBencodeListGetNext(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("l3:foo3:bare");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_list_get_next(&ben, &ben2));
    bencode_string_value(&ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp("foo", ren, len));
    free(str);
}

void TestBencodeListGetNextTwiceWhereOnlyOneAvailable(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("l4:teste");

    bencode_init(&ben, str, strlen(str));
    CuAssertTrue(tc, 1 == bencode_list_get_next(&ben, &ben2));
    CuAssertTrue(tc, 0 == bencode_list_get_next(&ben, &ben2));
    free(str);
}

void TestBencodeListGetNextTwice(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("l4:test3:fooe");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_list_get_next(&ben, &ben2));
    bencode_string_value(&ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp("test", ren, len));

    CuAssertTrue(tc, 1 == bencode_list_get_next(&ben, &ben2));
    bencode_string_value(&ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp("foo", ren, len));

//    printf("%s\n", str);
//    CuAssertTrue(tc, !strcmp("l4:test3:fooe", str));
    free(str);
}

void TestBencodeListGetNextAtInvalidEnd(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("l4:testg");

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_list_get_next(&ben, &ben2));
    CuAssertTrue(tc, -1 == bencode_list_get_next(&ben, &ben2));

    free(str);
}

void TestBencodeIsDict(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("d4:test3:fooe");

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_is_dict(&ben));
    free(str);
}

void TestBencodeIsDictEmpty(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup(" ");

    bencode_init(&ben, str, strlen(str));
    CuAssertTrue(tc, 0 == bencode_is_dict(&ben));
    free(str);
}

void TestBencodeDictHasNext(
    CuTest * tc
)
{
    bencode_t ben;

    char *str = strdup("d4:test3:fooe");

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_dict_has_next(&ben));
    free(str);
}

void TestBencodeDictGetNext(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("d3:foo3:bare");

    const char *ren;

    int len, ret;

    bencode_init(&ben, str, strlen(str));

    ret = bencode_dict_get_next(&ben, &ben2, &ren, &len);
    CuAssertTrue(tc, 1 == ret);
    CuAssertTrue(tc, !strncmp("foo", ren, len));
    bencode_string_value(&ben2, &ren, &len);
    CuAssertPtrNotNull(tc, ren);
    CuAssertTrue(tc, !strncmp("bar", ren, len));
    free(str);
}

#if 0
void TxestBencodeDictGetNextOnlyIfValidDictEnd(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    /*  no terminating 'e' */
    char *str = strdup("d3:foo3:barz");

    const char *ren;

    int len, ret;

    bencode_init(&ben, str, strlen(str));

    ret = bencode_dict_get_next(&ben, &ben2, &ren, &len);
    CuAssertTrue(tc, 0 == ret);
    free(str);
}
#endif

void TestBencodeDictGetNextTwice(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("d4:test3:egg3:foo3:hame");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));

    bencode_dict_get_next(&ben, &ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp(ren, "test", len));
    bencode_string_value(&ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp("egg", ren, len));

    bencode_dict_get_next(&ben, &ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp(ren, "foo", len));
    bencode_string_value(&ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp("ham", ren, len));
//    printf("%s\n", str);
//    CuAssertTrue(tc, !strcmp("l4:test3:fooe", str));
    free(str);
}

void TestBencodeDictGetNextTwiceOnlyIfSecondKeyValid(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("d4:test3:egg2:foo3:hame");

    const char *ren;

    int len, ret;

    bencode_init(&ben, str, strlen(str));

    bencode_dict_get_next(&ben, &ben2, &ren, &len);

    ret = bencode_dict_get_next(&ben, &ben2, &ren, &len);
    CuAssertTrue(tc, ret == 0);
    free(str);
}

void TestBencodeDictGetNextInnerList(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    bencode_t ben3;

    char *str = strdup("d3:keyl4:test3:fooee");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));

    bencode_dict_get_next(&ben, &ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp(ren, "key", len));

    bencode_list_get_next(&ben2, &ben3);
    bencode_string_value(&ben3, &ren, &len);
    CuAssertTrue(tc, !strncmp("test", ren, len));

    bencode_list_get_next(&ben2, &ben3);
    bencode_string_value(&ben3, &ren, &len);
    CuAssertTrue(tc, !strncmp("foo", ren, len));

    CuAssertTrue(tc, !bencode_dict_has_next(&ben));
//    printf("%s\n", str);
//    CuAssertTrue(tc, !strcmp("l4:test3:fooe", str));
    free(str);
}

void TestBencodeDictInnerList(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("d3:keyl4:test3:fooe3:foo3:bare");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));

    bencode_dict_get_next(&ben, &ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp(ren, "key", len));

    bencode_dict_get_next(&ben, &ben2, &ren, &len);
    CuAssertTrue(tc, !strncmp(ren, "foo", len));

    CuAssertTrue(tc, !bencode_dict_has_next(&ben));
//    printf("%s\n", str);
//    CuAssertTrue(tc, !strcmp("l4:test3:fooe", str));
    free(str);
}

void TestBencodeCloneClones(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("d3:keyl4:test3:fooe3:foo3:bare");

    bencode_init(&ben, str, strlen(str));

    bencode_clone(&ben, &ben2);

    CuAssertTrue(tc, !strcmp(ben.str, ben2.str));
    CuAssertTrue(tc, !strcmp(ben.start, ben2.start));
    CuAssertTrue(tc, ben.len == ben2.len);
    free(str);
}

void TestBencodeDictValueAsString(
    CuTest * tc __attribute__((__unused__))
)
{
#if 0
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("d3:keyl4:test3:fooe3:foo3:bare");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));

//    bencode_dict_get_value_as_string(&ben, &ren, &len);

    CuAssertTrue(tc, len == strlen("l4:test3:fooe"));
    CuAssertTrue(tc, !strncmp(ren, "l4:test3:fooe", len));
    free(str);
#endif
}

void TestBencodeDictValueAsString2(
    CuTest * tc __attribute__((__unused__))
)
{
#if 0
    bencode_t ben;

    bencode_t ben2;

    char *str = strdup("d4:infod3:keyl4:test3:fooe3:foo3:baree");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));

    bencode_dict_get_next(&ben, &ben2, &ren, &len);

    bencode_t ben3;

    bencode_dict_get_next(&ben2, &ben3, &ren, &len);

    printf("FFFF%s\n", ren);

    while (bencode_dict_has_next(&ben2))
    {

        bencode_dict_get_next(&ben2, &ben3, &ren, &len);
    }

    printf("BBBB: %s\n", ben2.str);
//    bencode_dict_get_value_as_string(&ben, &ren, &len);

    CuAssertTrue(tc, len == strlen("l4:test3:fooe"));
    CuAssertTrue(tc, !strncmp(ren, "l4:test3:fooe", len));
    free(str);
#endif
}

void TestBencodeDictGetStartAndLen(
    CuTest * tc
)
{
    bencode_t ben;

    bencode_t ben2;

    char *expected = "d3:keyl4:test3:fooe3:foo3:bare";

    char *str = strdup("d4:infod3:keyl4:test3:fooe3:foo3:baree");

    const char *ren;

    int len;

    bencode_init(&ben, str, strlen(str));

    bencode_dict_get_next(&ben, &ben2, &ren, &len);
    bencode_dict_get_start_and_len(&ben2, &ren, &len);

//    printf("FFF: %d '%s'\n", len, ren);
//    printf("%d\n", strlen(expected));

    CuAssertTrue(tc, len == (int)strlen(expected));
    CuAssertTrue(tc, !strncmp(ren, expected, len));
    free(str);
}

/*----------------------------------------------------------------------------*/

void TestBencodeStringValueIsZeroLength(
    CuTest * tc
)
{

    bencode_t ben, benk;

    char *str = strdup("d8:intervali1800e5:peers0:e");

    const char *key;

    const char *val;

    int klen, vlen;

    bencode_init(&ben, str, strlen(str));

    CuAssertTrue(tc, 1 == bencode_is_dict(&ben));
    CuAssertTrue(tc, 1 == bencode_dict_has_next(&ben));

    bencode_dict_get_next(&ben, &benk, &key, &klen);
    CuAssertTrue(tc, !strncmp(key, "interval", klen));
    CuAssertTrue(tc, 1 == bencode_dict_has_next(&ben));
    bencode_dict_get_next(&ben, &benk, &key, &klen);
    CuAssertTrue(tc, !strncmp(key, "peers", klen));
    bencode_string_value(&benk, &val, &vlen);
    CuAssertTrue(tc, !strncmp(val, "", vlen));

    free(str);
}

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

#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include <setjmp.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "bencode.h"

static int __carry_length(
    bencode_t * be,
    const char *pos
)
{
    assert(0 < be->len);
    return be->len - (pos - be->str);
}

static long int __read_string_int(
    const char *ptr,
    const char **end
)
{
    long int val = 0;

    assert(isdigit(*ptr));

    /* work out number */
    do
    {
        val *= 10;
        val += *ptr - '0';
        ptr++;
    }
    while (isdigit(*ptr));

    *end = ptr;

    return val;
}

bool bencode_is_dict(
    bencode_t * be
)
{
    const char *ptr;

    ptr = be->str;

    assert(ptr);

    return ptr[0] == 'd';
}

static const char *__iterate_to_next_string_pos(
    bencode_t * be,
    const char *ptr
)
{
    bencode_t iter;

    bencode_init(&iter, ptr, __carry_length(be, ptr));

    if (bencode_is_dict(&iter))
    {
        while (bencode_dict_has_next(&iter))
        {
            if (-1 == bencode_dict_get_next(&iter, NULL, NULL, NULL))
            {
                printf("error, DICT invalud: %s\n", iter.str);
                return 0;
            }
        }
        return iter.str + 1;
    }
    else if (bencode_is_list(&iter))
    {
        while (bencode_list_has_next(&iter))
        {
            if (-1 == bencode_list_get_next(&iter, NULL))
            {
                printf("error, LIST invalud: %s\n", iter.str);
                return 0;
            }
        }

        return iter.str + 1;
    }
    else if (bencode_is_string(&iter))
    {
        int len;

        const char *str;

        if (0 == bencode_string_value(&iter, &str, &len))
        {
            printf("error STRING invalid; '%s'\n", iter.str);
            return NULL;
        }

        return str + len;
    }
    else if (bencode_is_int(&iter))
    {
        const char *end;

        __read_string_int(&iter.str[1], &end);

        assert(end[0] == 'e');

        return end + 1;
    }

    assert(0);
    return NULL;
}

static const char *__read_string_len(
    const char *ptr,
    int *slen
)
{
    *slen = 0;

    if (!isdigit(*ptr))
        return NULL;

    /* work out number */
    do
    {
        *slen *= 10;
        *slen += *ptr - '0';
        ptr++;
    }
    while (isdigit(*ptr));

    assert(*ptr == ':');

    ptr++;

    assert(0 < *slen);

    return ptr;
}

void bencode_init(
    bencode_t * be,
    const char *str,
    int len
)
{
    memset(be, 0, sizeof(bencode_t));

    be->start = str;
    be->str = str;
    be->len = len;
    assert(0 < be->len);
}

bool bencode_is_int(
    bencode_t * be
)
{
    return (be->str[0] == 'i');
}

void bencode_done(
    bencode_t * be
)
{

}

bool bencode_is_list(
    bencode_t * be
)
{
    const char *ptr;

    ptr = be->str;

    assert(ptr);

    return ptr[0] == 'l';
}

bool bencode_is_string(
    bencode_t * be
)
{
    const char *ptr;

    ptr = be->str;

    assert(ptr);

    if (!isdigit(*ptr))
        return 0;

    do
    {
        ptr++;
    }
    while (isdigit(*ptr));

    return *ptr == ':';
}

/**
 * @return 1 on success, 0 otherwise */
int bencode_int_value(
    bencode_t * be,
    long int *val
)
{
    const char *end;

    *val = __read_string_int(&be->str[1], &end);

    assert(end[0] == 'e');

    return 1;
}

bool bencode_dict_has_next(
    bencode_t * be
)
{
    const char *ptr;

//    assert(bencode_is_dict(be));

    ptr = be->str;

    assert(ptr);

    if (*ptr == 'e')
    {
        return 0;
    }

    return 1;
}

/**
 * get the next item.
 * @param be_item : next item.
 * @param key : key of next item
 * @param klen : length of the key of next item. */
int bencode_dict_get_next(
    bencode_t * be,
    bencode_t * be_item,
    const char **key,
    int *klen
)
{
    const char *ptr;

    const char *keyin;

    int len;


//    assert(bencode_is_dict(be));

    ptr = be->str;

//    assert(be->str[0] != 'e');

    assert(be->str[0] != '\0');
    assert(be->str[0] != 'e');

    if (be->str[0] == 'd')
    {
        ptr++;
    }
    else if (be->str[0] == '\0')
    {
    }
    else if (be->str[0] == 'e')
    {
        assert(0);
        return 0;
    }

    keyin = __read_string_len(ptr, &len);

    if (be_item)
    {
        *klen = len;
        bencode_init(be_item, keyin + len, __carry_length(be, keyin + len));
    }

    /* iterate to next value */

    be->str = __iterate_to_next_string_pos(be, keyin + len);

    if (!be->str)
    {

        return 0;
    }

    assert(be->str);
//    assert(*key);

    if (key)
    {
        *key = keyin;
    }

    return 1;
}

int bencode_string_value(
    bencode_t * be,
    const char **str,
    int *slen
)
{
    const char *ptr;

    *slen = 0;

    assert(bencode_is_string(be));

    ptr = __read_string_len(be->str, slen);

    assert(ptr);
    assert(0 < be->len);

    if (ptr + *slen > be->start + (long int) be->len)
    {
        *str = NULL;
        return 0;
    }

    *str = ptr;

    return 1;
}

bool bencode_list_has_next(
    bencode_t * be
)
{
    const char *ptr;

//    assert(bencode_is_list(be));
    ptr = be->str;

    assert(ptr);

    if (*ptr == 'e')
    {
        return 0;
    }

    return 1;
}

/*
 *
 * @return 0 on end
 * @return 1 on have next
 * @return -1 on error
 */
int bencode_list_get_next(
    bencode_t * be,
    bencode_t * be_item
)
{
    const char *ptr;

//    assert(bencode_is_list(be));

    ptr = be->str;

    assert(be->str[0] != 'e');
//    printf("%s\n", be->str);

    if (be->str[0] == 'l')
    {
        ptr++;
    }
    else if (be->str[0] == '\0')
    {
        return -1;
    }
    else if (be->str[0] == 'e')
    {
        return 0;
    }

    if (be_item)
    {
        bencode_init(be_item, ptr, __carry_length(be, ptr));
    }

    /* iterate to next value */

    be->str = __iterate_to_next_string_pos(be, ptr);

    assert(be->str);

    return 1;
}

void bencode_clone(
    bencode_t * be,
    bencode_t * output
)
{
    memcpy(output, be, sizeof(bencode_t));
}

/**
 * get the start and end position of this dictionary
 * @param start : starting string
 * @parama len : len of the dictionary */
int bencode_dict_get_start_and_len(
    bencode_t * be,
    const char **start,
    int *len
)
{
    bencode_t ben, ben2;

    const char *ren;

    int tmplen;

    bencode_clone(be, &ben);

//    printf("%s\n", ben.str);
//    bencode_dict_get_next(&ben, &ben2, &ren, &tmplen);
//    bencode_dict_get_next(&ben, &ben2, &ren, &tmplen);

    *start = ben.str;

    while (bencode_dict_has_next(&ben))
    {
        bencode_dict_get_next(&ben, &ben2, &ren, &tmplen);
    }

    *len = ben.str - *start + 1;

    return 0;
}

#if 0
int bencode_dict_get_value_as_string(
    bencode_t * be,
    char **start,
    int *len
)
{
    bencode_t iter;

    bencode_t tmp;

    char *tmpk;

    int tmpklen;

    *start = be->str;

    bencode_clone(be, &iter);

//    printf("end::::: %s\n", iter.str);

    while (bencode_dict_has_next(&iter))
    {
        bencode_dict_get_next(&iter, &tmp, &tmpk, &tmpklen);
    }

//    printf("end::::: %s\n", iter.str);

    *len = iter.str - *start;

    printf("%s, %d\n", *start, *len);

    return 1;
}
#endif

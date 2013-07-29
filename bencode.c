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
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "bencode.h"

/**
 * Carry length over to a new bencode object.
 * This is done so that we don't exhaust the buffer */
static int __carry_length(
    bencode_t * be,
    const char *pos
)
{
    assert(0 < be->len);
    return be->len - (pos - be->str);
}

static long int __read_string_int(
    const char *sp,
    const char **end
)
{
    long int val = 0;

    assert(isdigit(*sp));

    /* work out number */
    do
    {
        val *= 10;
        val += *sp - '0';
        sp++;
    }
    while (isdigit(*sp));

    *end = sp;

    return val;
}

/**
* @return 1 if the bencode object is a dict; otherwise 0.
*/
int bencode_is_dict(
    bencode_t * be
)
{
    return be->str && *be->str == 'd';
}

/**
* @return 1 if the bencode object is an int; otherwise 0.
*/
int bencode_is_int(
    bencode_t * be
)
{
    return be->str && *be->str == 'i';
}

/**
* @return 1 if the bencode object is a list; otherwise 0.
*/
int bencode_is_list(
    bencode_t * be
)
{
    return be->str && *be->str == 'l';
}

/**
* @return 1 if the bencode object is a string; otherwise 0.
*/
int bencode_is_string(
    bencode_t * be
)
{
    const char *sp;

    sp = be->str;

    assert(sp);

    if (!isdigit(*sp))
        return 0;

    do
    {
        sp++;
    }
    while (isdigit(*sp));

    return *sp == ':';
}

/**
 * Move to next item
 * @param sp The bencode string we are processing
 * @return Pointer to string on success, otherwise NULL */
static const char *__iterate_to_next_string_pos(
    bencode_t * be,
    const char *sp
)
{
    bencode_t iter;

    bencode_init(&iter, sp, __carry_length(be, sp));

    if (bencode_is_dict(&iter))
    {
        /* navigate to the end of the dictionary */
        while (bencode_dict_has_next(&iter))
        {
            if (0 == bencode_dict_get_next(&iter, NULL, NULL, NULL))
            {
                /* ERROR: input string is invalid */
                return NULL;
            }
        }

        return iter.str + 1;
    }
    else if (bencode_is_list(&iter))
    {
        /* navigate to the end of the list */
        while (bencode_list_has_next(&iter))
        {
            if (-1 == bencode_list_get_next(&iter, NULL))
            {
                /* ERROR: input string is invalid */
                return NULL;
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
            /* input string is invalid */
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

    /* input string is invalid */
    return NULL;
}

static const char *__read_string_len(
    const char *sp,
    int *slen
)
{
    *slen = 0;

    if (!isdigit(*sp))
        return NULL;

    /* work out number */
    do
    {
        *slen *= 10;
        *slen += *sp - '0';
        sp++;
    }
    while (isdigit(*sp));

    assert(*sp == ':');

    sp++;

    assert(0 <= *slen);

    return sp;
}

/**
* Initialise a bencode object.
* @param be The bencode object
* @param str Buffer we expect input from
* @param len Length of buffer
*/
void bencode_init(
    bencode_t * be,
    const char *str,
    const int len
)
{
    memset(be, 0, sizeof(bencode_t));
    be->str = be->start = str;
    be->str = str;
    be->len = len;
    assert(0 < be->len);
}

/**
* Obtain value from integer bencode object.
* @param val Long int we are writing the result to
* @return 1 on success, otherwise 0
*/
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

/**
* @return 1 if there is another item on this dict; otherwise 0.
*/
int bencode_dict_has_next(
    bencode_t * be
)
{
    const char *sp;

    assert(be);

    sp = be->str;

    if (!sp || *sp == 'e' || *sp == '\0' || *sp == '\r'
        /* at the end of the input string */
        || be->str >= be->start + be->len - 1)
    {
        return 0;
    }

    return 1;
}

/**
* Get the next item within this dictionary.
* @return 1 on success; otherwise 0.
* @param be_item Next item.
* @param key Key of next item.
* @param klen Length of the key of next item.
*/
int bencode_dict_get_next(
    bencode_t * be,
    bencode_t * be_item,
    const char **key,
    int *klen
)
{
    const char *sp;
    const char *keyin;
    int len;

    sp = be->str;

    assert(*sp != 'e');

    /* if at start increment to 1st key */
    if (*sp == 'd')
    {
        sp++;
    }

    /* can't get the next item if we are at the end of the dict */
    if (*sp == 'e')
    {
        return 0;
    }

    /* 1. find out what the key's length is */
    keyin = __read_string_len(sp, &len);

    /* 2. if we have a value bencode, lets put the value inside */
    if (be_item)
    {
        *klen = len;
        bencode_init(be_item, keyin + len, __carry_length(be, keyin + len));
    }

    /* 3. iterate to next dict key, or move to next item in parent */
    if (!(be->str = __iterate_to_next_string_pos(be, keyin + len)))
    {
        /*  if there isn't anything else or we are at the end of the string */
        return 0;
    }

#if 0
    /*  if at the end of bencode, check that the 'e' terminator is there */
    if (be->str == be->start + be->len - 1 && *be->str != 'e')
    {
        be->str = NULL;
        return 0;
    }
#endif

    assert(be->str);

    if (key)
    {
        *key = keyin;
    }

    return 1;
}

/**
* Get the string value from this bencode object.
* The buffer returned is stored on the stack.
* @param be The bencode object.
* @param str Buffer that we are outputting to.
* @param slen Length of the buffer we are outputting.
* @return 1 on success; otherwise 0
*/
int bencode_string_value(
    bencode_t * be,
    const char **str,
    int *slen
)
{
    const char *sp;

    *slen = 0;
    
    assert(bencode_is_string(be));
    
    sp = __read_string_len(be->str, slen);
    
    assert(sp);
    assert(0 < be->len);
    
    /*  make sure we still fit within the buffer */
    if (sp + *slen > be->start + (long int) be->len)
    {
        *str = NULL;
        return 0;
    }

    *str = sp;
    return 1;
}

/**
* Tell if there is another item within this list.
* @param be The bencode object
* @return 1 if another item exists on the list; 0 otherwise; -1 on invalid processing
*/
int bencode_list_has_next(
    bencode_t * be
)
{
    const char *sp;

    sp = be->str;
    
    assert(sp);
    
    if (*sp == 'e')
    {
        return 0;
    }

    return 1;
}

/**
* Get the next item within this list.
* @param be The bencode object
* @param be_item The next bencode object that we are going to initiate.
* @return return 0 on end; 1 on have next; -1 on error
*/
int bencode_list_get_next(
    bencode_t * be,
    bencode_t * be_item
)
{
    const char *sp;

    sp = be->str;

    /* we're at the end */
    if (!sp || *sp == 'e')
        return 0;

#if 0 /* debugging */
    printf("%.*s\n", be->len - (be->str - be->start), be->str);
#endif

    if (*sp == 'l')
    {
        /* just move off the start of this list */
        if (be->start == be->str)
        {
            sp++;
        }
    }

    /* can't get the next item if we are at the end of the list */
    if (*sp == 'e')
    {
        be->str = sp;
        return 0;
    }

    /* populate the be_item if it is available */
    if (be_item)
    {
        bencode_init(be_item, sp, __carry_length(be, sp));
    }

    /* iterate to next value */
    if (!(be->str = __iterate_to_next_string_pos(be, sp)))
    {
        return -1;
    }

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
* Get the start and end position of this dictionary
* @param be Bencode object
* @param start Starting string
* @param len Length of the dictionary 
* @return 1 on success
**/
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
    *start = ben.str;
    while (bencode_dict_has_next(&ben))
    {
        bencode_dict_get_next(&ben, &ben2, &ren, &tmplen);
    }

    *len = ben.str - *start + 1;
    return 0;
}

/*
 * The Driver Station Library (LibDS)
 * Copyright (c) 2015-2017 Alex Spataru <alex_spataru@outlook>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "DS_String.h"

#include <stdio.h>
#include <assert.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

#define SPRINTF_S snprintf
#ifdef _WIN32
    #ifndef __MINGW32__
        #undef  SPRINTF_S
        #define SPRINTF_S sprintf_s
    #endif
#endif

/**
 * Returns the length of the given \a string
 * \warning The program will quit if \a string is \c NULL
 */
int DS_StrLen (const DS_String* string)
{
    assert (string);
    return (int) string->len;
}

/**
 * Returns a non-zero vaule if the given \a string is empty
 * \warning The program will quit if \a string is \c NULL
 */
int DS_StrEmpty (const DS_String* string)
{
    if (string)
        return DS_StrLen (string) == 0;

    return 1;
}

/**
 * @brief DS_StrCompare
 * @param a
 * @param b
 * @return
 */
int DS_StrCompare (const DS_String* a, const DS_String* b)
{
    /* Check arguments */
    assert (a);
    assert (b);

    /* Get length of both strings */
    int lenA = DS_StrLen (a);
    int lenB = DS_StrLen (b);

    /* Lengths are different */
    if (lenA != lenB) {
        if (lenA > lenB)
            return 1;
        else
            return -1;
    }

    /* Get the largest length */
    int minL = lenA > lenB ? lenA : lenB;

    /* Compare the buffers of both strings */
    int cmp = memcmp (a->buf, b->buf, minL);
    return cmp;
}

/**
 * Deletes the data buffer of the given \a string.
 * If the data buffer is already freed, then this function
 * shall have no effect.
 *
 * \warning The program will quit if \a string is \c NULL
 */
int DS_StrRmBuf (DS_String* string)
{
    /* Check parameters */
    assert (string);

    /* Delete the buffer */
    if (string->buf != NULL) {
        string->len = 0;
        free (string->buf);
        string->buf = NULL;
        return DS_STR_SUCCESS;
    }

    /* Buffer already freed */
    return DS_STR_FAILURE;
}

/**
 * Resizes the given \a string to the given \a size
 *
 * \param string the original string structure
 * \param size the new size to apply to the string
 *
 * \warning The program will quit if \a string is \c NULL
 * \warning The program will quit if buffer of the \a string is \c NULL
 */
int DS_StrResize (DS_String* string, size_t size)
{
    /* Check arguments */
    assert (string);
    assert (string->buf);

    /* Initialize variables */
    int i;
    int oldSize = (int) string->len;

    /* Copy old buffer */
    char* copy = calloc (oldSize, sizeof (char));
    for (i = 0; i < oldSize; ++i)
        copy [i] = string->buf [i];

    /* Re-initialize the buffer */
    free (string->buf);
    string->buf = calloc (size, sizeof (char));

    /* Copy old buffer into start of new buffer */
    if (string->buf) {
        string->len = size;
        for (i = 0; i < oldSize; ++i)
            string->buf [i] = copy [i];

        free (copy);
        return DS_STR_SUCCESS;
    }

    /* Could not initialize the buffer, restore data */
    string->buf = copy;
    return DS_STR_FAILURE;
}

/**
 * Appends the given \a byte to the end of the given \a string
 *
 * \param string the original string
 * \param byte the value to append at the end of the string
 *
 * \warning The program will quit if \a string is \c NULL
 * \warning The program will quit if buffer of the \a string is \c NULL
 */
int DS_StrAppend (DS_String* string, const uint8_t byte)
{
    /* Check arguments */
    assert (string);
    assert (string->buf);

    /* Resize string and add extra character */
    if (DS_StrResize (string, string->len + 1)) {
        string->buf [string->len - 1] = byte;
        return DS_STR_SUCCESS;
    }

    /* String cannot be resized */
    return DS_STR_FAILURE;
}

/**
 * Appends the \a second string to the \a first string
 *
 * \param first the original string
 * \param second the string to append at the end of the \a first string
 *
 * \warning The program will quit if either \a first or \a second are \c NULL
 */
int DS_StrJoin (DS_String* first, const DS_String* second)
{
    /* Check arguments */
    assert (second);
    assert (first);
    assert (second->buf);
    assert (first->buf);

    /* Get length of initial string */
    int append_len = (int) second->len;
    int original_len = (int) first->len;

    /* Resize the string and append the other string */
    if (DS_StrResize (first, original_len + append_len)) {
        int i;
        for (i = 0; i < append_len; ++i)
            first->buf [original_len + i] = second->buf [i];

        return DS_STR_SUCCESS;
    }

    /* String cannot be resized */
    return DS_STR_FAILURE;
}

/**
 * Appends the given \a cstring to the \a string
 *
 * \returns 0 on failure, 1 on success
 */
int DS_StrJoinCStr (DS_String* string, const char* cstring)
{
    /* Check arguments */
    assert (string);
    assert (cstring);

    /* Append each character to the string */
    int i;
    int len = (int) strlen (cstring);
    for (i = 0; i < len; ++i)
        if (!DS_StrAppend (string, cstring [i]))
            return DS_STR_FAILURE;

    /* Tell everyone how smart this function is */
    return DS_STR_SUCCESS;
}

/**
 * Changes the character of the \a string at the given \a pos to \a byte
 *
 * \param string the string structure
 * \param pos the position of the byte to replace
 * \param byte the new value to write at the given position
 *
 * \warning The program will quit if \a string is \c NULL
 * \warning The program will quit if buffer of the \a string is \c NULL
 */
int DS_StrSetChar (DS_String* string, const int pos, const char byte)
{
    /* Check arguments */
    assert (string);
    assert (string->buf);

    /* Change the character at the given position */
    if (abs (pos) < (int) string->len) {
        string->buf [abs (pos)] = byte;
        return DS_STR_SUCCESS;
    }

    /* The position was invalid */
    return DS_STR_FAILURE;
}

/**
 * Creates a C string from the data buffer of the given \a string
 *
 * \warning The program will quit if \a string is \c NULL
 * \warning The program will quit if buffer of the \a string is \c NULL
 */
char* DS_StrToChar (const DS_String* string)
{
    /* Check arguments */
    assert (string);

    /* Initialize the c-string with one extra byte (for null terminator) */
    size_t len = string->len + 1;
    char* cstr = (char*) calloc (len, sizeof (char));

    /* Copy buffer data into c-string */
    int i;
    for (i = 0; i < (int) string->len; ++i)
        cstr [i] = string->buf [i];

    /* Add NULL-terminator */
    cstr [string->len] = 0;

    /* Return obtained string */
    return cstr;
}

/**
 * Returns the byte at the given position of \a string
 *
 * \param pos the position of the byte to get
 * \param string the string from which to get the byte
 *
 * \note This function will return \c '\0' if the position
 *       does not fit to the length of the given \a string
 *
 * \warning The program will quit if \a string is \c NULL
 */
char DS_StrCharAt (const DS_String* string, const int pos)
{
    /* Check arguments */
    assert (string);
    assert (string->buf);

    /* Get the character at the given position */
    if ((int) string->len > abs (pos))
        return string->buf [abs (pos)];

    /* Position invalid */
    return '\0';
}

/**
 * Returns a new string structure with a copy of the given
 * \a string
 *
 * \note This function creates a copy of the given \c string
 *       and assigns it to the buffer of the \c DS_String.
 *
 * \warning The program will quit if \a string is \c NULL
 */
DS_String DS_StrNew (const char* string)
{
    /* Check arguments */
    assert (string);

    /* Create new empty string */
    DS_String str = DS_StrNewLen (strlen (string));

    /* Copy C string data into buffer */
    int i;
    for (i = 0; i < (int) str.len; ++i)
        str.buf [i] = string [i];

    /* Return obtained string */
    return str;
}

/**
 * Returns a 0-filled string with the given \a length
 */
DS_String DS_StrNewLen (const size_t length)
{
    DS_String string;
    string.len = length;
    string.buf = (char*) calloc (string.len, sizeof (char));
    return string;
}

/**
 * Returns an independent copy of the given \a source string
 *
 * \warning The program will quit if \a source is \c NULL
 */
DS_String DS_StrDup (const DS_String* source)
{
    /* Check arguments */
    assert (source);
    assert (source->buf);

    /* Create new empty string */
    DS_String string = DS_StrNewLen (source->len);

    /* Copy each character to the new string */
    int i;
    for (i = 0; i < (int) string.len; ++i)
        string.buf [i] = source->buf [i];

    /* Return the copy */
    return string;
}

/**
 * Constructs a string with the given \a format and arguments
 *
 * Accepted formats specifiers are:
 *     - %s C String
 *     - %c character
 *     - %d signed int
 *     - %u unsigned int
 *     - %f floating point number
 *     - Any other format specifier will be ignored
 *
 * \warning The program will quit if \a format is \c NULL
 */
DS_String DS_StrFormat (const char* format, ...)
{
    /* Check arguments */
    assert (format);

    /* Initialize variables */
    int init_len = 0;
    const char* f = format;

    /* Initialize string */
    DS_String string = DS_StrNewLen (init_len);

    /* Initialize argument list */
    va_list args;
    va_start (args, format);

    /* Get next byte specifier */
    f = format;

    while (*f) {
        char next;

        /* This is a format specifier, let's do some magic */
        if (*f == '%') {
            next = * (f + 1);
            f++;

            /* Handle number values */
            if (next == 'u' || next == 'd' || next == 'f') {
                char str [sizeof (double) * 2];
                memset (str, 0, sizeof (str));

                /* Get the representation of the number */
                if (next == 'u')
                    SPRINTF_S (str, sizeof (str), "%u", (unsigned int) va_arg (args, unsigned int));
                else if (next == 'd')
                    SPRINTF_S (str, sizeof (str), "%d", (int) va_arg (args, int));
                else if (next == 'f')
                    SPRINTF_S (str, sizeof (str), "%.2f", (double) va_arg (args, double));

                /* Append every character to the string */
                int i = 0;
                int len = (int) strlen (str);
                for (i = 0; i < len; ++i)
                    DS_StrAppend (&string, str [i]);
            }

            /* Handle characters */
            else if (next == 'c')
                DS_StrAppend (&string, (char) va_arg (args, int));

            /* Handle strings */
            else if (next == 's') {
                char* str = (char*) va_arg (args, char*);
                int len = (int) strlen (str);

                /* Append every character to the string */
                int i = 0;
                for (i = 0; i < len; ++i)
                    DS_StrAppend (&string, str [i]);
            }

            /* Handle everything else */
            else
                DS_StrAppend (&string, next);
        }

        /* This is not a specifier, just append the data to the string */
        else
            DS_StrAppend (&string, *f);

        /* Go to next byte */
        f++;
    }

    /* End argument list */
    va_end (args);

    /* Return string structure */
    return string;
}

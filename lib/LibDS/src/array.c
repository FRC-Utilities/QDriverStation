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

#include "DS_Array.h"
#include "DS_Utils.h"

#include <assert.h>

/**
 * Deallocates the memory used to store the \a array data and resets the
 * properties of the given \a array
 *
 * \param array the array object to free
 */
void DS_ArrayFree (DS_Array* array)
{
    /* Check arguments */
    assert (array);
    assert (array->data);

    /* De-allocate array data */
    if (array->size > 0) {
        int i;
        for (i = 0; i < (int) array->size; ++i)
            DS_FREE (array->data [i]);

        DS_FREE (array->data);
    }

    /* Update array properties */
    array->used = 0;
    array->size = 0;
}

/**
 * Inserts the given \a element in the \a array. If there is no memory left
 * in the array, then this function will float the size of the array and
 * insert the item.
 *
 * \param array the array object in which to insert the given element
 * \param element pointer to the item to register with the array
 */
void DS_ArrayInsert (DS_Array* array, void* element)
{
    /* Check arguments */
    assert (array);
    assert (array->data);

    /* Resize array if required */
    if (array->used == array->size) {
        array->size *= 2;
        array->data = realloc (array->data, array->size);
    }

    /* Insert element */
    array->data [array->used++] = element;
}

/**
 * Initializes the data list and properties of the given \a array
 *
 * \param array the array to initialize
 * \param initial_size the initial size of the array, since the array is an
 *        array of pointers, the initial size can also be thought as the
 *        initial number of supported items
 */
void DS_ArrayInit (DS_Array* array, size_t initial_size)
{
    /* Check arguments */
    assert (array);

    /* Allocate array data */
    array->data = calloc (initial_size, sizeof (void*));

    /* Update array data */
    array->used = 0;
    array->size = initial_size;
}

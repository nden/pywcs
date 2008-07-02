/*
Copyright (C) 2008 Association of Universities for Research in Astronomy (AURA)

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above
      copyright notice, this list of conditions and the following
      disclaimer in the documentation and/or other materials provided
      with the distribution.

    3. The name of AURA and its representatives may not be used to
      endorse or promote products derived from this software without
      specific prior written permission.

THIS SOFTWARE IS PROVIDED BY AURA ``AS IS'' AND ANY EXPRESS OR IMPLIED
WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL AURA BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE
USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.
*/

/*
 Author: Michael Droettboom
         mdroe@stsci.edu
*/

#ifndef __UTIL_H__
#define __UTIL_H__

#define PY_ARRAY_UNIQUE_SYMBOL pywcs_numpy_api

#include <Python.h>
#include <numpy/arrayobject.h>
#include <wcs.h>
#include <wcsmath.h>

#include "isnan.h"
#include "str_list_proxy.h"

/* Py_ssize_t for old Pythons */
/* This code is as recommended by: */
/* http://www.python.org/dev/peps/pep-0353/#conversion-guidelines */
#if PY_VERSION_HEX < 0x02050000 && !defined(PY_SSIZE_T_MIN)
typedef int Py_ssize_t;
# define PY_SSIZE_T_MAX INT_MAX
# define PY_SSIZE_T_MIN INT_MIN
#endif

/* TODO: Remove me from public API */
PyObject*
PyArrayProxy_New(PyObject* self, int nd, const npy_intp* dims,
                 int typenum, const void* data);

static inline void
offset_c_array(double* value, size_t size, double offset) {
  double* end = value + size;

  for ( ; value != end; ++value)
    *value += offset;
}

void
offset_array(PyArrayObject* array, double value);

void
copy_array_to_c_double(PyArrayObject* array, double* dest);

void
copy_array_to_c_int(PyArrayObject* array, int* dest);

static inline
void nan2undefined(double* value, size_t nvalues) {
  double* end = value + nvalues;

  for ( ; value != end; ++value)
    if (isnan64(*value))
      *value = UNDEFINED;
}

static inline
void undefined2nan(double* value, size_t nvalues) {
  double* end = value + nvalues;
  double  v   = 0;

  for ( ; value != end; ++value) {
    v = *value;
    *value = (v == UNDEFINED) ? NAN : v;
  }
}

/**
 Returns TRUE if pointer is NULL, and sets Python exception
*/
int
is_null(void *);

void
wcsprm_c2python(struct wcsprm* x);

void
wcsprm_python2c(struct wcsprm* x);

/***************************************************************************
  Property helpers
 ***************************************************************************/
static inline int
check_delete(const char* propname, PyObject* value) {
  if (value == NULL) {
    PyErr_Format(PyExc_TypeError, "'%s' can not be deleted", propname);
    return -1;
  }

  return 0;
}

static inline PyObject*
get_string(const char* propname, const char* value) {
  return PyString_FromString(value);
}

int
set_string(const char* propname, PyObject* value,
           char* dest, Py_ssize_t maxlen);

static inline PyObject*
get_bool(const char* propname, long value) {
  return PyBool_FromLong(value);
}

int
set_bool(const char* propname, PyObject* value, int* dest);

static inline PyObject*
get_int(const char* propname, long value) {
  return PyInt_FromLong(value);
}

int
set_int(const char* propname, PyObject* value, int* dest);

static inline PyObject*
get_double(const char* propname, double value) {
  return PyFloat_FromDouble(value);
}

int
set_double(const char* propname, PyObject* value, double* dest);

static inline PyObject*
get_double_array(const char* propname, double* value,
                 npy_int ndims, const npy_intp* dims, PyObject* owner) {
  return PyArrayProxy_New(owner, ndims, dims, PyArray_DOUBLE, value);
}

int
set_double_array(const char* propname, PyObject* value, npy_int ndims,
                 const npy_intp* dims, double* dest);

static inline PyObject*
get_int_array(const char* propname, int* value,
              npy_int ndims, const npy_intp* dims, PyObject* owner) {
  return PyArrayProxy_New(owner, ndims, dims, PyArray_INT, value);
}

int
set_int_array(const char* propname, PyObject* value, npy_int ndims,
              const npy_intp* dims, int* dest);

/* Defined in str_list_proxy.h */
PyObject *
PyStrListProxy_New(PyObject* owner, Py_ssize_t size, char (*array)[72]);

static inline PyObject*
get_str_list(const char* propname, char (*array)[72], Py_ssize_t len,
             PyObject* owner) {
  return PyStrListProxy_New(owner, len, array);
}

int
set_str_list(const char* propname, PyObject* value, Py_ssize_t len,
             Py_ssize_t maxlen, char (*dest)[72]);

PyObject*
get_pscards(const char* propname, struct pscard* ps, int nps);

int
set_pscards(const char* propname, PyObject* value, struct pscard** ps,
            int *nps, int *npsmax);

PyObject*
get_pvcards(const char* propname, struct pvcard* pv, int npv);

int
set_pvcards(const char* propname, PyObject* value, struct pvcard** pv,
            int *npv, int *npvmax);

#endif /* __UTIL_H__ */
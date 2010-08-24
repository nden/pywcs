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

#define NO_IMPORT_ARRAY

#include "wcslib_units_wrap.h"

#include <wcsunits.h>

/*
 It gets to be really tedious to type long docstrings in ANSI C syntax
 (since multi-line strings literals are not valid).  Therefore, the
 docstrings are written in doc/docstrings.py, which are then converted
 by setup.py into docstrings.h, which we include here.
*/
#include "docstrings.h"

PyObject** units_errexc[13];

/***************************************************************************
 * PyTabprm methods
 */

static int
PyUnits_traverse(
    PyUnits* self, visitproc visit, void *arg) {

  return 0;
}

static int
PyUnits_clear(
    PyUnits* self) {

  return 0;
}

static void
PyUnits_dealloc(
    PyUnits* self) {

  PyUnits_clear(self);
  self->ob_type->tp_free((PyObject*)self);
}

PyUnits*
PyUnits_cnew(const double scale, const double offset, const double power) {
  PyUnits* self;
  self = (PyUnits*)(&PyUnitsType)->tp_alloc(&PyUnitsType, 0);
  self->scale = scale;
  self->offset = offset;
  self->power = power;
  return self;
}

static PyObject *
PyUnits_new(
    PyTypeObject* type,
    /*@unused@*/ PyObject* args,
    /*@unused@*/ PyObject* kwds) {

  PyUnits* self;
  self = (PyUnits*)type->tp_alloc(type, 0);
  self->scale = 1.0;
  self->offset = 0.0;
  self->power = 1.0;
  return (PyObject*)self;
}

static int
PyUnits_init(
    PyUnits* self,
    PyObject* args,
    PyObject* kwds) {

  int       status          = -1;
  char*     have;
  char*     want;
  char*     ctrl_str        = NULL;
  int       ctrl            = 0;
  const char*    keywords[] = {"have", "want", "translate_units", NULL};

  if (!PyArg_ParseTupleAndKeywords(args, kwds, "ss|s:UnitConverter.__init__",
                                   (char **)keywords, &have, &want,
                                   &ctrl_str)) {
    goto exit;
  }

  if (ctrl_str != NULL) {
    if (parse_unsafe_unit_conversion_spec(ctrl_str, &ctrl)) {
      goto exit;
    }
  }

  status = wcsutrn(ctrl, have);
  if (status) {
    goto exit;
  }

  status = wcsutrn(ctrl, want);
  if (status) {
    goto exit;
  }

  status = wcsunits(have, want, &self->scale, &self->offset, &self->power);

 exit:

  if (PyErr_Occurred()) {
    return -1;
  } else if (status == 0) {
    return 0;
  } else if (status >= 1 && status < 13) {
    PyErr_SetString((PyObject *)units_errexc[status], wcsunits_errmsg[status]);
    return -1;
  } else {
    PyErr_SetString(PyExc_RuntimeError,
                    "Unknown error occurred.  Something is seriously wrong.");
    return -1;
  }
}

/*@null@*/ static PyObject*
PyUnits___str__(
    PyUnits* self) {

    /* TODO */
  return NULL;
}

static PyObject*
PyUnits_convert(
    PyUnits* self,
    PyObject* args,
    PyObject* kwds) {

  int            status       = 1;
  PyObject*      input        = NULL;
  PyArrayObject* input_arr    = NULL;
  PyArrayObject* output_arr   = NULL;
  PyObject*      input_iter   = NULL;
  PyObject*      output_iter  = NULL;
  double         input_val;
  double         output_val;

  if (!PyArg_ParseTuple(args, "O:UnitConverter.convert", &input)) {
    goto exit;
  }

  input_arr = (PyArrayObject*)PyArray_FromObject(
      input, NPY_DOUBLE, 0, NPY_MAXDIMS);
  if (input_arr == NULL) {
    goto exit;
  }

  output_arr = (PyArrayObject*)PyArray_SimpleNew(
      PyArray_NDIM(input_arr), PyArray_DIMS(input_arr), PyArray_DOUBLE);
  if (output_arr == NULL) {
    goto exit;
  }

  input_iter = PyArray_IterNew((PyObject*)input_arr);
  if (input_iter == NULL) {
    goto exit;
  }

  output_iter = PyArray_IterNew((PyObject*)output_arr);
  if (output_iter == NULL) {
    goto exit;
  }

  if (self->power != 1.0) {
    while (PyArray_ITER_NOTDONE(input_iter)) {
      input_val = *(double *)PyArray_ITER_DATA(input_iter);
      output_val = pow(self->scale*input_val + self->offset, self->power);
      if (errno) {
        PyErr_SetFromErrno(PyExc_ValueError);
        goto exit;
      }
      *(double *)PyArray_ITER_DATA(output_iter) = output_val;
      PyArray_ITER_NEXT(input_iter);
      PyArray_ITER_NEXT(output_iter);
    }
  } else {
    while (PyArray_ITER_NOTDONE(input_iter)) {
      input_val = *(double *)PyArray_ITER_DATA(input_iter);
      output_val = self->scale*input_val + self->offset;
      *(double *)PyArray_ITER_DATA(output_iter) = output_val;
      PyArray_ITER_NEXT(input_iter);
      PyArray_ITER_NEXT(output_iter);
    }
  }

  status = 0;

 exit:

  Py_XDECREF((PyObject*)input_arr);
  Py_XDECREF(input_iter);
  Py_XDECREF(output_iter);
  if (status) {
    Py_XDECREF((PyObject*)output_arr);
    return NULL;
  }
  return (PyObject*)output_arr;
}

/***************************************************************************
 * Member getters/setters (properties)
 */

/*@null@*/ static PyObject*
PyUnits_get_scale(
    PyUnits* self,
    /*@unused@*/ void* closure) {

  return get_double("scale", self->scale);
}

/*@null@*/ static PyObject*
PyUnits_get_offset(
    PyUnits* self,
    /*@unused@*/ void* closure) {

  return get_double("offset", self->offset);
}

/*@null@*/ static PyObject*
PyUnits_get_power(
    PyUnits* self,
    /*@unused@*/ void* closure) {

  return get_double("power", self->power);
}

/***************************************************************************
 * PyUnits definition structures
 */

static PyGetSetDef PyUnits_getset[] = {
  {"scale", (getter)PyUnits_get_scale, NULL, (char *)doc_scale},
  {"offset", (getter)PyUnits_get_offset, NULL, (char *)doc_offset},
  {"power", (getter)PyUnits_get_power, NULL, (char *)doc_power},
  {NULL}
};

static PyMethodDef PyUnits_methods[] = {
  {"convert", (PyCFunction)PyUnits_convert, METH_VARARGS, doc_convert},
  {NULL}
};

PyTypeObject PyUnitsType = {
  PyObject_HEAD_INIT(NULL)
  0,                            /*ob_size*/
  "pywcs.UnitConverter",        /*tp_name*/
  sizeof(PyUnits),              /*tp_basicsize*/
  0,                            /*tp_itemsize*/
  (destructor)PyUnits_dealloc,  /*tp_dealloc*/
  0,                            /*tp_print*/
  0,                            /*tp_getattr*/
  0,                            /*tp_setattr*/
  0,                            /*tp_compare*/
  (reprfunc)PyUnits___str__,    /*tp_repr*/
  0,                            /*tp_as_number*/
  0,                            /*tp_as_sequence*/
  0,                            /*tp_as_mapping*/
  0,                            /*tp_hash */
  0,                            /*tp_call*/
  (reprfunc)PyUnits___str__,    /*tp_str*/
  0,                            /*tp_getattro*/
  0,                            /*tp_setattro*/
  0,                            /*tp_as_buffer*/
  Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE, /*tp_flags*/
  doc_UnitConverter,            /* tp_doc */
  (traverseproc)PyUnits_traverse, /* tp_traverse */
  (inquiry)PyUnits_clear,       /* tp_clear */
  0,                            /* tp_richcompare */
  0,                            /* tp_weaklistoffset */
  0,                            /* tp_iter */
  0,                            /* tp_iternext */
  PyUnits_methods,              /* tp_methods */
  0,                            /* tp_members */
  PyUnits_getset,               /* tp_getset */
  0,                            /* tp_base */
  0,                            /* tp_dict */
  0,                            /* tp_descr_get */
  0,                            /* tp_descr_set */
  0,                            /* tp_dictoffset */
  (initproc)PyUnits_init,       /* tp_init */
  0,                            /* tp_alloc */
  PyUnits_new,                  /* tp_new */
};

int
_setup_units_type(
    PyObject* m) {

  if (PyType_Ready(&PyUnitsType) < 0) {
    return -1;
  }

  Py_INCREF(&PyUnitsType);

  PyModule_AddObject(m, "UnitConverter", (PyObject *)&PyUnitsType);

  units_errexc[0]  = NULL;               /* Success */
  units_errexc[1]  = &PyExc_ValueError;  /* Invalid numeric multiplier */
  units_errexc[2]  = &PyExc_SyntaxError; /* Dangling binary operator */
  units_errexc[3]  = &PyExc_SyntaxError; /* Invalid symbol in INITIAL context */
  units_errexc[4]  = &PyExc_SyntaxError; /* Function in invalid context */
  units_errexc[5]  = &PyExc_SyntaxError; /* Invalid symbol in EXPON context */
  units_errexc[6]  = &PyExc_SyntaxError; /* Unbalanced bracket */
  units_errexc[7]  = &PyExc_SyntaxError; /* Unbalanced parenthesis */
  units_errexc[8]  = &PyExc_SyntaxError; /* Conservative binary operators */
  units_errexc[9]  = &PyExc_SyntaxError; /* Internal parser error */
  units_errexc[10] = &PyExc_SyntaxError; /* Non-conformant unit specifications */
  units_errexc[11] = &PyExc_SyntaxError; /* Non-conformant functions */
  units_errexc[12] = &PyExc_ValueError;  /* Potentially unsafe translation */

  return 0;
}
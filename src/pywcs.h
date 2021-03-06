/*
Copyright (C) 2008-2012 Association of Universities for Research in Astronomy (AURA)

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

#ifndef __PYWCS_H__
#define __PYWCS_H__

/* 
* This is necessary to avoid the wcsset() function that is in the 
* Windows libraries.  We define it here so that any other module that 
* also uses pywcs C code can also get the right behavior. 
*/ 
#ifdef _WIN32 
#define __STDC__ 1 
#endif 


/* util.h must be imported first */
#include "pyutil.h"
#include "pipeline.h"

typedef struct {
  PyObject_HEAD
  pipeline_t x;
  /*@shared@*/ PyObject*            py_det2im[2];
  /*@null@*/ /*@shared@*/ PyObject* py_sip;
  /*@shared@*/ PyObject*            py_distortion_lookup[2];
  /*@null@*/ /*@shared@*/ PyObject* py_wcsprm;
} PyWcs;

#endif /* __PYWCS_H__ */

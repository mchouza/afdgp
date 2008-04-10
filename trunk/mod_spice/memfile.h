/*
   Copyright (c) 2008, Mariano M. Chouza
   All rights reserved.
  
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
  
      * Redistributions of source code must retain the above copyright notice, 
        this list of conditions and the following disclaimer.
  
      * Redistributions in binary form must reproduce the above copyright
        notice, this list of conditions and the following disclaimer in the
        documentation and/or other materials provided with the distribution.
  
      * The names of the contributors may not be used to endorse or promote
        products derived from this software without specific prior written
        permission.
  
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
*/

/*****************************************************************************\
*  memfile.h
*------------------------------------------------------------------------------
*  Creado por Mariano M. Chouza | Agregado a AFDGP el 10 de abril de 2008
\*****************************************************************************/

#ifndef MEMFILE_H
#define MEMFILE_H

#include <stdlib.h>

/* Es la struct que representa un "archivo" en memoria (MEMFILE) */
typedef struct MEMFILE_T
{
	/* El buffer */
	unsigned char* buffer;
	/* La cantidad de elemntos válidos que tiene */
	size_t cant_elem;
	/* La cantidad de elementos reservados que tiene */
	size_t cant_reserved;
	/* Cursor (apunta al próximo lugar a leer o escribir) */
	size_t pos;
	/* El nombre */
	unsigned char* name;
	/* Está abierto? */
	int is_open;
} MEMFILE;

/* Abre un MEMFILE. No necesito nombre ni modo. */
MEMFILE* memfopen(const char* name, const char* mode);

/* Cierra un MEMFILE */
void memfclose(MEMFILE* mfp);

/* Escribe en un MEMFILE */
size_t memfwrite(const void* buffer, size_t size, size_t count,
				 MEMFILE* mfp);

/* Lee desde un MEMFILE */
size_t memfread(void* buffer, size_t size, size_t count, MEMFILE* mfp);

/* Escribe una cadena en un MEMFILE */
int memfputs(const char* str, MEMFILE* mfp);

/* Escribe con formato en un MEMFILE */
int memfprintf(MEMFILE* mfp, const char* fmt, ...);

/* Hace seek en MEMFILE */
int memfseek(MEMFILE* mfp, long offset, int base);

/* Devuelve la posición en un MEMFILE */
size_t memftell(MEMFILE* mfp);

/* Limpia todos los MEMFILEs creados */
void memfgencleanup(void);

/* Limpia un MEMFILE en particular */
void memfcleanup(const char* name);

/* Obtiene el tamaño de los datos */
size_t memfgetsize(MEMFILE* mfp);

#endif

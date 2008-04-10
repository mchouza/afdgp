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
*  memfile.c
*------------------------------------------------------------------------------
*  Creado por Mariano M. Chouza | Agregado a AFDGP el 10 de abril de 2008
\*****************************************************************************/

#include "memfile.h"
#include <assert.h>
#include <memory.h>
#include <stdio.h>
#include <stdarg.h>

#define MAX_MEMFILES 10
#define BUFFER_SIZE 1024

static MEMFILE* mftable[MAX_MEMFILES];

static unsigned char buffer[BUFFER_SIZE];

/* El modo de apertua es ignorado */
MEMFILE* memfopen(const char* name, const char* mode)
{
	/* i */
	int i;
	
	/* Se fija si está */
	for (i = 0; i < MAX_MEMFILES; i++)
	{
		if (mftable[i] && !strcmp(mftable[i]->name, name))
		{
			if (mftable[i]->is_open)
			{
				return NULL; /* Ya está abierto */
			}
			else
			{
				mftable[i]->is_open = 1;
				return mftable[i];
			}
		}
	}

	/* No lo encontró, busca un slot vacío */
	for (i = 0; i < MAX_MEMFILES; i++)
		if (!mftable[i])
			break;

	/* Si no hay ninguno, salgo */
	if (i == MAX_MEMFILES)
		return NULL;
	
	/* Reservo la memoria */
	mftable[i] = (MEMFILE*)malloc(sizeof(MEMFILE));

	/* Empieza sin memoria reservada ni elementos */
	mftable[i]->cant_reserved = mftable[i]->cant_elem = 0;

	/* Empieza sin datos */
	mftable[i]->buffer = NULL;

	/* El nombre es el que me pidieron */
	mftable[i]->name = malloc(strlen(name) + 1);
	strcpy(mftable[i]->name, name);

	/* Posición 0 */
	mftable[i]->pos = 0;

	/* Devuelvo el MEMFILE */
	return mftable[i];
}

void memfclose(MEMFILE* mfp)
{
	mfp->pos = 0;
	mfp->is_open = 0;
}

size_t memfread(void* buffer, size_t size, size_t count, MEMFILE* mfp)
{
	/* Cantidad de elementos leídos que debe retornarse */
	size_t ret;
	/* Cursor en el buffer */
	unsigned char* p;

	/* Empiezo desde el principio */
	p = (unsigned char*)buffer;

	/* Para cada elemento a leer */
	for (ret = 0; ret < count; ret++)
	{
		/* Me fijo si me pasaría al leer este elemento */
		if (mfp->pos + size > mfp->cant_elem)
		{
			/* Si, corto acá */
			break;
		}
		{
			/* No, lo leo y continúo */
			memcpy(p, mfp->buffer + mfp->pos, size);
			p += size;
			mfp->pos += size;
		}
	}

	/* Devuelvo la cantidad de elementos leída */
	return ret;
}

size_t memfwrite(const void* buffer, size_t size, size_t count, MEMFILE* mfp)
{
	/* FIXME: Supongo que nunca falla, o sea que siempre tiene memoria */

	/* Me fijo si no tengo suficiente memoria */
	if (mfp->pos + size * count >= mfp->cant_reserved)
	{
		/* No, reservo más */
		mfp->cant_reserved = mfp->pos + size * count;
		mfp->buffer = realloc(mfp->buffer, mfp->cant_reserved);
		assert(mfp->buffer);
	}

	/* Realizo la copia */
	memcpy(mfp->buffer + mfp->pos, buffer, size * count);
	mfp->pos += size * count;
	mfp->cant_elem = (mfp->pos >= mfp->cant_elem) ? mfp->pos : mfp->cant_elem;

	/* FIXME: Como asumo que nunca falta memoria, siempre escribe todo */
	return count;
}

int memfseek(MEMFILE* mfp, long offset, int base)
{
	/* Posición deseada */
	long new_pos;
	
	/* Elijo según el caso */
	switch (base)
	{
	case SEEK_SET:
		new_pos = offset;
		break;
	case SEEK_CUR:
		new_pos = (long)mfp->pos + offset;
		break;
	case SEEK_END:
		new_pos = (long)mfp->cant_elem + offset;
	default:
		return 0;
	}

	/* Reviso que la posición deseada no sea negativa */
	if (new_pos < 0)
		return 0;

	/* Tuvo éxito */
	return 1;
}

void memfgencleanup(void)
{
	/* i */
	int i;

	/* Recorro la tabla */
	for (i = 0; i < MAX_MEMFILES; i++)
	{
		/* Si el ítem no tiene nada, sigo con el próximo */
		if (!mftable[i])
			continue;

		/* Limpio sistemáticamente */

		/* Libero el buffer y el nombre */
		free(mftable[i]->buffer);
		free(mftable[i]->name);

		/* Libero la estructura */
		free(mftable[i]); 

		/* Marco que no hay nada */
		mftable[i] = NULL;
	}
}

void memfcleanup(const char* name)
{
	/* i */
	int i;

	/* Recorro la tabla buscando el archivo a limpiar */
	for (i = 0; i < MAX_MEMFILES; i++)
	{
		/* Si el ítem no tiene nada, sigo con el próximo */
		if (!mftable[i])
			continue;

		/* Si no es este, sigo con el próximo */
		if (strcmp(mftable[i]->name, name))
			continue;

		/* Limpio sistemáticamente */

		/* Libero el buffer y el nombre */
		free(mftable[i]->buffer);
		free(mftable[i]->name);

		/* Libero la estructura */
		free(mftable[i]); 

		/* Marco que no hay nada */
		mftable[i] = NULL;

		/* Listo */
		return;
	}
}

int memfputs(const char* str, MEMFILE* mfp)
{
	/* Tamaño de la string */
	size_t str_size;

	/* Obtengo el tamaño de la string */
	str_size = strlen(str);

	/* Escribo los datos */
	return (int)memfwrite(str, str_size, 1, mfp);
}

int memfprintf(MEMFILE* mfp, const char* fmt, ...)
{
	/* Valor de retorno */
	int ret;
	/* Lista de argumentos */
	va_list vl;
	
	/* Empiezo */
	va_start(vl, fmt);

	/* Se la paso a vsprintf para hacer el trabajo en el buffer */
	ret = vsprintf(buffer, fmt, vl);

	/* Lo grabo en el MEMFILE */
	memfputs(buffer, mfp);

	/* Termino */
	va_end(vl);

	/* Devuelvo el valor de retorno */
	return ret;
}

size_t memftell(MEMFILE* mfp)
{
	return mfp->pos;
}

size_t memfgetsize(MEMFILE* mfp)
{
	return mfp->cant_elem;
}

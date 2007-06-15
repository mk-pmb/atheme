/*
 * atheme-services: A collection of minimalist IRC services   
 * linker.c: Abstraction of the dynamic linking system.
 *
 * Copyright (c) 2005-2007 Atheme Project (http://www.atheme.org)           
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "atheme.h"

#include <dlfcn.h>

#ifdef __OpenBSD__
# define RTLD_NOW RTLD_LAZY
#endif

#ifndef __HPUX__
# define PLATFORM_SUFFIX ".so"
#else
# define PLATFORM_SUFFIX ".sl"
#endif

/*
 * linker_open()
 *
 * Inputs:
 *       path to file to open
 *
 * Outputs:
 *       linker handle.
 *
 * Side Effects:
 *       a shared module is loaded into the application's memory space
 */
void *linker_open(char *path)
{
	return dlopen(path, RTLD_NOW);
}

/*
 * linker_open_ext()
 *
 * Inputs:
 *       path to file to open
 *
 * Outputs:
 *       linker handle
 *
 * Side Effects:
 *       the extension is appended if it's not already there.
 *       a shared module is loaded into the application's memory space
 */
void *linker_open_ext(char *path)
{
	char *buf = smalloc(strlen(path) + 20);
	
	strlcpy(buf, path, strlen(path) + 20);

	if (!strstr(buf, PLATFORM_SUFFIX))
		strlcat(buf, PLATFORM_SUFFIX, strlen(path) + 20);

	return linker_open(buf);
}

/*
 * linker_getsym()
 *
 * Inputs:
 *       linker handle, symbol to retrieve
 *
 * Outputs:
 *       pointer to symbol, or NULL if no symbol.
 *
 * Side Effects:
 *       none
 */
void *linker_getsym(void *vptr, char *sym)
{
	return dlsym(vptr, sym);
}

/*
 * linker_close()
 *
 * Inputs:
 *       linker handle
 *
 * Outputs:
 *       none
 *
 * Side Effects:
 *       the module is unloaded from the linker
 */
void linker_close(void *vptr)
{
	dlclose(vptr);
}

/* vim:cinoptions=>s,e0,n0,f0,{0,}0,^0,=s,ps,t0,c3,+s,(2s,us,)20,*30,gs,hs
 * vim:ts=8
 * vim:sw=8
 * vim:noexpandtab
 */

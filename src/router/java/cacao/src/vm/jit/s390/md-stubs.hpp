/* src/vm/jit/s390/md-stubs.hpp - s390 JIT stubs

   Copyright (C) 2008 Theobroma Systems Ltd.

   This file is part of CACAO.

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2, or (at
   your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
   02110-1301, USA.

*/


#ifndef _MD_STUBS_HPP
#define _MD_STUBS_HPP

#include "config.h"

#include "vm/jit/s390/codegen.h"


/**
 * Return the code size of a compiler on a s390 architecture.
 *
 * @return Code size in bytes.
 */
int CompilerStub::get_code_size()
{
	return (SZ_AHI + SZ_L + SZ_L + SZ_BCR);
}

#endif // _MD_STUBS_HPP


/*
 * These are local overrides for various environment variables in Emacs.
 * Please do not remove this and leave it at the end of the file, where
 * Emacs will automagically detect them.
 * ---------------------------------------------------------------------
 * Local variables:
 * mode: c++
 * indent-tabs-mode: t
 * c-basic-offset: 4
 * tab-width: 4
 * End:
 * vim:noexpandtab:sw=4:ts=4:
 */

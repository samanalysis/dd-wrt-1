/* 
   Unix SMB/CIFS implementation.

   routines for marshalling/unmarshalling basic types

   Copyright (C) Andrew Tridgell 2003
   
   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "includes.h"
#include "system/network.h"
#include "librpc/ndr/libndr.h"

#define NDR_SVAL(ndr, ofs) (NDR_BE(ndr)?RSVAL(ndr->data,ofs):SVAL(ndr->data,ofs))
#define NDR_IVAL(ndr, ofs) (NDR_BE(ndr)?RIVAL(ndr->data,ofs):IVAL(ndr->data,ofs))
#define NDR_IVALS(ndr, ofs) (NDR_BE(ndr)?RIVALS(ndr->data,ofs):IVALS(ndr->data,ofs))
#define NDR_SSVAL(ndr, ofs, v) do { if (NDR_BE(ndr))  { RSSVAL(ndr->data,ofs,v); } else SSVAL(ndr->data,ofs,v); } while (0)
#define NDR_SIVAL(ndr, ofs, v) do { if (NDR_BE(ndr))  { RSIVAL(ndr->data,ofs,v); } else SIVAL(ndr->data,ofs,v); } while (0)
#define NDR_SIVALS(ndr, ofs, v) do { if (NDR_BE(ndr))  { RSIVALS(ndr->data,ofs,v); } else SIVALS(ndr->data,ofs,v); } while (0)


/*
  check for data leaks from the server by looking for non-zero pad bytes
  these could also indicate that real structure elements have been
  mistaken for padding in the IDL
*/
_PUBLIC_ void ndr_check_padding(struct ndr_pull *ndr, size_t n)
{
	size_t ofs2 = (ndr->offset + (n-1)) & ~(n-1);
	int i;
	for (i=ndr->offset;i<ofs2;i++) {
		if (ndr->data[i] != 0) {
			break;
		}
	}
	if (i<ofs2) {
		DEBUG(0,("WARNING: Non-zero padding to %d: ", (int)n));
		for (i=ndr->offset;i<ofs2;i++) {
			DEBUG(0,("%02x ", ndr->data[i]));
		}
		DEBUG(0,("\n"));
	}

}

/*
  parse a int8_t
*/
_PUBLIC_ enum ndr_err_code ndr_pull_int8(struct ndr_pull *ndr, int ndr_flags, int8_t *v)
{
	NDR_PULL_NEED_BYTES(ndr, 1);
	*v = (int8_t)CVAL(ndr->data, ndr->offset);
	ndr->offset += 1;
	return NDR_ERR_SUCCESS;
}

/*
  parse a uint8_t
*/
_PUBLIC_ enum ndr_err_code ndr_pull_uint8(struct ndr_pull *ndr, int ndr_flags, uint8_t *v)
{
	NDR_PULL_NEED_BYTES(ndr, 1);
	*v = CVAL(ndr->data, ndr->offset);
	ndr->offset += 1;
	return NDR_ERR_SUCCESS;
}

/*
  parse a int16_t
*/
_PUBLIC_ enum ndr_err_code ndr_pull_int16(struct ndr_pull *ndr, int ndr_flags, int16_t *v)
{
	NDR_PULL_ALIGN(ndr, 2);
	NDR_PULL_NEED_BYTES(ndr, 2);
	*v = (uint16_t)NDR_SVAL(ndr, ndr->offset);
	ndr->offset += 2;
	return NDR_ERR_SUCCESS;
}

/*
  parse a uint16_t
*/
_PUBLIC_ enum ndr_err_code ndr_pull_uint16(struct ndr_pull *ndr, int ndr_flags, uint16_t *v)
{
	NDR_PULL_ALIGN(ndr, 2);
	NDR_PULL_NEED_BYTES(ndr, 2);
	*v = NDR_SVAL(ndr, ndr->offset);
	ndr->offset += 2;
	return NDR_ERR_SUCCESS;
}

/*
  parse a int32_t
*/
_PUBLIC_ enum ndr_err_code ndr_pull_int32(struct ndr_pull *ndr, int ndr_flags, int32_t *v)
{
	NDR_PULL_ALIGN(ndr, 4);
	NDR_PULL_NEED_BYTES(ndr, 4);
	*v = NDR_IVALS(ndr, ndr->offset);
	ndr->offset += 4;
	return NDR_ERR_SUCCESS;
}

/*
  parse a uint32_t
*/
_PUBLIC_ enum ndr_err_code ndr_pull_uint32(struct ndr_pull *ndr, int ndr_flags, uint32_t *v)
{
	NDR_PULL_ALIGN(ndr, 4);
	NDR_PULL_NEED_BYTES(ndr, 4);
	*v = NDR_IVAL(ndr, ndr->offset);
	ndr->offset += 4;
	return NDR_ERR_SUCCESS;
}

/*
  parse a pointer referent identifier
*/
_PUBLIC_ enum ndr_err_code ndr_pull_generic_ptr(struct ndr_pull *ndr, uint32_t *v)
{
	NDR_CHECK(ndr_pull_uint32(ndr, NDR_SCALARS, v));
	if (*v != 0) {
		ndr->ptr_count++;
	}
	return NDR_ERR_SUCCESS;
}

/*
  parse a ref pointer referent identifier
*/
_PUBLIC_ enum ndr_err_code ndr_pull_ref_ptr(struct ndr_pull *ndr, uint32_t *v)
{
	NDR_CHECK(ndr_pull_uint32(ndr, NDR_SCALARS, v));
	/* ref pointers always point to data */
	*v = 1;
	return NDR_ERR_SUCCESS;
}

/*
  parse a udlong
*/
_PUBLIC_ enum ndr_err_code ndr_pull_udlong(struct ndr_pull *ndr, int ndr_flags, uint64_t *v)
{
	NDR_PULL_ALIGN(ndr, 4);
	NDR_PULL_NEED_BYTES(ndr, 8);
	*v = NDR_IVAL(ndr, ndr->offset);
	*v |= (uint64_t)(NDR_IVAL(ndr, ndr->offset+4)) << 32;
	ndr->offset += 8;
	return NDR_ERR_SUCCESS;
}

/*
  parse a udlongr
*/
_PUBLIC_ enum ndr_err_code ndr_pull_udlongr(struct ndr_pull *ndr, int ndr_flags, uint64_t *v)
{
	NDR_PULL_ALIGN(ndr, 4);
	NDR_PULL_NEED_BYTES(ndr, 8);
	*v = ((uint64_t)NDR_IVAL(ndr, ndr->offset)) << 32;
	*v |= NDR_IVAL(ndr, ndr->offset+4);
	ndr->offset += 8;
	return NDR_ERR_SUCCESS;
}

/*
  parse a dlong
*/
_PUBLIC_ enum ndr_err_code ndr_pull_dlong(struct ndr_pull *ndr, int ndr_flags, int64_t *v)
{
	return ndr_pull_udlong(ndr, ndr_flags, (uint64_t *)v);
}

/*
  parse a hyper
*/
_PUBLIC_ enum ndr_err_code ndr_pull_hyper(struct ndr_pull *ndr, int ndr_flags, uint64_t *v)
{
	NDR_PULL_ALIGN(ndr, 8);
	return ndr_pull_udlong(ndr, ndr_flags, v);
}

/*
  parse a pointer
*/
_PUBLIC_ enum ndr_err_code ndr_pull_pointer(struct ndr_pull *ndr, int ndr_flags, void* *v)
{
	uintptr_t h;
	NDR_PULL_ALIGN(ndr, sizeof(h));
	NDR_PULL_NEED_BYTES(ndr, sizeof(h));
	memcpy(&h, ndr->data+ndr->offset, sizeof(h));
	ndr->offset += sizeof(h);
	*v = (void *)h;
	return NDR_ERR_SUCCESS;
}

/*
  pull a NTSTATUS
*/
_PUBLIC_ enum ndr_err_code ndr_pull_NTSTATUS(struct ndr_pull *ndr, int ndr_flags, NTSTATUS *status)
{
	uint32_t v;
	NDR_CHECK(ndr_pull_uint32(ndr, NDR_SCALARS, &v));
	*status = NT_STATUS(v);
	return NDR_ERR_SUCCESS;
}

/*
  push a NTSTATUS
*/
_PUBLIC_ enum ndr_err_code ndr_push_NTSTATUS(struct ndr_push *ndr, int ndr_flags, NTSTATUS status)
{
	return ndr_push_uint32(ndr, ndr_flags, NT_STATUS_V(status));
}

_PUBLIC_ void ndr_print_NTSTATUS(struct ndr_print *ndr, const char *name, NTSTATUS r)
{
	ndr->print(ndr, "%-25s: %s", name, nt_errstr(r));
}

/*
  pull a WERROR
*/
_PUBLIC_ enum ndr_err_code ndr_pull_WERROR(struct ndr_pull *ndr, int ndr_flags, WERROR *status)
{
	uint32_t v;
	NDR_CHECK(ndr_pull_uint32(ndr, NDR_SCALARS, &v));
	*status = W_ERROR(v);
	return NDR_ERR_SUCCESS;
}

/*
  push a WERROR
*/
_PUBLIC_ enum ndr_err_code ndr_push_WERROR(struct ndr_push *ndr, int ndr_flags, WERROR status)
{
	return ndr_push_uint32(ndr, NDR_SCALARS, W_ERROR_V(status));
}

_PUBLIC_ void ndr_print_WERROR(struct ndr_print *ndr, const char *name, WERROR r)
{
#ifdef AVM_VERY_SMALL
	AVM_VERY_SMALL_LOG
#else
	ndr->print(ndr, "%-25s: %s", name, win_errstr(r));
#endif
}

/*
  parse a set of bytes
*/
_PUBLIC_ enum ndr_err_code ndr_pull_bytes(struct ndr_pull *ndr, uint8_t *data, uint32_t n)
{
	NDR_PULL_NEED_BYTES(ndr, n);
	memcpy(data, ndr->data + ndr->offset, n);
	ndr->offset += n;
	return NDR_ERR_SUCCESS;
}

/*
  pull an array of uint8
*/
_PUBLIC_ enum ndr_err_code ndr_pull_array_uint8(struct ndr_pull *ndr, int ndr_flags, uint8_t *data, uint32_t n)
{
	if (!(ndr_flags & NDR_SCALARS)) {
		return NDR_ERR_SUCCESS;
	}
	return ndr_pull_bytes(ndr, data, n);
}

/*
  push a int8_t
*/
_PUBLIC_ enum ndr_err_code ndr_push_int8(struct ndr_push *ndr, int ndr_flags, int8_t v)
{
	NDR_PUSH_NEED_BYTES(ndr, 1);
	SCVAL(ndr->data, ndr->offset, (uint8_t)v);
	ndr->offset += 1;
	return NDR_ERR_SUCCESS;
}

/*
  push a uint8_t
*/
_PUBLIC_ enum ndr_err_code ndr_push_uint8(struct ndr_push *ndr, int ndr_flags, uint8_t v)
{
	NDR_PUSH_NEED_BYTES(ndr, 1);
	SCVAL(ndr->data, ndr->offset, v);
	ndr->offset += 1;
	return NDR_ERR_SUCCESS;
}

/*
  push a int16_t
*/
_PUBLIC_ enum ndr_err_code ndr_push_int16(struct ndr_push *ndr, int ndr_flags, int16_t v)
{
	NDR_PUSH_ALIGN(ndr, 2);
	NDR_PUSH_NEED_BYTES(ndr, 2);
	NDR_SSVAL(ndr, ndr->offset, (uint16_t)v);
	ndr->offset += 2;
	return NDR_ERR_SUCCESS;
}

/*
  push a uint16_t
*/
_PUBLIC_ enum ndr_err_code ndr_push_uint16(struct ndr_push *ndr, int ndr_flags, uint16_t v)
{
	NDR_PUSH_ALIGN(ndr, 2);
	NDR_PUSH_NEED_BYTES(ndr, 2);
	NDR_SSVAL(ndr, ndr->offset, v);
	ndr->offset += 2;
	return NDR_ERR_SUCCESS;
}

/*
  push a int32_t
*/
_PUBLIC_ enum ndr_err_code ndr_push_int32(struct ndr_push *ndr, int ndr_flags, int32_t v)
{
	NDR_PUSH_ALIGN(ndr, 4);
	NDR_PUSH_NEED_BYTES(ndr, 4);
	NDR_SIVALS(ndr, ndr->offset, v);
	ndr->offset += 4;
	return NDR_ERR_SUCCESS;
}

/*
  push a uint32_t
*/
_PUBLIC_ enum ndr_err_code ndr_push_uint32(struct ndr_push *ndr, int ndr_flags, uint32_t v)
{
	NDR_PUSH_ALIGN(ndr, 4);
	NDR_PUSH_NEED_BYTES(ndr, 4);
	NDR_SIVAL(ndr, ndr->offset, v);
	ndr->offset += 4;
	return NDR_ERR_SUCCESS;
}

/*
  push a udlong
*/
_PUBLIC_ enum ndr_err_code ndr_push_udlong(struct ndr_push *ndr, int ndr_flags, uint64_t v)
{
	NDR_PUSH_ALIGN(ndr, 4);
	NDR_PUSH_NEED_BYTES(ndr, 8);
	NDR_SIVAL(ndr, ndr->offset, (v & 0xFFFFFFFF));
	NDR_SIVAL(ndr, ndr->offset+4, (v>>32));
	ndr->offset += 8;
	return NDR_ERR_SUCCESS;
}

/*
  push a udlongr
*/
_PUBLIC_ enum ndr_err_code ndr_push_udlongr(struct ndr_push *ndr, int ndr_flags, uint64_t v)
{
	NDR_PUSH_ALIGN(ndr, 4);
	NDR_PUSH_NEED_BYTES(ndr, 8);
	NDR_SIVAL(ndr, ndr->offset, (v>>32));
	NDR_SIVAL(ndr, ndr->offset+4, (v & 0xFFFFFFFF));
	ndr->offset += 8;
	return NDR_ERR_SUCCESS;
}

/*
  push a dlong
*/
_PUBLIC_ enum ndr_err_code ndr_push_dlong(struct ndr_push *ndr, int ndr_flags, int64_t v)
{
	return ndr_push_udlong(ndr, NDR_SCALARS, (uint64_t)v);
}

/*
  push a hyper
*/
_PUBLIC_ enum ndr_err_code ndr_push_hyper(struct ndr_push *ndr, int ndr_flags, uint64_t v)
{
	NDR_PUSH_ALIGN(ndr, 8);
	return ndr_push_udlong(ndr, NDR_SCALARS, v);
}

/*
  push a pointer
*/
_PUBLIC_ enum ndr_err_code ndr_push_pointer(struct ndr_push *ndr, int ndr_flags, void* v)
{
	uintptr_t h = (intptr_t)v;
	NDR_PUSH_ALIGN(ndr, sizeof(h));
	NDR_PUSH_NEED_BYTES(ndr, sizeof(h));
	memcpy(ndr->data+ndr->offset, &h, sizeof(h));
	ndr->offset += sizeof(h);
	return NDR_ERR_SUCCESS;
}

_PUBLIC_ enum ndr_err_code ndr_push_align(struct ndr_push *ndr, size_t size)
{
	NDR_PUSH_ALIGN(ndr, size);
	return NDR_ERR_SUCCESS;
}

_PUBLIC_ enum ndr_err_code ndr_pull_align(struct ndr_pull *ndr, size_t size)
{
	NDR_PULL_ALIGN(ndr, size);
	return NDR_ERR_SUCCESS;
}

/*
  push some bytes
*/
_PUBLIC_ enum ndr_err_code ndr_push_bytes(struct ndr_push *ndr, const uint8_t *data, uint32_t n)
{
	NDR_PUSH_NEED_BYTES(ndr, n);
	memcpy(ndr->data + ndr->offset, data, n);
	ndr->offset += n;
	return NDR_ERR_SUCCESS;
}

/*
  push some zero bytes
*/
_PUBLIC_ enum ndr_err_code ndr_push_zero(struct ndr_push *ndr, uint32_t n)
{
	NDR_PUSH_NEED_BYTES(ndr, n);
	memset(ndr->data + ndr->offset, 0, n);
	ndr->offset += n;
	return NDR_ERR_SUCCESS;
}

/*
  push an array of uint8
*/
_PUBLIC_ enum ndr_err_code ndr_push_array_uint8(struct ndr_push *ndr, int ndr_flags, const uint8_t *data, uint32_t n)
{
	if (!(ndr_flags & NDR_SCALARS)) {
		return NDR_ERR_SUCCESS;
	}
	return ndr_push_bytes(ndr, data, n);
}

/*
  save the current position
 */
_PUBLIC_ void ndr_push_save(struct ndr_push *ndr, struct ndr_push_save *save)
{
	save->offset = ndr->offset;
}

/*
  restore the position
 */
_PUBLIC_ void ndr_push_restore(struct ndr_push *ndr, struct ndr_push_save *save)
{
	ndr->offset = save->offset;
}

/*
  push a unique non-zero value if a pointer is non-NULL, otherwise 0
*/
_PUBLIC_ enum ndr_err_code ndr_push_unique_ptr(struct ndr_push *ndr, const void *p)
{
	uint32_t ptr = 0;
	if (p) {
		ptr = ndr->ptr_count * 4;
		ptr |= 0x00020000;
		ndr->ptr_count++;
	}
	return ndr_push_uint32(ndr, NDR_SCALARS, ptr);
}

/*
  push a 'simple' full non-zero value if a pointer is non-NULL, otherwise 0
*/
_PUBLIC_ enum ndr_err_code ndr_push_full_ptr(struct ndr_push *ndr, const void *p)
{
	uint32_t ptr = 0;
	if (p) {
		/* Check if the pointer already exists and has an id */
		ptr = ndr_token_peek(&ndr->full_ptr_list, p);
		if (ptr == 0) {
			ndr->ptr_count++;
			ptr = ndr->ptr_count;
			ndr_token_store(ndr, &ndr->full_ptr_list, p, ptr);
		}
	}
	return ndr_push_uint32(ndr, NDR_SCALARS, ptr);
}

/*
  push always a 0, if a pointer is NULL it's a fatal error
*/
_PUBLIC_ enum ndr_err_code ndr_push_ref_ptr(struct ndr_push *ndr)
{
	return ndr_push_uint32(ndr, NDR_SCALARS, 0xAEF1AEF1);
}


/*
  push a NTTIME
*/
_PUBLIC_ enum ndr_err_code ndr_push_NTTIME(struct ndr_push *ndr, int ndr_flags, NTTIME t)
{
	NDR_CHECK(ndr_push_udlong(ndr, ndr_flags, t));
	return NDR_ERR_SUCCESS;
}

/*
  pull a NTTIME
*/
_PUBLIC_ enum ndr_err_code ndr_pull_NTTIME(struct ndr_pull *ndr, int ndr_flags, NTTIME *t)
{
	NDR_CHECK(ndr_pull_udlong(ndr, ndr_flags, t));
	return NDR_ERR_SUCCESS;
}

/*
  push a NTTIME
*/
_PUBLIC_ enum ndr_err_code ndr_push_NTTIME_1sec(struct ndr_push *ndr, int ndr_flags, NTTIME t)
{
	t /= 10000000;
	NDR_CHECK(ndr_push_hyper(ndr, ndr_flags, t));
	return NDR_ERR_SUCCESS;
}

/*
  pull a NTTIME_1sec
*/
_PUBLIC_ enum ndr_err_code ndr_pull_NTTIME_1sec(struct ndr_pull *ndr, int ndr_flags, NTTIME *t)
{
	NDR_CHECK(ndr_pull_hyper(ndr, ndr_flags, t));
	(*t) *= 10000000;
	return NDR_ERR_SUCCESS;
}

/*
  pull a NTTIME_hyper
*/
_PUBLIC_ enum ndr_err_code ndr_pull_NTTIME_hyper(struct ndr_pull *ndr, int ndr_flags, NTTIME *t)
{
	NDR_CHECK(ndr_pull_hyper(ndr, ndr_flags, t));
	return NDR_ERR_SUCCESS;
}

/*
  push a NTTIME_hyper
*/
_PUBLIC_ enum ndr_err_code ndr_push_NTTIME_hyper(struct ndr_push *ndr, int ndr_flags, NTTIME t)
{
	NDR_CHECK(ndr_push_hyper(ndr, ndr_flags, t));
	return NDR_ERR_SUCCESS;
}

/*
  push a time_t
*/
_PUBLIC_ enum ndr_err_code ndr_push_time_t(struct ndr_push *ndr, int ndr_flags, time_t t)
{
	return ndr_push_uint32(ndr, ndr_flags, t);
}

/*
  pull a time_t
*/
_PUBLIC_ enum ndr_err_code ndr_pull_time_t(struct ndr_pull *ndr, int ndr_flags, time_t *t)
{
	uint32_t tt;
	NDR_CHECK(ndr_pull_uint32(ndr, ndr_flags, &tt));
	*t = tt;
	return NDR_ERR_SUCCESS;
}


/*
  pull a ipv4address
*/
_PUBLIC_ enum ndr_err_code ndr_pull_ipv4address(struct ndr_pull *ndr, int ndr_flags, const char **address)
{
	struct in_addr in;
	NDR_CHECK(ndr_pull_uint32(ndr, ndr_flags, &in.s_addr));
	in.s_addr = htonl(in.s_addr);
	*address = talloc_strdup(ndr->current_mem_ctx, inet_ntoa(in));
	NDR_ERR_HAVE_NO_MEMORY(*address);
	return NDR_ERR_SUCCESS;
}

/*
  push a ipv4address
*/
_PUBLIC_ enum ndr_err_code ndr_push_ipv4address(struct ndr_push *ndr, int ndr_flags, const char *address)
{
	uint32_t addr;
	if (!is_ipaddress(address)) {
		return ndr_push_error(ndr, NDR_ERR_IPV4ADDRESS,
				      "Invalid IPv4 address: '%s'", 
				      address);
	}
	addr = inet_addr(address);
	NDR_CHECK(ndr_push_uint32(ndr, ndr_flags, htonl(addr)));
	return NDR_ERR_SUCCESS;
}

/*
  print a ipv4address
*/
_PUBLIC_ void ndr_print_ipv4address(struct ndr_print *ndr, const char *name, 
			   const char *address)
{
	ndr->print(ndr, "%-25s: %s", name, address);
}


_PUBLIC_ void ndr_print_struct(struct ndr_print *ndr, const char *name, const char *type)
{
	ndr->print(ndr, "%s: struct %s", name, type);
}

_PUBLIC_ void ndr_print_enum(struct ndr_print *ndr, const char *name, const char *type, 
		    const char *val, uint32_t value)
{
	if (ndr->flags & LIBNDR_PRINT_ARRAY_HEX) {
		ndr->print(ndr, "%-25s: %s (0x%X)", name, val?val:"UNKNOWN_ENUM_VALUE", value);
	} else {
		ndr->print(ndr, "%-25s: %s (%d)", name, val?val:"UNKNOWN_ENUM_VALUE", value);
	}
}

_PUBLIC_ void ndr_print_bitmap_flag(struct ndr_print *ndr, size_t size, const char *flag_name, uint32_t flag, uint32_t value)
{
	/* this is an attempt to support multi-bit bitmap masks */
	value &= flag;

	while (!(flag & 1)) {
		flag >>= 1;
		value >>= 1;
	}	
	if (flag == 1) {
		ndr->print(ndr, "   %d: %-25s", value, flag_name);
	} else {
		ndr->print(ndr, "0x%02x: %-25s (%d)", value, flag_name, value);
	}
}

_PUBLIC_ void ndr_print_int8(struct ndr_print *ndr, const char *name, int8_t v)
{
	ndr->print(ndr, "%-25s: %d", name, v);
}

_PUBLIC_ void ndr_print_uint8(struct ndr_print *ndr, const char *name, uint8_t v)
{
	ndr->print(ndr, "%-25s: 0x%02x (%u)", name, v, v);
}

_PUBLIC_ void ndr_print_int16(struct ndr_print *ndr, const char *name, int16_t v)
{
	ndr->print(ndr, "%-25s: %d", name, v);
}

_PUBLIC_ void ndr_print_uint16(struct ndr_print *ndr, const char *name, uint16_t v)
{
	ndr->print(ndr, "%-25s: 0x%04x (%u)", name, v, v);
}

_PUBLIC_ void ndr_print_int32(struct ndr_print *ndr, const char *name, int32_t v)
{
	ndr->print(ndr, "%-25s: %d", name, v);
}

_PUBLIC_ void ndr_print_uint32(struct ndr_print *ndr, const char *name, uint32_t v)
{
	ndr->print(ndr, "%-25s: 0x%08x (%u)", name, v, v);
}

_PUBLIC_ void ndr_print_udlong(struct ndr_print *ndr, const char *name, uint64_t v)
{
	ndr->print(ndr, "%-25s: 0x%016llx (%llu)", name, (unsigned long long)v, (unsigned long long)v);
}

_PUBLIC_ void ndr_print_udlongr(struct ndr_print *ndr, const char *name, uint64_t v)
{
	ndr_print_udlong(ndr, name, v);
}

_PUBLIC_ void ndr_print_dlong(struct ndr_print *ndr, const char *name, int64_t v)
{
	ndr->print(ndr, "%-25s: 0x%016llx (%lld)", name, (unsigned long long)v, (long long)v);
}

_PUBLIC_ void ndr_print_hyper(struct ndr_print *ndr, const char *name, uint64_t v)
{
	ndr_print_dlong(ndr, name, v);
}

_PUBLIC_ void ndr_print_pointer(struct ndr_print *ndr, const char *name, void *v)
{
	ndr->print(ndr, "%-25s: %p", name, v);
}

_PUBLIC_ void ndr_print_ptr(struct ndr_print *ndr, const char *name, const void *p)
{
	if (p) {
		ndr->print(ndr, "%-25s: *", name);
	} else {
		ndr->print(ndr, "%-25s: NULL", name);
	}
}

_PUBLIC_ void ndr_print_NTTIME(struct ndr_print *ndr, const char *name, NTTIME t)
{
	ndr->print(ndr, "%-25s: %s", name, nt_time_string(ndr, t));
}

_PUBLIC_ void ndr_print_NTTIME_1sec(struct ndr_print *ndr, const char *name, NTTIME t)
{
	/* this is a standard NTTIME here
	 * as it's already converted in the pull/push code
	 */
	ndr_print_NTTIME(ndr, name, t);
}

_PUBLIC_ void ndr_print_NTTIME_hyper(struct ndr_print *ndr, const char *name, NTTIME t)
{
	ndr_print_NTTIME(ndr, name, t);
}

_PUBLIC_ void ndr_print_time_t(struct ndr_print *ndr, const char *name, time_t t)
{
	if (t == (time_t)-1 || t == 0) {
		ndr->print(ndr, "%-25s: (time_t)%d", name, (int)t);
	} else {
		ndr->print(ndr, "%-25s: %s", name, timestring(ndr, t));
	}
}

_PUBLIC_ void ndr_print_union(struct ndr_print *ndr, const char *name, int level, const char *type)
{
	if (ndr->flags & LIBNDR_PRINT_ARRAY_HEX) {
		ndr->print(ndr, "%-25s: union %s(case 0x%X)", name, type, level);
	} else {
		ndr->print(ndr, "%-25s: union %s(case %d)", name, type, level);
	}
}

_PUBLIC_ void ndr_print_bad_level(struct ndr_print *ndr, const char *name, uint16_t level)
{
	ndr->print(ndr, "UNKNOWN LEVEL %u", level);
}

_PUBLIC_ void ndr_print_array_uint8(struct ndr_print *ndr, const char *name, 
			   const uint8_t *data, uint32_t count)
{
	int i;

	if (count <= 600 && (ndr->flags & LIBNDR_PRINT_ARRAY_HEX)) {
		char s[1202];
		for (i=0;i<count;i++) {
			snprintf(&s[i*2], 3, "%02x", data[i]);
		}
		s[i*2] = 0;
		ndr->print(ndr, "%-25s: %s", name, s);
		return;
	}

	ndr->print(ndr, "%s: ARRAY(%d)", name, count);
	ndr->depth++;
	for (i=0;i<count;i++) {
		char *idx=NULL;
		if (asprintf(&idx, "[%d]", i) != -1) {
			ndr_print_uint8(ndr, idx, data[i]);
			free(idx);
		}
	}
	ndr->depth--;	
}

_PUBLIC_ void ndr_print_DATA_BLOB(struct ndr_print *ndr, const char *name, DATA_BLOB r)
{
	ndr->print(ndr, "%-25s: DATA_BLOB length=%u", name, (unsigned)r.length);
	if (r.length) {
		dump_data(10, r.data, r.length);
	}
}


/*
  push a DATA_BLOB onto the wire. 
*/
_PUBLIC_ enum ndr_err_code ndr_push_DATA_BLOB(struct ndr_push *ndr, int ndr_flags, DATA_BLOB blob)
{
	if (ndr->flags & LIBNDR_ALIGN_FLAGS) {
		if (ndr->flags & LIBNDR_FLAG_ALIGN2) {
			blob.length = NDR_ALIGN(ndr, 2);
		} else if (ndr->flags & LIBNDR_FLAG_ALIGN4) {
			blob.length = NDR_ALIGN(ndr, 4);
		} else if (ndr->flags & LIBNDR_FLAG_ALIGN8) {
			blob.length = NDR_ALIGN(ndr, 8);
		}
		NDR_PUSH_ALLOC_SIZE(ndr, blob.data, blob.length);
		data_blob_clear(&blob);
	} else if (!(ndr->flags & LIBNDR_FLAG_REMAINING)) {
		NDR_CHECK(ndr_push_uint32(ndr, NDR_SCALARS, blob.length));
	}
	NDR_CHECK(ndr_push_bytes(ndr, blob.data, blob.length));
	return NDR_ERR_SUCCESS;
}

/*
  pull a DATA_BLOB from the wire. 
*/
_PUBLIC_ enum ndr_err_code ndr_pull_DATA_BLOB(struct ndr_pull *ndr, int ndr_flags, DATA_BLOB *blob)
{
	uint32_t length = 0;

	if (ndr->flags & LIBNDR_ALIGN_FLAGS) {
		if (ndr->flags & LIBNDR_FLAG_ALIGN2) {
			length = NDR_ALIGN(ndr, 2);
		} else if (ndr->flags & LIBNDR_FLAG_ALIGN4) {
			length = NDR_ALIGN(ndr, 4);
		} else if (ndr->flags & LIBNDR_FLAG_ALIGN8) {
			length = NDR_ALIGN(ndr, 8);
		}
		if (ndr->data_size - ndr->offset < length) {
			length = ndr->data_size - ndr->offset;
		}
	} else if (ndr->flags & LIBNDR_FLAG_REMAINING) {
		length = ndr->data_size - ndr->offset;
	} else {
		NDR_CHECK(ndr_pull_uint32(ndr, NDR_SCALARS, &length));
	}
	NDR_PULL_NEED_BYTES(ndr, length);
	*blob = data_blob_talloc(ndr->current_mem_ctx, ndr->data+ndr->offset, length);
	ndr->offset += length;
	return NDR_ERR_SUCCESS;
}

_PUBLIC_ uint32_t ndr_size_DATA_BLOB(int ret, const DATA_BLOB *data, int flags)
{
	if (!data) return ret;
	return ret + data->length;
}

_PUBLIC_ void ndr_print_bool(struct ndr_print *ndr, const char *name, const bool b)
{
	ndr->print(ndr, "%-25s: %s", name, b?"true":"false");
}

_PUBLIC_ void ndr_print_sockaddr_storage(struct ndr_print *ndr, const char *name, const struct sockaddr_storage *ss)
{
	char addr[INET6_ADDRSTRLEN];
	ndr->print(ndr, "%-25s: %s", name, print_sockaddr(addr, sizeof(addr), ss));
}

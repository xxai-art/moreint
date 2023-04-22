#include <postgres.h>
#include <fmgr.h>
#include <access/hash.h>

#include "moreint.h"

#define make_hashfunc(type, BTYPE, casttype) \
PG_FUNCTION_INFO_V1(hash##type); \
Datum \
hash##type(PG_FUNCTION_ARGS) \
{ \
	return hash_uint32((casttype) PG_GETARG_##BTYPE(0)); \
} \
extern int no_such_variable

make_hashfunc(i8, INT8, int32);
make_hashfunc(u8, UINT8, uint32);
make_hashfunc(u16, UINT16, uint32);
make_hashfunc(u32, UINT32, uint32);

PG_FUNCTION_INFO_V1(hashu64);
Datum
hashu64(PG_FUNCTION_ARGS)
{
	/* see also hashint8 */
	uint64		val = PG_GETARG_UINT64(0);
	uint32		lohalf = (uint32) val;
	uint32		hihalf = (uint32) (val >> 32);

	lohalf ^= hihalf;

	return hash_uint32(lohalf);
}

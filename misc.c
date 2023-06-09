#include <postgres.h>
#include <fmgr.h>

#include "moreint.h"


PG_FUNCTION_INFO_V1(i8um);
Datum
i8um(PG_FUNCTION_ARGS)
{
	int8		arg = PG_GETARG_INT8(0);
	int8		result;

	result = -arg;
	/* overflow check */
	if (arg != 0 && SAMESIGN(result, arg))
		ereport(ERROR,
				(errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
				 errmsg("integer out of range")));
	PG_RETURN_INT8(result);
}

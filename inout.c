#include <postgres.h>

#include <fmgr.h>
#include <libpq/pqformat.h>
#include <utils/builtins.h>

#include "moreint.h"

#include <inttypes.h>
#include <limits.h>

inline uint64 u64_from_le_bytes(const uint8* bytes)
{
  return (((uint64)bytes[7]) << (8 * 7)) | (((uint64)bytes[6]) << (8 * 6)) | (((uint64)bytes[5]) << (8 * 5)) | (((uint64)bytes[4]) << (8 * 4)) | (((uint64)bytes[3]) << (8 * 3)) | (((uint64)bytes[2]) << (8 * 2)) | (((uint64)bytes[1]) << (8 * 1)) | (((uint64)bytes[0]) << (8 * 0));
}

PG_FUNCTION_INFO_V1(i8in);
Datum i8in(PG_FUNCTION_ARGS)
{
  char* s = PG_GETARG_CSTRING(0);

#if PG_VERSION_NUM >= 150000
  PG_RETURN_INT8(pg_strtoint64(s));
#else
  PG_RETURN_INT8(pg_atoi(s, sizeof(int8), '\0'));
#endif
}

PG_FUNCTION_INFO_V1(i8out);
Datum i8out(PG_FUNCTION_ARGS)
{
  int8 arg1 = PG_GETARG_INT8(0);
  char* result = palloc(5); /* sign, 3 digits, '\0' */

  sprintf(result, "%d", arg1);
  PG_RETURN_CSTRING(result);
}

static uint32
pg_atou(const char* s, int size)
{
  unsigned long int result;
  bool out_of_range = false;
  char* badp;

  if (s == NULL)
    elog(ERROR, "NULL pointer");
  if (*s == 0)
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input syntax for unsigned integer: \"%s\"",
                    s)));

  if (strchr(s, '-'))
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input syntax for unsigned integer: \"%s\"",
                    s)));

  errno = 0;
  result = strtoul(s, &badp, 10);

  switch (size)
  {
  case sizeof(uint32):
    if (errno == ERANGE
#if defined(HAVE_LONG_INT_64)
        || result > UINT_MAX
#endif
       )
      out_of_range = true;
    break;
  case sizeof(uint16):
    if (errno == ERANGE || result > USHRT_MAX)
      out_of_range = true;
    break;
  case sizeof(uint8):
    if (errno == ERANGE || result > UCHAR_MAX)
      out_of_range = true;
    break;
  default:
    elog(ERROR, "unsupported result size: %d", size);
  }

  if (out_of_range)
    ereport(ERROR,
            (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
             errmsg("value \"%s\" is out of range for type u%d", s, size)));

  while (*badp && isspace((unsigned char)*badp))
    badp++;

  if (*badp)
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input syntax for unsigned integer: \"%s\"",
                    s)));

  return result;
}

PG_FUNCTION_INFO_V1(u8in);
Datum u8in(PG_FUNCTION_ARGS)
{
  char* s = PG_GETARG_CSTRING(0);

  PG_RETURN_UINT8(pg_atou(s, sizeof(uint8)));
}

PG_FUNCTION_INFO_V1(u8out);
Datum u8out(PG_FUNCTION_ARGS)
{
  uint8 arg1 = PG_GETARG_UINT8(0);
  char* result = palloc(4); /* 3 digits, '\0' */

  sprintf(result, "%u", arg1);
  PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(u16in);
Datum u16in(PG_FUNCTION_ARGS)
{
  char* s = PG_GETARG_CSTRING(0);

  PG_RETURN_UINT16(pg_atou(s, sizeof(uint16)));
}

PG_FUNCTION_INFO_V1(u16out);
Datum u16out(PG_FUNCTION_ARGS)
{
  uint16 arg1 = PG_GETARG_UINT16(0);
  char* result = palloc(6); /* 5 digits, '\0' */

  sprintf(result, "%u", arg1);
  PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(u32in);
Datum u32in(PG_FUNCTION_ARGS)
{
  char* s = PG_GETARG_CSTRING(0);

  PG_RETURN_UINT32(pg_atou(s, sizeof(uint32)));
}

PG_FUNCTION_INFO_V1(u32out);
Datum u32out(PG_FUNCTION_ARGS)
{
  uint32 arg1 = PG_GETARG_UINT32(0);
  char* result = palloc(11); /* 10 digits, '\0' */

  sprintf(result, "%u", arg1);
  PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(u64in);
Datum u64in(PG_FUNCTION_ARGS)
{
  char* s = PG_GETARG_CSTRING(0);
  unsigned long long int result;
  char* badp;

  if (s == NULL)
    elog(ERROR, "NULL pointer");
  if (*s == 0)
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input syntax for unsigned integer: \"%s\"",
                    s)));

  if (strchr(s, '-'))
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input syntax for unsigned integer: \"%s\"",
                    s)));

  errno = 0;
  result = strtoull(s, &badp, 10);

  if (errno == ERANGE)
    ereport(ERROR,
            (errcode(ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE),
             errmsg("value \"%s\" is out of range for type u%d", s, 8)));

  while (*badp && isspace((unsigned char)*badp))
    badp++;

  if (*badp)
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input syntax for unsigned integer: \"%s\"",
                    s)));

  PG_RETURN_UINT64(result);
}

PG_FUNCTION_INFO_V1(u64out);
Datum u64out(PG_FUNCTION_ARGS)
{
  uint64 arg1 = PG_GETARG_UINT64(0);
  char* result = palloc(21); /* 20 digits, '\0' */

  sprintf(result, "%" PRIu64, (unsigned long long)arg1);
  PG_RETURN_CSTRING(result);
}

PG_FUNCTION_INFO_V1(i8recv);
Datum i8recv(PG_FUNCTION_ARGS)
{
  StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
  PG_RETURN_INT8((int8)pq_getmsgint(buf, sizeof(int8)));
}

PG_FUNCTION_INFO_V1(i8send);
Datum i8send(PG_FUNCTION_ARGS)
{
  int8 arg1 = PG_GETARG_INT8(0);
  StringInfoData buf;

  pq_begintypsend(&buf);
  pq_sendint(&buf, arg1, sizeof(int8));
  PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(u8recv);
Datum u8recv(PG_FUNCTION_ARGS)
{
  StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
  PG_RETURN_UINT8((uint8)pq_getmsgint(buf, sizeof(uint8)));
}

PG_FUNCTION_INFO_V1(u8send);
Datum u8send(PG_FUNCTION_ARGS)
{
  uint8 arg1 = PG_GETARG_UINT8(0);
  StringInfoData buf;

  pq_begintypsend(&buf);
  pq_sendint(&buf, arg1, sizeof(uint8));
  PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(u16recv);
Datum u16recv(PG_FUNCTION_ARGS)
{
  StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
  PG_RETURN_UINT16((uint16)pq_getmsgint(buf, sizeof(uint16)));
}

PG_FUNCTION_INFO_V1(u16send);
Datum u16send(PG_FUNCTION_ARGS)
{
  uint16 arg1 = PG_GETARG_UINT16(0);
  StringInfoData buf;

  pq_begintypsend(&buf);
  pq_sendint(&buf, arg1, sizeof(uint16));
  PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(u32recv);
Datum u32recv(PG_FUNCTION_ARGS)
{
  StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
  PG_RETURN_UINT32((uint32)pq_getmsgint(buf, sizeof(uint32)));
}

PG_FUNCTION_INFO_V1(u32send);
Datum u32send(PG_FUNCTION_ARGS)
{
  uint32 arg1 = PG_GETARG_UINT32(0);
  StringInfoData buf;

  pq_begintypsend(&buf);
  pq_sendint(&buf, arg1, sizeof(uint32));
  PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}

PG_FUNCTION_INFO_V1(u64recv);
Datum u64recv(PG_FUNCTION_ARGS)
{
  StringInfo buf = (StringInfo)PG_GETARG_POINTER(0);
  PG_RETURN_UINT64((uint64)pq_getmsgint64(buf));
}

PG_FUNCTION_INFO_V1(u64send);
Datum u64send(PG_FUNCTION_ARGS)
{
  uint64 arg1 = PG_GETARG_UINT64(0);
  StringInfoData buf;

  pq_begintypsend(&buf);
  pq_sendint64(&buf, arg1);
  PG_RETURN_BYTEA_P(pq_endtypsend(&buf));
}


PG_FUNCTION_INFO_V1(bytea_u64);
Datum bytea_u64(PG_FUNCTION_ARGS)
{
  bytea* data = PG_GETARG_BYTEA_P(0);
  size_t data_length = VARSIZE_ANY_EXHDR(data);
  if (data_length != 8)
    ereport(ERROR,
            (errcode(ERRCODE_INVALID_TEXT_REPRESENTATION),
             errmsg("invalid input length for u64: expected 8 , get %lu", data_length)));

  //  elog(NOTICE, ">> u64_from_le_bytes(data->vl_dat) %lu", u64_from_le_bytes(data->vl_dat));
  PG_RETURN_UINT64(u64_from_le_bytes((uint8*)data->vl_dat));
}

CREATE FUNCTION to_hex(u32) RETURNS text IMMUTABLE STRICT LANGUAGE C AS '$libdir/moreint', 'to_hex_u32';
CREATE FUNCTION to_hex(u64) RETURNS text IMMUTABLE STRICT LANGUAGE C AS '$libdir/moreint', 'to_hex_u64';

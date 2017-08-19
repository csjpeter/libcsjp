/*
 * Author: Csaszar, Peter <csjpeter@gmail.com>
 * Copyright (C) 2016 Csaszar, Peter
 */

#include <math.h>
#include <stdio.h>
#include <stdint.h>

#include "csjp_string.h"

namespace csjp {

int AStr::compare(const char * str, size_t _length) const
{
	ENSURE(str || !_length,  InvalidArgument);

	//DBG("'%' (%) == '%' (%)", data, len, str, _length);

	if(!_length)
		return (len == 0) ? 0 : 1;
	if(!len)
		return -1;

	const char * ptr = data;
	const char * const endPtr = data + len;
	const char * const strEndPtr = str + _length;

	for(; ptr < endPtr && str < strEndPtr; str++, ptr++){
		if(*ptr == *str)
			continue;
		if(*ptr < *str)
			return -1;
		else
			return 1;
	}

	if(len < _length)
		return -1;
	else if(_length < len)
		return 1;

	return 0;
}

int AStr::compare(const char * str) const
{
	if(!str)
		return (len == 0) ? 0 : 1;

	size_t _length = strlen(str);

	return compare(str, _length);
}

bool AStr::isEqual(const char * str, size_t _length) const
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return len == 0;
	if(len != _length)
		return false;

	return !compare(str, _length);
}

bool AStr::isEqual(const char * str) const
{
	if(!str)
		return len == 0;

	size_t _length = strlen(str);

	return !compare(str, _length);
}

bool AStr::findFirst(size_t & pos, const char * str, size_t _length, size_t from, size_t until)
	const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(str,  InvalidArgument);

	if(!_length)
		return false;

	if(until - from < _length)
		return false;
	if(!data)
		return false;

	size_t p = 0;

	const char * refPtr = data + from;
	const char * const refEndPtr = data + until;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; refPtr < refEndPtr; refPtr++){
		if(*refPtr != *strPtr){
			if(strPtr == str)
				continue;
			strPtr = str;
		}
		if(*refPtr == *strPtr){
			if(strPtr == str)
				p = refPtr - data;
			strPtr++;
			if(strPtr == strEndPtr){
				pos = p;
				return true;
			}
		}
	}

	return false;
}

bool AStr::findFirstOf(size_t & pos, const char * str, size_t _length, size_t from, size_t until)
	const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(str,  InvalidArgument);
	ENSURE(_length,  InvalidArgument);

	if(until - from < _length)
		return false;
	if(!data)
		return false;

	const char * refPtr = data + from;
	const char * const refEndPtr = data + until;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; refPtr < refEndPtr; refPtr++){
		for(strPtr = str; strPtr < strEndPtr; strPtr++){
			if(*refPtr == *strPtr){
				pos = refPtr - data;
				return true;
			}
		}
	}

	return false;
}

bool AStr::findFirstNotOf(size_t & pos, const char * str, size_t _length, size_t from, size_t until)
	const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(str,  InvalidArgument);
	ENSURE(_length,  InvalidArgument);

	if(!data)
		return false;

	const char * refPtr = data + from;
	const char * const refEndPtr = data + until;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; refPtr < refEndPtr; refPtr++){
		bool stranger = true;
		for(strPtr = str; strPtr < strEndPtr; strPtr++){
			if(*refPtr == *strPtr){
				stranger = false;
				break;
			}
		}
		if(stranger){
			pos = refPtr - data;
			return true;
		}
	}

	return false;
}

bool AStr::findLast(size_t & pos, const char * str, size_t _length, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(str,  InvalidArgument);
	ENSURE(_length,  InvalidArgument);

	if(!data)
		return false;

	const char * refPtr = data + until - 1;
	const char * strLastPtr = str + _length - 1;
	const char * strPtr = strLastPtr;

	for(; data <= refPtr; refPtr--){
		if(*refPtr != *strPtr){
			if(strPtr == strLastPtr)
				continue;
			strPtr = strLastPtr;
		}
		if(*refPtr == *strPtr){
			strPtr--;
			if(strPtr < str){
				pos = refPtr - data;
				return true;
			}
		}
	}

	return false;
}

bool AStr::findLastOf(size_t & pos, const char * str, size_t _length, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(str,  InvalidArgument);
	ENSURE(_length,  InvalidArgument);

	if(!data)
		return false;

	const char * refPtr = data + until - 1;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; data <= refPtr; refPtr--){
		for(strPtr = str; strPtr < strEndPtr; strPtr++){
			if(*refPtr == *strPtr){
				pos = refPtr - data;
				return true;
			}
		}
	}

	return false;
}

bool AStr::findLastNotOf(size_t & pos, const char * str, size_t _length, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(str || !_length,  InvalidArgument);

	if(!until)
		return false;
	if(!data)
		return false;
	if(!str){
		/* len != 0 ; if len was 0, until is either greater (invalid)
		 * or 0 what is already handled by returning false. */
		pos = len - 1;
		return true;
	}

	const char * refPtr = data + until - 1;
	const char * strPtr = str;
	const char * const strEndPtr = str + _length;

	for(; data <= refPtr; refPtr--){
		bool stranger = true;
		for(strPtr = str; strPtr < strEndPtr; strPtr++){
			if(*refPtr == *strPtr){
				stranger = false;
				break;
			}
		}
		if(stranger){
			pos = refPtr - data;
			return true;
		}
	}

	return false;
}

bool AStr::startsWith(const char * str, size_t _length) const
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return true;
	if(!len)
		return false;
	if(len < _length)
		return false;

	const char * ptr = data;
	const char * const strEndPtr = str + _length;

	for(; str < strEndPtr; str++, ptr++)
		if(*ptr != *str)
			return false;

	return true;
}

bool AStr::endsWith(const char * str, size_t _length) const
{
	ENSURE(str || !_length,  InvalidArgument);

	if(!_length)
		return true;
	if(!len)
		return false;
	if(len < _length)
		return false;

	const char * refPtr = data + len - 1;
	const char * strPtr = str + _length - 1;

	for(; data <= refPtr && str <= strPtr; refPtr--, strPtr--){
		if(*refPtr == *strPtr)
			continue;
		return false;
	}

	return true;
}

size_t AStr::count(const char * str, size_t _length, size_t from, size_t until) const
{
	ENSURE(until <= len,  InvalidArgument);
	ENSURE(from <= until,  InvalidArgument);
	ENSURE(str,  InvalidArgument);

	if(until - from < _length)
		return 0;

	size_t pos = from;
	size_t matches = 0;

	while(findFirst(pos, str, _length, pos, until)){
		pos += _length;
		matches++;
	}

	return matches;
}

Array<Str> AStr::split(const char * delimiters, bool avoidEmptyResults) const
{
	Array<Str> result;

	if(!delimiters || delimiters[0] == 0){
		if(avoidEmptyResults && len == 0)
			return result;
		result.setCapacity(1);
		result.add(data, len);
		return result;
	}

	const char * until = data + len;
	size_t d_len = strlen(delimiters);
	const char * d_until = delimiters + d_len;

	const char * res_start = data;
	size_t res_len = 0;
	const char * iter = data;

	while(iter < until){
		const char * d_iter = delimiters;
		for(; d_iter < d_until; d_iter++)
			if(*iter == *d_iter)
				break;

		if(d_iter == d_until){
			iter++;
			res_len++;
			continue;
		}

		if(!avoidEmptyResults || res_len)
			result.add(res_start, res_len);
		iter++;
		res_start = iter;
		res_len = 0;
	}
	if(!avoidEmptyResults || res_len)
		result.add(res_start, res_len);

	return result;
}

static const unsigned char * base64EncodeArray = (unsigned char*)
		"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
		"abcdefghijklmnopqrstuvwxyz"
		"0123456789+/";

String AStr::encodeBase64() const
{
	String base64;
	base64.setCapacity(len + len / 3 + 4);
	size_t j = 0;

	size_t l = len - len % 3;
	for(size_t i = 0; i < l; i += 3){
		unsigned char d0 = data[i];
		unsigned char d1 = data[i+1];
		unsigned char d2 = data[i+2];
		base64[j++] = base64EncodeArray[(d0 & 0xfc) >> 2];
		base64[j++] = base64EncodeArray[((d0 & 0x03) << 4) | ((d1 & 0xf0) >> 4)];
		base64[j++] = base64EncodeArray[((d1 & 0x0f) << 2) | ((d2 & 0xc0) >> 6)];
		base64[j++] = base64EncodeArray[(d2 & 0x3f)];
	}

	if(len % 3 == 1){
		unsigned char d0 = data[l];
		base64[j++] = base64EncodeArray[(d0 & 0xfc) >> 2];
		base64[j++] = base64EncodeArray[((d0 & 0x03) << 4)];
		base64[j++] = '=';
		base64[j++] = '=';
	} else if(len % 3 == 2){
		unsigned char d0 = data[l];
		unsigned char d1 = data[l+1];
		base64[j++] = base64EncodeArray[(d0 & 0xfc) >> 2];
		base64[j++] = base64EncodeArray[((d0 & 0x03) << 4) | ((d1 & 0xf0) >> 4)];
		base64[j++] = base64EncodeArray[((d1 & 0x0f) << 2)];
		base64[j++] = '=';
	}

	base64.len = j;
	base64[j] = 0;
	
	return base64;
}

/* At position of ie. 'B', lets find the index of 'B' (1) in base64EncodeArray.
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,'+',768,768,768,'/',
	'0','1','2','3','4','5','6','7','8','9',768,768,768,'=',768,768,
	768,'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O',
	'P','Q','R','S','T','U','V','W','X','Y','Z',768,768,768,768,768,
	768,'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o',
	'p','q','r','s','t','u','v','w','x','y','z',768,768,768,768,768
*/
static const unsigned base64DecodeArray[] = {
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768, 62,768,768,768, 63,
	 52, 53, 54, 55, 56, 57, 58, 59, 60, 61,768,768,768,256,768,768,
	768,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
	 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 25,768,768,768,768,
	768, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
	 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,
	768,768,768,768,768,768,768,768,768,768,768,768,768,768,768,768
};

String AStr::decodeBase64() const
{
	if(len % 4)
		throw InvalidArgument("This string is not in base64 format "
				"(length % 4 != 0).");

	String str;
	str.setCapacity(( len / 4 ) * 3);
	size_t j = 0;

	unsigned d0, d1, d2=0, d3=0;
	for(size_t i = 0; i < len; i += 4){
		d0 = base64DecodeArray[(unsigned char)data[i]];
		d1 = base64DecodeArray[(unsigned char)data[i+1]];
		d2 = base64DecodeArray[(unsigned char)data[i+2]];
		d3 = base64DecodeArray[(unsigned char)data[i+3]];
#ifndef PERFMODE
		if(i < len - 4) {
			if(256 <= (d0 + d1 + d2 + d3))
				throw InvalidArgument("Unexpected character found "
						"near position %", i);
		} else {
			if(768 <= (d0 + d1 + d2 + d3) ||
				512 <= (d0 + d1 + d2) ||
				256 <= (d0 + d1) ||
				(d2 == 256 && d3 != 256))
				throw InvalidArgument("Unexpected character found "
						"near position %", i);
		}
#endif
		str[j++] = (d0 << 2) | (d1 >> 4);
		str[j++] = (d1 << 4) | ((unsigned char)d2 >> 2);
		str[j++] = ((unsigned char)d2 << 6) | (unsigned char)d3;
	}

	if(d3 == 256)
		j--;
	if(d2 == 256)
		j--;
	str.len = j;
	str[j] = 0;

	return str;
}

/** Convert the string into space separated uppercase hexadecimal numbers.
 */
String AStr::toHexaString() const
{
	String hexaString;
	for (size_t i = 0; i < length; i++)
		hexaString.appendPrintf("%02hhX ", (unsigned char)(data[i]));
	return hexaString;
}

} // namespace


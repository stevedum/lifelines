/* 
   Copyright (c) 1991-1999 Thomas T. Wetmore IV

   Permission is hereby granted, free of charge, to any person
   obtaining a copy of this software and associated documentation
   files (the "Software"), to deal in the Software without
   restriction, including without limitation the rights to use, copy,
   modify, merge, publish, distribute, sublicense, and/or sell copies
   of the Software, and to permit persons to whom the Software is
   furnished to do so, subject to the following conditions:

   The above copyright notice and this permission notice shall be
   included in all copies or substantial portions of the Software.

   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
   NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
   BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
   ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
   CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
   SOFTWARE.
*/
/*=============================================================
 * place.c -- Handle place values
 * Copyright(c) 1993-94 by T.T. Wetmore IV; all rights reserved
 *   2.3.4 - 30 Aug 93    3.0.0 - 28 Jun 94
 *===========================================================*/

#include "standard.h"

BOOLEAN value_to_list (STRING str,
		       LIST list,
		       INT *plen,
		       STRING dlm);

/*===================================================
 * place_to_list -- Convert place string to word list
 *=================================================*/
BOOLEAN place_to_list (place, list, plen)
STRING place;
LIST list;
INT *plen;
{
	return value_to_list(place, list, plen, ",");
}
/*=============================================
 * value_to_list -- Convert string to word list
 *===========================================*/
BOOLEAN value_to_list (STRING str,
		       LIST list,
		       INT *plen,
		       STRING dlm)
{
	static STRING buf = NULL;
	static INT len0 = 0;
	STRING p, q, n;
	INT len, c, i, j;

	if (!str || *str == 0 || !list) return FALSE;
	make_list_empty(list);
	set_list_type(list, LISTDOFREE);
	if ((len = strlen(str)) > len0 - 2) {
		if (buf) stdfree(buf);
		buf = (STRING) stdalloc(len0 = len + 80);
	}
	strcpy(buf, str);
	buf[len + 1] = 0;
	p = buf;
	j = 1;
	while (c = *p++) {
		if (in_string(c, dlm)) {
			*(p - 1) = 0;
			j++;
		}
	}
	p = buf;
	for (i = 1;  i <= j;  i++) {
		n = p + strlen(p) + 1;
		while (chartype(c = *p++) == WHITE)
			;
		p--;
		q = p + strlen(p) - 1;
		while (q > p && chartype(*q) == WHITE)
			*q-- = 0;
		set_list_element(list, i, strsave(p));
		p = n;
	}
	*plen = j;
	return TRUE;
}

BOOLEAN in_string (chr, str)
INT chr;
STRING str;
{
	while (*str && chr != *str)
		str++;
	return *str != 0;
}

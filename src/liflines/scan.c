/* 
   Copyright (c) 2000 Perry Rapp

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

#include "sys_inc.h"
#include <stdarg.h>
#include "llstdlib.h"
#include "table.h"
#include "indiseq.h"
#include "interp.h"
#include "screen.h"
#include "liflines.h"

#include "llinesi.h"

extern STRING scanrs, scnnmf, scnfnm, scnrfn, scantt;

typedef struct
{
	INT scantype;
	char string[64];
} SCAN_PATTERN;

static INDISEQ results_seq;

static INT NAMESCAN_FULL=0;
static INT NAMESCAN_FRAG=1;
static INT REFNSCAN=2;

/*=============================================
 * pattern_match -- Compare a name to a pattern
 *===========================================*/
static BOOLEAN
pattern_match (SCAN_PATTERN *patt, STRING name)
{
	STRING p1,p2;
	/* match . to any letter, and trailing * to anything */
	p1=patt->string;
	p2=name;
	for (p1=patt->string,p2=name; *p1 || *p2; p1++,p2++) {
		if (*p1 == '*' && *(p1+1) == 0)
			return TRUE;
		if (!(*p1) || !(*p2))
			return FALSE;
		if (*p1 != '.' && ll_toupper(*p1) != ll_toupper(*p2))
			return FALSE;
	}
	return TRUE;
}
/*===========================================
 * ns_callback -- callback for name traversal
 *=========================================*/
static BOOLEAN
ns_callback (STRING key, STRING name, BOOLEAN newset, void *param)
{
	LIST list;
	INT len, ind;
	STRING piece;
	SCAN_PATTERN * patt = (SCAN_PATTERN *)param;
	if (newset) {
		/* clear dup table */
	} else {
		/* if in dup table, report */
		/* else add to dup table */
	}
	if (patt->scantype == NAMESCAN_FULL) {
		if (pattern_match(patt, name)) {
			/* if we pass in name, append_indiseq won't check for dups */
			append_indiseq_null(results_seq, key, NULL, FALSE, FALSE);
		}
	} else {
		/* NAMESCAN_FRAG */
		list = create_list();
		name_to_list(name, list, &len, &ind);
		FORLIST(list, el)
			piece = (STRING)el;
			if (pattern_match(patt, piece)) {
				/* if we pass in name, append_indiseq won't check for dups */
				append_indiseq_null(results_seq, key, NULL, FALSE, FALSE);
				break;
			}
		ENDLIST
		free_name_list(list);
	}
	return TRUE;
}
/*===========================================
 * rs_callback -- callback for refn traversal
 *=========================================*/
static BOOLEAN
rs_callback (STRING key, STRING refn, BOOLEAN newset, void *param)
{
	SCAN_PATTERN * patt = (SCAN_PATTERN *)param;
	ASSERT(patt->scantype == REFNSCAN);

	if (pattern_match(patt, refn)) {
		/* if we pass in name, append_indiseq won't check for dups */
		append_indiseq_null(results_seq, key, NULL, FALSE, FALSE);
	}
	return TRUE;
}
/*=================================================
 * set_pattern -- check pattern for validity
 *  for first cut, just refuse anything with spaces
 *===============================================*/
static BOOLEAN
set_pattern (SCAN_PATTERN * patt, STRING str, INT scantype)
{
	INT i;
	/* for full name & refn scans, accept anything ? */
	if (scantype == NAMESCAN_FRAG) {
		for (i=0; str[i]; i++)
			if (str[i] == ' ')
				return FALSE;
	}

	if (strlen(str) > sizeof(patt->string)-1)
		return FALSE;

	strcpy(patt->string, str);

	return TRUE;
}
/*==============================
 * name_scan -- traverse names
 *  looking for pattern matching
 *============================*/
static NOD0
name_scan (INT scantype)
{
	SCAN_PATTERN patt;
	NOD0 indi = NULL;
	STRING str;

	patt.scantype = scantype;
	while (1) {
		if (scantype == NAMESCAN_FRAG)
			str = ask_for_string(scnnmf, scantt);
		else
			str = ask_for_string(scnfnm, scantt);
		if (!str || !str[0])
			return NULL;
		if (set_pattern(&patt, str, scantype))
			break;
	}


	results_seq = create_indiseq_null();
	traverse_names(ns_callback, &patt);

	if (length_indiseq(results_seq)) {
		indi = choose_from_indiseq(results_seq, DOASK1, scanrs, scanrs);
	}
	remove_indiseq(results_seq, FALSE); 
	results_seq=NULL;
	return indi;
}
/*==============================================
 * name_fragment_scan -- traverse name fragments
 *  looking for pattern matching
 *============================================*/
NOD0
name_fragment_scan (void)
{
	return name_scan(NAMESCAN_FRAG);
}
/*======================================
 * full_name_scan -- traverse full names
 *  looking for pattern matching
 *====================================*/
NOD0
full_name_scan (void)
{
	return name_scan(NAMESCAN_FULL);
}
/*==============================
 * refn_scan -- traverse refns
 *  looking for pattern matching
 *============================*/
NOD0
refn_scan(void)
{
	SCAN_PATTERN patt;
	NOD0 nod0 = NULL;
	STRING str;
	INT scantype = REFNSCAN;

	patt.scantype = scantype;
	while (1) {
		str = ask_for_string(scnrfn, scantt);
		if (!str || !str[0])
			return NULL;
		if (set_pattern(&patt, str, scantype))
			break;
	}

	results_seq = create_indiseq_null();
	traverse_refns(rs_callback, &patt);

	if (length_indiseq(results_seq)) {
		nod0 = choose_from_indiseq(results_seq, DOASK1, scanrs, scanrs);
	}
	remove_indiseq(results_seq, FALSE); 
	results_seq=NULL;
	return nod0;
}

﻿/*
 * @progname       exer_UTF-8
 * @version        0.3 (2002/11/16)
 * @author         Perry Rapp
 
 * @category       test

 * @output         mixed

 * @description    UTF-8 subset of exercise.ll tests.


*/



char_encoding("UTF-8")

require("lifelines-reports.version:1.3")
option("explicitvars") /* Disallow use of undefined variables */


proc finnish_UTF_8()
{
	if (not(set_and_check_locale("fi_FI", "Finnish"))) {
		return()
	}
	call set_section("finnish_UTF-8")
	/* sanity check */
	call check_collate3("A", "L", "Z")
	/* Adia sorts between Z and Odia */
	call check_collate3("Z", "Ä:[Adia]", "Ö:[Odia]")
	/* ydia & udia sort as y */
	call check_collate3("x", "y", "z")
	call check_collate3("x", "ÿ:[ydia]", "z")
	call check_collate3("x", "ü:[udia]", "z")
	/* eth (lower=u00F0) sorts as d */
	call check_collate3("c", "d", "e")
	call check_collate3("c", "ð:[eth]", "e")
}
proc polish_UTF_8()
{
	if (not(set_and_check_locale("pl_PL", "Polish"))) {
		return()
	}
	call set_section("polish_UTF-8")
	/* sanity check */
	call check_collate3("A", "L", "Z")
	/* Lstroke is between L and M */
	call check_collate3("L", "Ł:[Lstroke]", "M")
}
proc spanish_UTF_8()
{
	if (not(set_and_check_locale("es", "Spanish"))) {
		return()
	}
	call set_section("spanish_UTF-8")
	/* sanity check */
	call check_collate3("A", "N", "Z")
	/* ennay is between N and O */
	call check_collate3("N", "Ñ:[Ntilde]", "O")
}
proc testCollate_UTF_8()
{
	call finnish_UTF_8()
	call polish_UTF_8()
	call spanish_UTF_8()
	call set_section("")
}


## Makefile.am -- Run tests in the tests subdirectory

EXTRA_DIST              = defs $(TESTS)
MAINTAINERCLEANFILES    = Makefile.in
SHELL                   = /bin/bash

testsubdir              = date gengedcomstrong interp math pedigree-longname \
                          string view-history Royal92

TESTS_ENVIRONMENT       = top_builddir=$(top_builddir)

TESTS   =               date/checkjd2date.llscr         \
			gengedcomstrong/test1.llscr     \
			interp/eqv_pvalue.llscr         \
			interp/fullname.llscr           \
			math/test1.llscr                \
			math/test2.llscr                \
			pedigree-longname/test1.llscr   \
			string/string-unicode.llscr     \
			string/string-utf8.llscr        \
			view-history/view-history.llscr \
			Royal92/Royal92.llscr
# make runs run_a_test file.llscr to generate file.log
TEST_EXTENSIONS = .llscr
LLSCR_LOG_COMPILER = $(srcdir)/run_a_test

clean-local:
	-rm -f */*.llout */*.out */*.diff */*.fix
	-rm -rf */testdb */*.filter

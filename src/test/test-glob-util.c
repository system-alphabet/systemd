/* SPDX-License-Identifier: LGPL-2.1-or-later */

#include <stdlib.h>
#include <unistd.h>

#include "alloc-util.h"
#include "fs-util.h"
#include "glob-util.h"
#include "rm-rf.h"
#include "strv.h"
#include "tests.h"
#include "tmpfile-util.h"

TEST(glob_first) {
        char *first, name[] = SYSTEM_TMPDIR "/test-glob_first.XXXXXX";
        int fd = -EBADF;

        fd = ASSERT_OK(mkostemp_safe(name));
        close(fd);

        ASSERT_OK_EQ(glob_first(SYSTEM_TMPDIR "/test-glob_first*", &first), 1);
        ASSERT_STREQ(name, first);
        first = mfree(first);

        ASSERT_OK_ZERO_ERRNO(unlink(name));
        ASSERT_OK_ZERO(glob_first(SYSTEM_TMPDIR "/test-glob_first*", &first));
        ASSERT_NULL(first);
}

TEST(glob_exists) {
        char name[] = SYSTEM_TMPDIR "/test-glob_exists.XXXXXX";
        int fd = -EBADF;

        fd = ASSERT_OK(mkostemp_safe(name));
        close(fd);

        ASSERT_OK_EQ(glob_exists(SYSTEM_TMPDIR "/test-glob_exists*"), 1);

        ASSERT_OK_ZERO_ERRNO(unlink(name));
        ASSERT_OK_ZERO(glob_exists(SYSTEM_TMPDIR "/test-glob_exists*"));
}

TEST(safe_glob) {
        char template[] = SYSTEM_TMPDIR "/test-glob-util.XXXXXXX";
        const char *fn, *fn2, *fname;
        _cleanup_strv_free_ char **v = NULL;

        ASSERT_NOT_NULL(mkdtemp(template));

        fn = strjoina(template, "/*");
        ASSERT_ERROR(safe_glob(fn, /* flags= */ 0, &v), ENOENT);

        fn2 = strjoina(template, "/.*");
        ASSERT_ERROR(safe_glob(fn2, GLOB_NOSORT|GLOB_BRACE, &v), ENOENT);

        fname = strjoina(template, "/.foobar");
        ASSERT_OK(touch(fname));

        ASSERT_ERROR(safe_glob(fn, /* flags= */ 0, &v), ENOENT);

        ASSERT_OK(safe_glob(fn2, GLOB_NOSORT|GLOB_BRACE, &v));
        ASSERT_EQ(strv_length(v), 1u);
        ASSERT_STREQ(v[0], fname);
        ASSERT_NULL(v[1]);

        (void) rm_rf(template, REMOVE_ROOT|REMOVE_PHYSICAL);
}

static void test_glob_non_glob_prefix_one(const char *path, const char *expected) {
        _cleanup_free_ char *t;

        ASSERT_OK_ZERO(glob_non_glob_prefix(path, &t));
        ASSERT_STREQ(t, expected);
}

TEST(glob_non_glob) {
        test_glob_non_glob_prefix_one(SYSTEM_TMPDIR "/.X11-*", SYSTEM_TMPDIR "/");
        test_glob_non_glob_prefix_one(SYSTEM_TMPDIR "/*", SYSTEM_TMPDIR "/");
        test_glob_non_glob_prefix_one("/tmp*", "/");
        test_glob_non_glob_prefix_one(SYSTEM_TMPDIR "/*/whatever", SYSTEM_TMPDIR "/");
        test_glob_non_glob_prefix_one(SYSTEM_TMPDIR "/*/whatever?", SYSTEM_TMPDIR "/");
        test_glob_non_glob_prefix_one("/?", "/");

        char *x;
        ASSERT_ERROR(glob_non_glob_prefix("?", &x), ENOENT);
}

DEFINE_TEST_MAIN(LOG_INFO);

/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <errno.h>
#include <linux/fanotify.h>
#include <sys/syscall.h>
#include <unistd.h>

/* bionic has no <sys/fanotify.h> and never grew libc wrappers for the fanotify API, so provide the
 * glibc-compatible declarations here and implement them as direct syscalls. The kernel UAPI
 * constants and structs come from <linux/fanotify.h>. */

#if defined(__NR_fanotify_init)
static inline int fanotify_init(unsigned int flags, unsigned int event_f_flags) {
        return (int) syscall(__NR_fanotify_init, flags, event_f_flags);
}
#else
static inline int fanotify_init(unsigned int flags, unsigned int event_f_flags) {
        errno = ENOSYS;
        return -1;
}
#endif

#if defined(__NR_fanotify_mark)
static inline int fanotify_mark(int fanotify_fd, unsigned int flags, uint64_t mask, int dirfd, const char *pathname) {
        return (int) syscall(__NR_fanotify_mark, fanotify_fd, flags, mask, dirfd, pathname);
}
#else
static inline int fanotify_mark(int fanotify_fd, unsigned int flags, uint64_t mask, int dirfd, const char *pathname) {
        errno = ENOSYS;
        return -1;
}
#endif

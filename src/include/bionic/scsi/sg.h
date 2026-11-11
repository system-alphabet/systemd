/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include <stddef.h>

#include_next <scsi/sg.h>

/* SG_FLAG_LUN_INHIBIT was renamed to SG_FLAG_UNUSED_LUN_INHIBIT in newer kernel headers.
 * Android bionic's sg.h only provides the new name; provide the old name as an alias. */
#ifndef SG_FLAG_LUN_INHIBIT
#  ifdef SG_FLAG_UNUSED_LUN_INHIBIT
#    define SG_FLAG_LUN_INHIBIT SG_FLAG_UNUSED_LUN_INHIBIT
#  else
#    define SG_FLAG_LUN_INHIBIT 2
#  endif
#endif

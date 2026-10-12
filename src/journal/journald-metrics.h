/* SPDX-License-Identifier: LGPL-2.1-or-later */
#pragma once

#include "journald-forward.h"

#define JOURNALD_METRICS_SOCKET RUNSTATEDIR "/systemd/report/io.systemd.JournalDaemon"

int manager_open_metrics(Manager *m, int fd);

--- /dev/null	2015-09-11 11:21:55.750647684 +0300
+++ src/dragonfly.c
@@ -0,0 +1,736 @@
+/* -*- mode: c; c-basic-offset: 4; tab-width: 4; indent-tabs-mode: t -*-
+ * vim: ts=4 sw=4 noet ai cindent syntax=c
+ *
+ * Conky, a system monitor, based on torsmo
+ *
+ * Any original torsmo code is licensed under the BSD license
+ *
+ * All code written since the fork of torsmo is licensed under the GPL
+ *
+ * Please see COPYING for details
+ *
+ * Copyright (c) 2005-2010 Brenden Matthews, Philip Kovacs, et. al.
+ *	(see AUTHORS)
+ * All rights reserved.
+ *
+ * This program is free software: you can redistribute it and/or modify
+ * it under the terms of the GNU General Public License as published by
+ * the Free Software Foundation, either version 3 of the License, or
+ * (at your option) any later version.
+ *
+ * This program is distributed in the hope that it will be useful,
+ * but WITHOUT ANY WARRANTY; without even the implied warranty of
+ * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
+ * GNU General Public License for more details.
+ * You should have received a copy of the GNU General Public License
+ * along with this program.  If not, see <http://www.gnu.org/licenses/>.
+ *
+ */
+
+#include <sys/ioctl.h>
+#include <sys/param.h>
+#include <sys/resource.h>
+#include <sys/socket.h>
+#include <sys/stat.h>
+#include <sys/sysctl.h>
+#include <sys/time.h>
+#include <sys/types.h>
+#include <sys/user.h>
+
+#include <net/if.h>
+#include <net/if_mib.h>
+#include <net/if_media.h>
+#include <net/if_var.h>
+
+#include <devstat.h>
+#include <ifaddrs.h>
+#include <limits.h>
+#include <unistd.h>
+
+//#include <dev/wi/if_wavelan_ieee.h>
+#include <dev/acpica/acpiio.h>
+
+#include "conky.h"
+#include "dragonfly.h"
+#include "logging.h"
+#include "net_stat.h"
+#include "top.h"
+#include "diskio.h"
+
+#define	GETSYSCTL(name, var)	getsysctl(name, &(var), sizeof(var))
+#define	KELVTOC(x)				((x - 2732) / 10.0)
+
+__attribute__((gnu_inline)) inline void
+proc_find_top(struct process **cpu, struct process **mem, struct process **time);
+
+static short cpu_setup = 0;
+
+static int getsysctl(const char *name, void *ptr, size_t len)
+{
+	size_t nlen = len;
+
+	if (sysctlbyname(name, ptr, &nlen, NULL, 0) == -1) {
+		return -1;
+	}
+
+	if (nlen != len && errno == ENOMEM) {
+		return -1;
+	}
+
+	return 0;
+}
+
+struct ifmibdata *data = NULL;
+size_t len = 0;
+
+static int swapmode(unsigned long *retavail, unsigned long *retfree)
+{
+	int n;
+	unsigned long pagesize = getpagesize();
+	struct kvm_swap swapary[1];
+
+	*retavail = 0;
+	*retfree = 0;
+
+#define	CONVERT(v)	((quad_t)(v) * (pagesize / 1024))
+
+	n = kvm_getswapinfo(kd, swapary, 1, 0);
+	if (n < 0 || swapary[0].ksw_total == 0) {
+		return 0;
+	}
+
+	*retavail = CONVERT(swapary[0].ksw_total);
+	*retfree = CONVERT(swapary[0].ksw_total - swapary[0].ksw_used);
+
+	n = (int) ((double) swapary[0].ksw_used * 100.0 /
+		(double) swapary[0].ksw_total);
+
+	return n;
+}
+
+void prepare_update(void)
+{
+}
+
+int update_uptime(void)
+{
+	int mib[2] = { CTL_KERN, KERN_BOOTTIME };
+	struct timeval boottime;
+	time_t now;
+	size_t size = sizeof(boottime);
+
+	if ((sysctl(mib, 2, &boottime, &size, NULL, 0) != -1)
+			&& (boottime.tv_sec != 0)) {
+		time(&now);
+		info.uptime = now - boottime.tv_sec;
+	} else {
+		fprintf(stderr, "Could not get uptime\n");
+		info.uptime = 0;
+	}
+
+	return 0;
+}
+
+int check_mount(char *s)
+{
+	struct statfs *mntbuf;
+	int i, mntsize;
+
+	mntsize = getmntinfo(&mntbuf, MNT_NOWAIT);
+	for (i = mntsize - 1; i >= 0; i--) {
+		if (strcmp(mntbuf[i].f_mntonname, s) == 0) {
+			return 1;
+		}
+	}
+
+	return 0;
+}
+
+int update_meminfo(void)
+{
+	u_int total_pages, inactive_pages, free_pages;
+	unsigned long swap_avail, swap_free;
+
+	int pagesize = getpagesize();
+
+	if (GETSYSCTL("vm.stats.vm.v_page_count", total_pages)) {
+		fprintf(stderr, "Cannot read sysctl \"vm.stats.vm.v_page_count\"\n");
+	}
+
+	if (GETSYSCTL("vm.stats.vm.v_free_count", free_pages)) {
+		fprintf(stderr, "Cannot read sysctl \"vm.stats.vm.v_free_count\"\n");
+	}
+
+	if (GETSYSCTL("vm.stats.vm.v_inactive_count", inactive_pages)) {
+		fprintf(stderr, "Cannot read sysctl \"vm.stats.vm.v_inactive_count\"\n");
+	}
+
+	info.memmax = total_pages * (pagesize >> 10);
+	info.mem = (total_pages - free_pages - inactive_pages) * (pagesize >> 10);
+	info.memeasyfree = info.memfree = info.memmax - info.mem;
+
+	if ((swapmode(&swap_avail, &swap_free)) >= 0) {
+		info.swapmax = swap_avail;
+		info.swap = (swap_avail - swap_free);
+		info.swapfree = swap_free;
+	} else {
+		info.swapmax = 0;
+		info.swap = 0;
+		info.swapfree = 0;
+	}
+
+	return 0;
+}
+
+int update_net_stats(void)
+{
+	struct net_stat *ns;
+	double delta;
+	long long r, t, last_recv, last_trans;
+	struct ifaddrs *ifap, *ifa;
+	struct if_data *ifd;
+
+	/* get delta */
+	delta = current_update_time - last_update_time;
+	if (delta <= 0.0001) {
+		return 0;
+	}
+
+	if (getifaddrs(&ifap) < 0) {
+		return 0;
+	}
+
+	for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
+		ns = get_net_stat((const char *) ifa->ifa_name, NULL, NULL);
+
+		if (ifa->ifa_flags & IFF_UP) {
+			struct ifaddrs *iftmp;
+
+			ns->up = 1;
+			last_recv = ns->recv;
+			last_trans = ns->trans;
+
+			if (ifa->ifa_addr->sa_family != AF_LINK) {
+				continue;
+			}
+
+			for (iftmp = ifa->ifa_next;
+					iftmp != NULL && strcmp(ifa->ifa_name, iftmp->ifa_name) == 0;
+					iftmp = iftmp->ifa_next) {
+				if (iftmp->ifa_addr->sa_family == AF_INET) {
+					memcpy(&(ns->addr), iftmp->ifa_addr,
+						iftmp->ifa_addr->sa_len);
+				}
+			}
+
+			ifd = (struct if_data *) ifa->ifa_data;
+			r = ifd->ifi_ibytes;
+			t = ifd->ifi_obytes;
+
+			if (r < ns->last_read_recv) {
+				ns->recv += ((long long) 4294967295U - ns->last_read_recv) + r;
+			} else {
+				ns->recv += (r - ns->last_read_recv);
+			}
+
+			ns->last_read_recv = r;
+
+			if (t < ns->last_read_trans) {
+				ns->trans += ((long long) 4294967295U -
+					ns->last_read_trans) + t;
+			} else {
+				ns->trans += (t - ns->last_read_trans);
+			}
+
+			ns->last_read_trans = t;
+
+			/* calculate speeds */
+			ns->recv_speed = (ns->recv - last_recv) / delta;
+			ns->trans_speed = (ns->trans - last_trans) / delta;
+		} else {
+			ns->up = 0;
+		}
+	}
+
+	freeifaddrs(ifap);
+	return 0;
+}
+
+int update_total_processes(void)
+{
+	int n_processes;
+
+	pthread_mutex_lock(&kvm_proc_mutex);
+	kvm_getprocs(kd, KERN_PROC_ALL, 0, &n_processes);
+	pthread_mutex_unlock(&kvm_proc_mutex);
+
+	info.procs = n_processes;
+	return 0;
+}
+
+int update_running_processes(void)
+{
+	struct kinfo_proc *p;
+	int n_processes;
+	int i, cnt = 0;
+
+	pthread_mutex_lock(&kvm_proc_mutex);
+	p = kvm_getprocs(kd, KERN_PROC_ALL, 0, &n_processes);
+	for (i = 0; i < n_processes; i++) {
+		if ((p[i].kp_stat == SACTIVE) &&
+		    (p[i].kp_lwp.kl_stat == LSRUN)) {
+			cnt++;
+		}
+	}
+	pthread_mutex_unlock(&kvm_proc_mutex);
+
+	info.run_procs = cnt;
+	return 0;
+}
+
+void get_cpu_count(void)
+{
+	int cpu_count = 0;
+
+	if (GETSYSCTL("hw.ncpu", cpu_count) == 0) {
+		info.cpu_count = cpu_count;
+	} else {
+		fprintf(stderr, "Cannot get hw.ncpu\n");
+		info.cpu_count = 0;
+	}
+
+	info.cpu_usage = malloc((info.cpu_count + 1) * sizeof(float));
+	if (info.cpu_usage == NULL) {
+		CRIT_ERR(NULL, NULL, "malloc");
+	}
+}
+
+struct cpu_info {
+	long oldtotal;
+	long oldused;
+};
+
+int update_cpu_usage(void)
+{
+	int i, j = 0;
+	long used, total;
+	long *cp_time = NULL;
+	size_t cp_len;
+	static struct cpu_info *cpu = NULL;
+	unsigned int malloc_cpu_size = 0;
+	extern void* global_cpu;
+
+	/* add check for !info.cpu_usage since that mem is freed on a SIGUSR1 */
+	if ((cpu_setup == 0) || (!info.cpu_usage)) {
+		get_cpu_count();
+		cpu_setup = 1;
+	}
+
+	if (!global_cpu) {
+		malloc_cpu_size = (info.cpu_count + 1) * sizeof(struct cpu_info);
+		cpu = malloc(malloc_cpu_size);
+		memset(cpu, 0, malloc_cpu_size);
+		global_cpu = cpu;
+	}
+
+	/* cpu[0] is overall stats, get it from separate sysctl */
+	cp_len = CPUSTATES * sizeof(long);
+	cp_time = malloc(cp_len);
+
+	if (sysctlbyname("kern.cp_time", cp_time, &cp_len, NULL, 0) < 0) {
+		fprintf(stderr, "Cannot get kern.cp_time\n");
+	}
+
+	total = 0;
+	for (j = 0; j < CPUSTATES; j++)
+		total += cp_time[j];
+
+	used = total - cp_time[CP_IDLE];
+
+	if ((total - cpu[0].oldtotal) != 0) {
+		info.cpu_usage[0] = ((double) (used - cpu[0].oldused)) /
+		(double) (total - cpu[0].oldtotal);
+	} else {
+		info.cpu_usage[0] = 0;
+	}
+
+	cpu[0].oldused = used;
+	cpu[0].oldtotal = total;
+
+	free(cp_time);
+
+	/* per-core stats */
+	cp_len = CPUSTATES * sizeof(long) * info.cpu_count;
+	cp_time = malloc(cp_len);
+
+	for (i = 0; i < info.cpu_count; i++)
+	{
+		total = 0;
+		for (j = 0; j < CPUSTATES; j++)
+			total += cp_time[i*CPUSTATES + j];
+
+		used = total - cp_time[i*CPUSTATES + CP_IDLE];
+
+		if ((total - cpu[i+1].oldtotal) != 0) {
+			info.cpu_usage[i+1] = ((double) (used - cpu[i+1].oldused)) /
+			(double) (total - cpu[i+1].oldtotal);
+		} else {
+			info.cpu_usage[i+1] = 0;
+		}
+
+		cpu[i+1].oldused = used;
+		cpu[i+1].oldtotal = total;
+	}
+
+	free(cp_time);
+	return 0;
+}
+
+int update_load_average(void)
+{
+	double v[3];
+
+	getloadavg(v, 3);
+
+	info.loadavg[0] = (double) v[0];
+	info.loadavg[1] = (double) v[1];
+	info.loadavg[2] = (double) v[2];
+
+	return 0;
+}
+
+double get_acpi_temperature(int fd)
+{
+	int temp;
+	(void)fd;
+
+	if (GETSYSCTL("hw.acpi.thermal.tz0.temperature", temp)) {
+		fprintf(stderr,
+			"Cannot read sysctl \"hw.acpi.thermal.tz0.temperature\"\n");
+		return 0.0;
+	}
+
+	return KELVTOC(temp);
+}
+
+static void get_battery_stats(int *battime, int *batcapacity, int *batstate, int *ac) {
+	if (battime && GETSYSCTL("hw.acpi.battery.time", *battime)) {
+		fprintf(stderr, "Cannot read sysctl \"hw.acpi.battery.time\"\n");
+	}
+	if (batcapacity && GETSYSCTL("hw.acpi.battery.life", *batcapacity)) {
+		fprintf(stderr, "Cannot read sysctl \"hw.acpi.battery.life\"\n");
+	}
+	if (batstate && GETSYSCTL("hw.acpi.battery.state", *batstate)) {
+		fprintf(stderr, "Cannot read sysctl \"hw.acpi.battery.state\"\n");
+	}
+	if (ac && GETSYSCTL("hw.acpi.acline", *ac)) {
+		fprintf(stderr, "Cannot read sysctl \"hw.acpi.acline\"\n");
+	}
+}
+
+void get_battery_stuff(char *buf, unsigned int n, const char *bat, int item)
+{
+	int battime, batcapacity, batstate, ac;
+	(void)bat;
+
+	get_battery_stats(&battime, &batcapacity, &batstate, &ac);
+
+	if (batstate != 1 && batstate != 2 && batstate != 0 && batstate != 7)
+		fprintf(stderr, "Unknown battery state %d!\n", batstate);
+	else if (batstate != 1 && ac == 0)
+		fprintf(stderr, "Battery charging while not on AC!\n");
+	else if (batstate == 1 && ac == 1)
+		fprintf(stderr, "Battery discharing while on AC!\n");
+
+	switch (item) {
+		case BATTERY_TIME:
+			if (batstate == 1 && battime != -1)
+				snprintf(buf, n, "%d:%2.2d", battime / 60, battime % 60);
+			break;
+		case BATTERY_STATUS:
+			if (batstate == 1) // Discharging
+				snprintf(buf, n, "remaining %d%%", batcapacity);
+			else
+				snprintf(buf, n, batstate == 2 ? "charging (%d%%)" :
+						(batstate == 7 ? "absent/on AC" : "charged (%d%%)"),
+						batcapacity);
+			break;
+		default:
+			fprintf(stderr, "Unknown requested battery stat %d\n", item);
+	}
+}
+
+static int check_bat(const char *bat)
+{
+	int batnum, numbatts;
+	char *endptr;
+	if (GETSYSCTL("hw.acpi.battery.units", numbatts)) {
+		fprintf(stderr, "Cannot read sysctl \"hw.acpi.battery.units\"\n");
+		return -1;
+	}
+	if (numbatts <= 0) {
+		fprintf(stderr, "No battery unit detected\n");
+		return -1;
+	}
+	if (!bat || (batnum = strtol(bat, &endptr, 10)) < 0 ||
+			bat == endptr || batnum > numbatts) {
+		fprintf(stderr, "Wrong battery unit %s requested\n", bat ? bat : "");
+		return -1;
+	}
+	return batnum;
+}
+
+int get_battery_perct(const char *bat)
+{
+	union acpi_battery_ioctl_arg battio;
+	int batnum, acpifd;
+	int designcap, lastfulcap, batperct;
+
+	if ((battio.unit = batnum = check_bat(bat)) < 0)
+		return 0;
+	if ((acpifd = open("/dev/acpi", O_RDONLY)) < 0) {
+		fprintf(stderr, "Can't open ACPI device\n");
+		return 0;
+	}
+	if (ioctl(acpifd, ACPIIO_BATT_GET_BIF, &battio) == -1) {
+		fprintf(stderr, "Unable to get info for battery unit %d\n", batnum);
+		return 0;
+	}
+	close(acpifd);
+	designcap = battio.bif.dcap;
+	lastfulcap = battio.bif.lfcap;
+	batperct = (designcap > 0 && lastfulcap > 0) ?
+		(int) (((float) lastfulcap / designcap) * 100) : 0;
+	return batperct > 100 ? 100 : batperct;
+}
+
+int get_battery_perct_bar(const char *bar)
+{
+	int batperct = get_battery_perct(bar);
+	return (int)(batperct * 2.56 - 1);
+}
+
+int open_acpi_temperature(const char *name)
+{
+	(void)name;
+	/* Not applicable for DragonFly. */
+	return 0;
+}
+
+void get_acpi_ac_adapter(char *p_client_buffer, size_t client_buffer_size, const char *adapter)
+{
+	int state;
+
+	(void) adapter; // only linux uses this
+
+	if (!p_client_buffer || client_buffer_size <= 0) {
+		return;
+	}
+
+	if (GETSYSCTL("hw.acpi.acline", state)) {
+		fprintf(stderr, "Cannot read sysctl \"hw.acpi.acline\"\n");
+		return;
+	}
+
+	if (state) {
+		strncpy(p_client_buffer, "Running on AC Power", client_buffer_size);
+	} else {
+		strncpy(p_client_buffer, "Running on battery", client_buffer_size);
+	}
+}
+
+void get_acpi_fan(char *p_client_buffer, size_t client_buffer_size)
+{
+	/* not implemented */
+	if (p_client_buffer && client_buffer_size > 0) {
+		memset(p_client_buffer, 0, client_buffer_size);
+	}
+}
+
+/* void */
+char get_freq(char *p_client_buffer, size_t client_buffer_size, const char *p_format,
+		int divisor, unsigned int cpu)
+{
+	int freq;
+	char *freq_sysctl;
+
+	freq_sysctl = (char *) calloc(16, sizeof(char));
+	if (freq_sysctl == NULL) {
+		exit(-1);
+	}
+
+	snprintf(freq_sysctl, 16, "hw.clockrate", (cpu - 1));
+
+	if (!p_client_buffer || client_buffer_size <= 0 || !p_format
+			|| divisor <= 0) {
+		return 0;
+	}
+
+	if (GETSYSCTL(freq_sysctl, freq) == 0) {
+		snprintf(p_client_buffer, client_buffer_size, p_format,
+			(float) freq / divisor);
+	} else {
+		snprintf(p_client_buffer, client_buffer_size, p_format, 0.0f);
+	}
+
+	free(freq_sysctl);
+	return 1;
+}
+
+int update_top(void)
+{
+	proc_find_top(info.cpu, info.memu, info.time);
+	return 0;
+}
+
+int update_diskio(void)
+{
+	/* XXX TODO */
+	return 0;
+}
+
+/* While topless is obviously better, top is also not bad. */
+
+int comparecpu(const void *a, const void *b)
+{
+	if (((const struct process *)a)->amount > ((const struct process *)b)->amount) {
+		return -1;
+	} else if (((const struct process *)a)->amount < ((const struct process *)b)->amount) {
+		return 1;
+	} else {
+		return 0;
+	}
+}
+
+int comparemem(const void *a, const void *b)
+{
+	if (((const struct process *)a)->rss > ((const struct process *)b)->rss) {
+		return -1;
+	} else if (((const struct process *)a)->rss < ((const struct process *)b)->rss) {
+		return 1;
+	} else {
+		return 0;
+	}
+}
+
+int comparetime(const void *va, const void *vb)
+{
+	struct process *a = (struct process *)va, *b = (struct process *)vb;
+
+	return b->total_cpu_time - a->total_cpu_time;
+}
+
+__attribute__((gnu_inline)) inline void
+proc_find_top(struct process **cpu, struct process **mem, struct process **time)
+{
+	struct kinfo_proc *p;
+	int n_processes;
+	int i, j = 0;
+	struct process *processes;
+
+	int total_pages;
+
+	/* we get total pages count again to be sure it is up to date */
+	if (GETSYSCTL("vm.stats.vm.v_page_count", total_pages) != 0) {
+		CRIT_ERR(NULL, NULL, "Cannot read sysctl \"vm.stats.vm.v_page_count\"");
+	}
+
+	pthread_mutex_lock(&kvm_proc_mutex);
+	p = kvm_getprocs(kd, KERN_PROC_ALL, 0, &n_processes);
+	processes = malloc(n_processes * sizeof(struct process));
+
+	for (i = 0; i < n_processes; i++) {
+		if (!((p[i].kp_flags & P_SYSTEM)) && p[i].kp_comm != NULL) {
+			processes[j].pid = p[i].kp_pid;
+			processes[j].name = strndup(p[i].kp_comm, text_buffer_size);
+			processes[j].amount = 100 * (float)(p[i].kp_lwp.kl_pctcpu) / FSCALE;
+			processes[j].vsize = p[i].kp_vm_map_size;
+			processes[j].rss = (p[i].kp_vm_rssize * getpagesize());
+			/* XXX for now show percentage of cpu time */
+			processes[j].total_cpu_time = p[i].kp_lwp.kl_pctcpu / 10000 * FSCALE;
+			j++;
+		}
+	}
+	pthread_mutex_unlock(&kvm_proc_mutex);
+
+	qsort(processes, j - 1, sizeof(struct process), comparemem);
+	for (i = 0; i < 10 && i < n_processes; i++) {
+		struct process *tmp, *ttmp;
+
+		tmp = malloc(sizeof(struct process));
+		memcpy(tmp, &processes[i], sizeof(struct process));
+		tmp->name = strndup(processes[i].name, text_buffer_size);
+
+		ttmp = mem[i];
+		mem[i] = tmp;
+		if (ttmp != NULL) {
+			free(ttmp->name);
+			free(ttmp);
+		}
+	}
+
+	qsort(processes, j - 1, sizeof(struct process), comparecpu);
+	for (i = 0; i < 10 && i < n_processes; i++) {
+		struct process *tmp, *ttmp;
+
+		tmp = malloc(sizeof(struct process));
+		memcpy(tmp, &processes[i], sizeof(struct process));
+		tmp->name = strndup(processes[i].name, text_buffer_size);
+
+		ttmp = cpu[i];
+		cpu[i] = tmp;
+		if (ttmp != NULL) {
+			free(ttmp->name);
+			free(ttmp);
+		}
+	}
+
+	qsort(processes, j - 1, sizeof(struct process), comparetime);
+	for (i = 0; i < 10 && i < n_processes; i++) {
+		struct process *tmp, *ttmp;
+
+		tmp = malloc(sizeof(struct process));
+		memcpy(tmp, &processes[i], sizeof(struct process));
+		tmp->name = strndup(processes[i].name, text_buffer_size);
+
+		ttmp = time[i];
+		time[i] = tmp;
+		if (ttmp != NULL) {
+			free(ttmp->name);
+			free(ttmp);
+		}
+	}
+
+	for (i = 0; i < j; i++) {
+		free(processes[i].name);
+	}
+	free(processes);
+}
+
+void get_battery_short_status(char *buffer, unsigned int n, const char *bat)
+{
+	get_battery_stuff(buffer, n, bat, BATTERY_STATUS);
+	if (0 == strncmp("charging", buffer, 8)) {
+		buffer[0] = 'C';
+		memmove(buffer + 1, buffer + 8, n - 8);
+	} else if (0 == strncmp("discharging", buffer, 11)) {
+		buffer[0] = 'D';
+		memmove(buffer + 1, buffer + 11, n - 11);
+	} else if (0 == strncmp("absent/on AC", buffer, 12)) {
+		buffer[0] = 'A';
+		memmove(buffer + 1, buffer + 12, n - 12);
+	}
+}
+
+/* dummies */
+int get_entropy_avail(unsigned int *val)
+{
+	(void)val;
+	return 1;
+}
+
+int get_entropy_poolsize(unsigned int *val)
+{
+	(void)val;
+	return 1;
+}

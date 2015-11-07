/*******************************************************************
* Project:	SystemInfo
*
* Author:	Kfiros (Kfir Shtober)
* Year:		2015	
*
* File:		systeminfo.c
* Description:	SystemInfo is a simple but useful cli utility,
*		which targets at providing information about the 
*		linux system it runs upon. The idea is based on the
*		systeminfo.exe binary, which appears on Windows
*		systems.
*******************************************************************/

/*******************************************************************
* Includes
*******************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/user.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/sysinfo.h>
#include <sys/socket.h>
#include <sys/statvfs.h>
#include <sys/time.h>
#include <utmp.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <linux/if_link.h>
#include <time.h>
#include "systeminfo.h"

/*******************************************************************
* Name: 	get_kernel_info()
* Description:	This function prints information about the currently
*		used linux kernel.
*******************************************************************/
static void get_kernel_info() {
	int call_rv;
	struct utsname uname_info;
	
	/* Get name and information about current kernel */
	call_rv = uname(&uname_info);
	if (SUCCESS != call_rv) {
		return;
	}

	fprintf(stdout, "%-30s%s\n", "Kernel Version:", uname_info.release);
	fprintf(stdout, "%-30s%s\n", "Compilation Info:", uname_info.version);
	fprintf(stdout, "%-30s%s\n", "System Type:", uname_info.machine);
	fprintf(stdout, "%-30s%s\n", "Hostname:", uname_info.nodename);
}

/*******************************************************************
* Name: 	get_os_release_info()
* Description:	This function prints information about the OS
*		release.
*******************************************************************/
static void get_os_release_info() {
	int call_rv;
	FILE * os_release_fd = NULL;
	char line[MAX_FILE_LINE] = {0};
	char var[MAX_FILE_VAR] = {0};
	char val[MAX_FILE_VAL] = {0};
	char * token = NULL;

	/* First obtain information saved in os-release file */
	os_release_fd = fopen(OS_RELEASE_PATH, "rb");
	
	if (NULL == os_release_fd) {
		return;
	}

	/* Iterate over the lines in os-release file */
	while (NULL != fgets(line, MAX_FILE_LINE, os_release_fd)) {

		/* Parse current line */
		call_rv = sscanf(line, "%[^=]=%[^\n]", var, val);
		if (OS_RELEASE_LINE_CELLS_COUNT != call_rv) {
			continue;
		}	
		
		call_rv = strncmp(var, OS_RELEASE_NAME, MAX_FILE_VAR);
		if (0 == call_rv) {
			token = strtok(val, "\"");
			if (NULL != token) {
				fprintf(stdout, "%-30s%s\n", "OS Name:", token);
			}
		}

		call_rv = strncmp(var, OS_RELEASE_VERSION, MAX_FILE_VAR);
		if (0 == call_rv) {
			token = strtok(val, "\"");
			if (NULL != token) {
				fprintf(stdout, "%-30s%s\n", "OS Version:", token);
			}
		}
	}

	if (NULL != os_release_fd) {
		fclose(os_release_fd);
	}
}	

/*******************************************************************
* Name: 	get_os_info
* Description:	This function obtains information related to the
*		running operating system.	
*******************************************************************/
static void get_os_info() {

	/* Get information about the OS release */
	get_os_release_info();

	/* Get information about current kernel */
	get_kernel_info();

}

/*******************************************************************
* Name: 	get_uptime
* Description:	This function prints the system's uptime.
*******************************************************************/
static void get_uptime(long uptime) {
	unsigned int days, hours, minutes;

	/* Get system's uptime */
	days = uptime / (60 * 60 * 24);
	hours = (uptime / 60 / 60) % 24;
	minutes = (uptime / 60) % 60;

	fprintf(stdout, "%-30sUp %d Days, %d Hours, %d Minutes \n", "Uptime:",
							days, hours, minutes);
	
}

/*******************************************************************
* Name: 	get_boot_time
* Description:	This function prints the system's boot time.
*******************************************************************/
static void get_boot_time() {
	FILE * stat_fd = NULL;
	time_t t_boot;
	char line[MAX_FILE_LINE] = {0};
	char * token = NULL;

	/* Get system's boot time from /proc/stat */
	stat_fd = fopen(STAT_PATH, "rb");	
	if (NULL == stat_fd) {
		return;
	}

	/* Iterate over the lines in /proc/stat file */
	while (NULL != fgets(line, MAX_FILE_LINE, stat_fd)) {
		token = strtok(line, SPACE_SEP);		
		if (NULL == token) {
			continue;
		}

		if (0 == strcmp(token, STAT_BTIME)) {
			token = strtok(NULL, SPACE_SEP);	
			break;
		}
	}
	
	if (NULL == token) {
		goto cleanup;
	}

	t_boot = atoi(token);
	fprintf(stdout, "%-30s%s", "System Boot Time:", ctime(&t_boot));
	
cleanup:
	if (NULL != stat_fd) {
		fclose(stat_fd);
	}

	return;
}

/*******************************************************************
* Name: 	get_free_space
* Description:	Calculates the ammount of available space in the
*		fs of the root directory.
*******************************************************************/
static void get_free_space() {
	int call_rv;
	struct statvfs stats = {0};
	size_t free_space;

	/* Get filesystem statistics */
	call_rv = statvfs(ROOT_DIR, &stats);
	if (SUCCESS != call_rv) {
		return;
	}

	/* Calculate the free space in MBs */
	free_space = (stats.f_bsize * stats.f_bavail) / (MB_SIZE);

	fprintf(stdout, "%-30s%zu MB\n", "Free Space:", free_space);

}

/*******************************************************************
* Name: 	get_sys_stats
* Description:	This function obtains general statistical information
*		about the system (Current date & time, uptime, free
*		ram, and so on)...
*******************************************************************/
void get_sys_stats() {
	int call_rv;
	struct sysinfo info = {0};
	time_t t_now;

	/* Get free space in the fs of the root directory */
	get_free_space();

	/* Get current date and time */
	t_now = time(NULL);
	fprintf(stdout, "%-30s%s", "Date:", ctime(&t_now));

	/* Get system's boot time */
	get_boot_time();

	/* Get overall system statistics */	
	call_rv = sysinfo(&info);
	if (SUCCESS != call_rv) {
		return;
	}

	/* Get system's uptime */	
	get_uptime(info.uptime);

	fprintf(stdout, "%-30s%d MB\n", "Total RAM:", (unsigned int) (info.totalram  / 1024) );
	fprintf(stdout, "%-30s%d MB\n", "Total Available RAM:", (unsigned int) info.freeram / 1024 );
	fprintf(stdout, "%-30s%d\n", "Number Of Processes:", info.procs );
	
}

/*******************************************************************
* Name: 	test_cpuinfo_line
* Description:	This function tests for known information in a given
*		cpuinfo line.
*******************************************************************/
static inline void test_cpuinfo_line(char * var, char * val) {
		if (0 == strncmp(var, CPUINFO_MODEL_NAME, MAX_FILE_VAR)) {
			fprintf(stdout, "%-30s%s\n", "Processor Model:", val);

		} else if (0 == strncmp(var, CPUINFO_VENDOR_ID, MAX_FILE_VAR)) {
			fprintf(stdout, "%-30s%s\n", "Vendor ID:", val);

		} else if (0 == strncmp(var, CPUINFO_CPU_FAMILY, MAX_FILE_VAR)) {
			fprintf(stdout, "%-30s%s\n", "CPU Family:", val);

		} else if (0 == strncmp(var, CPUINFO_STEPPING, MAX_FILE_VAR)) {
			fprintf(stdout, "%-30s%s\n", "CPU Stepping:", val);

		} else if (0 == strncmp(var, CPUINFO_CPU_CORES, MAX_FILE_VAR)) {
			fprintf(stdout, "%-30s%s\n", "CPU Cores:", val);

		} else if (0 == strncmp(var, CPUINFO_CPU_MHZ, MAX_FILE_VAR)) {
			fprintf(stdout, "%-30s%s\n", "CPU MHz:", val);

		} 
}

/*******************************************************************
* Name: 	get_cpu_info
* Description:	This function obtains important details about the
*		CPU and prints it.
*******************************************************************/
static void get_cpu_info() {
	int call_rv;
	FILE * cpuinfo_fd = NULL;
	char line[MAX_FILE_LINE] = {0};
	char var[MAX_FILE_VAR] = {0};
	char val[MAX_FILE_VAL] = {0};
	
	cpuinfo_fd = fopen(CPUINFO_PATH, "rb");
	if (NULL == cpuinfo_fd) {
		return;
	}
	
	/* Iterate over the lines in cpuinfo file */
	while(NULL != fgets(line, MAX_FILE_LINE, cpuinfo_fd)) {

		/* Parse given line in the cpuinfo file */
		call_rv = sscanf(line, "%[^\t]\t: %[^\n]", var, val);
		if (CPUINFO_LINE_CELLS_COUNT != call_rv) {
			continue;
		}
		
		test_cpuinfo_line(var, val);	
		
	}

	if (NULL != cpuinfo_fd) {
		fclose(cpuinfo_fd);
	}

	return;
}

/*******************************************************************
* Name: 	get_ip_addrs
* Description:	Obtains the IP addresses assigned to the system's
		network interfaces.
*******************************************************************/
void get_ip_addrs() {
	int call_rv;
	struct ifaddrs * ifaddr = NULL, * ifa = NULL;
	int family;	
	char host[NI_MAXHOST] = {0};

	/* Get interface addresses */
	call_rv = getifaddrs(&ifaddr);
	if (SUCCESS != call_rv) {
		return;
	}
	
	fprintf(stdout, "%-30s\n", "Interfaces:");

	/* Iterating over the addresses */
	for (ifa = ifaddr; NULL != ifa; ifa = ifa->ifa_next) {

		if (NULL == ifa->ifa_addr) {
			continue;
		}

		family = ifa->ifa_addr->sa_family;

		/* Check for IPv4 & IPv6 addresses only */
		if (AF_INET == family || AF_INET6 == family){

			/* Get appropriate IP address */
			call_rv = getnameinfo(ifa->ifa_addr,
					(AF_INET == family) ?	sizeof(struct sockaddr_in) :
								sizeof(struct sockaddr_in6),
					host, NI_MAXHOST, NULL, 0, NI_NUMERICHOST);

			if (SUCCESS != call_rv) {
				continue;
			}

			fprintf(stdout, "%-30s%s (%s) %s\n", "", ifa->ifa_name,
							(AF_INET == family) ? "IPv4" : "IPv6", host);
		}				
		
	}
	
	/* Free getifaddrs' allocated data */
	freeifaddrs(ifaddr);
}



/*******************************************************************
* Name: 	main
* Description:	Main function of the program
*******************************************************************/
int main(int argc, char * argv[]) {

	/* Eliminate the `unused` warnings */
	UNUSED(argc);
	UNUSED(argv);

	fprintf(stdout, "[*] SystemInfo (Kfiros 2015)\n");
	fprintf(stdout, "[*] Obtaining system information... \n");

	/* Obtain system information. On any error - continue... */
	get_os_info();
	get_sys_stats();
	get_cpu_info();
	get_ip_addrs();

	return SUCCESS;
}


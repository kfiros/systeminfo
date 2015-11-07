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

#ifndef __SYSTEMINFO_H__
#define __SYSTEMINFO_H__

/*******************************************************************
* Constants & Macros
*******************************************************************/

#define UNUSED(param) do { (void) (param); } while (0);

typedef enum {
	false = 0,
	true,
} bool;

enum errors {
	ERROR = -1,
	SUCCESS = 0,
	ERR_PACKAGE_NOT_FOUND = 1000,
	
};

#define MB_SIZE (1024 * 1024)
#define MAX_FILE_LINE (255)
#define MAX_FILE_VAR (50)
#define MAX_FILE_VAL (MAX_FILE_LINE - MAX_FILE_VAR)
#define ROOT_DIR ("/")

/* get_boot_time() */ 
#define SPACE_SEP (" ")
#define STAT_PATH ("/proc/stat")
#define STAT_BTIME ("btime")

/* get_os_release_info() */ 
#define OS_RELEASE_PATH ("/etc/os-release")
#define OS_RELEASE_LINE_CELLS_COUNT (2)
#define OS_RELEASE_NAME ("NAME")
#define OS_RELEASE_VERSION ("VERSION")

/* get_cpu_info() */
#define CPUINFO_PATH ("/proc/cpuinfo")
#define CPUINFO_LINE_CELLS_COUNT (2)
#define CPUINFO_MODEL_NAME ("model name")
#define CPUINFO_VENDOR_ID ("vendor_id")
#define CPUINFO_CPU_FAMILY ("cpu family")
#define CPUINFO_STEPPING ("stepping")
#define CPUINFO_CPU_CORES ("cpu cores")
#define CPUINFO_CPU_MHZ ("cpu MHz")

#endif /* __SYSTEMINFO_H__*/

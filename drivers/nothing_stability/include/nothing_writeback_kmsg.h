#ifndef NOTHING_WRITEBACK_KMSG_H
#define NOTHING_WRITEBACK_KMSG_H

#include <linux/printk.h>
#include <linux/version.h>

#define NT_rkl_info_print(fmt,arg...) \
pr_info("[NT_reserve_kernel_log] "fmt, ##arg)

#define NT_rkl_err_print(fmt,arg...) \
pr_err("[NT_reserve_kernel_log] "fmt, ##arg)

#define RETRY_COUNT_FOR_GET_DEV_T 60
#define PER_LOOP_MS 30
#define NT_MAJOR 14
#define NT_MINOR 7
#define NT_K_MULT 1024
#define NT_M_MULT 1048576
#define NT_64M_MULT (64 * NT_M_MULT)
#define NT_512M_MULT (512 * NT_M_MULT)
#define PREFIX_BUFFER_SIZE 32
#define NT_PAGE_SIZE 4096
#define BOOT_LOG_SIZE (2048 * NT_K_MULT) //per boot log size
#define MAX_BOOT_LOG_COUNT 16 // The boot log can record 30 kmsg logs.
#define BOOT_LOG_PAGES (BOOT_LOG_SIZE / NT_PAGE_SIZE) //per boot log size
#define BOOT_LOG_SIZE_OF_LINE 2048
#define COMPUTE_BOOT_LOG_OFFSET(x) ((NT_BOOT_KMSG_LOG_OFFSET)+(((x)%(MAX_BOOT_LOG_COUNT))*(BOOT_LOG_SIZE)))
/* ex.
 * boot_count = 0 start offset is NT_BOOT_KMSG_LOG_OFFSET + 0
 * boot_count = 1 start offset is NT_BOOT_KMSG_LOG_OFFSET + 512K
 * boot_count = 2 start offset is NT_BOOT_KMSG_LOG_OFFSET + 1024K
 * ....etc
 * //cycle run , boot_count = 32 equal boot_count = 0
 * boot_count = 32 start offset is NT_BOOT_KMSG_LOG_OFFSET + 0
 * boot_count = 33 start offset is NT_BOOT_KMSG_LOG_OFFSET + 512K
 * boot_count = 34 start offset is NT_BOOT_KMSG_LOG_OFFSET + 1024K
 */
#define PANIC_LOG_SIZE (256 * NT_K_MULT) //per panic log size, kmsg size 256K default in kernel
#define MAX_PANIC_LOG_COUNT 16 // The panic log can record 16 kmsg logs.
#define COMPUTE_PANIC_LOG_OFFSET(x) ((NT_PANIC_KMSG_LOG_OFFSET)+(((x)%(MAX_PANIC_LOG_COUNT))*(PANIC_LOG_SIZE)))
/* ex. panic_count = 0 start offset is NT_PANIC_KMSG_LOG_OFFSET + 0
 * panic_count = 1 start offset is NT_PANIC_KMSG_LOG_OFFSET + 256K
 * panic_count = 2 start offset is NT_PANIC_KMSG_LOG_OFFSET + 512K....etc
 */
#define NT_MAGIC_SIZE 16 // need follow 2^n
#define NT_KMSG_PARTITION_LABEL "PARTLABEL=nt_kmsg"
#define NT_UEFI_PARTITION_LABEL "PARTLABEL=nt_uefi"
#define LOGDUMP_PARTITION_LABEL "PARTLABEL=logdump"

/* offset list, total 64m */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(5, 10, 0))
#define NT_RESERVE_N_HEADER_OFFSET 0
#define NT_RESERVE_N_HEADER_SIZE (1 * NT_M_MULT)
#define NT_BOOT_KMSG_LOG_OFFSET (NT_RESERVE_N_HEADER_OFFSET + NT_RESERVE_N_HEADER_SIZE)
#define NT_BOOT_KMSG_LOG_SIZE (63 * NT_M_MULT)
#define NT_CHECK_LAYOUT_END_OFFSET (NT_BOOT_KMSG_LOG_OFFSET + NT_BOOT_KMSG_LOG_SIZE)
#define NT_MAGIC_SIZE 16 // need follow 2^n
/* offset list end, total 64m*/
#else
/* offset list, total 512m */
#define NT_MINIDUMP_OFFSET 0
#define NT_MINIDUMP_SIZE (256 * NT_M_MULT)
#define NT_AP_LOG_OFFSET (NT_MINIDUMP_OFFSET + NT_MINIDUMP_SIZE)
#define NT_AP_LOG_SIZE (128 * NT_M_MULT)
#define NT_RESERVE_N_HEADER_OFFSET (NT_AP_LOG_OFFSET + NT_AP_LOG_SIZE)
#define NT_RESERVE_N_HEADER_SIZE (32 * NT_M_MULT)
#define NT_PANIC_KMSG_LOG_OFFSET (NT_RESERVE_N_HEADER_OFFSET + NT_RESERVE_N_HEADER_SIZE)
#define NT_PANIC_KMSG_LOG_SIZE (32 * NT_M_MULT)
#define NT_BOOT_KMSG_LOG_OFFSET (NT_PANIC_KMSG_LOG_OFFSET + NT_PANIC_KMSG_LOG_SIZE)
#define NT_BOOT_KMSG_LOG_SIZE (32 * NT_M_MULT)
#define NT_BOOTLOADER_LOG_OFFSET (NT_BOOT_KMSG_LOG_OFFSET + NT_BOOT_KMSG_LOG_SIZE)
#define NT_BOOTLOADER_LOG_SIZE (32 * NT_M_MULT)
#define NT_CHECK_LAYOUT_END_OFFSET (NT_BOOTLOADER_LOG_OFFSET + NT_BOOTLOADER_LOG_SIZE)
/* offset list end, total 512m*/
#endif

const char* NT_LOG_MAGIC = "NOTING_RKL_V0";
/* If someone needs to modify the NT_reserve_kernel_log_header struct.
 * you also need to change the NT_LOG_MAGIC!
 * ex. NOTING_IS_RKL_V0 -> NOTING_IS_RKL_V1
 * This means that the header needs to be "re-initialized"
 */

typedef struct
{
	char magic[NT_MAGIC_SIZE];
	unsigned char boot_count;
	unsigned char panic_count;
	unsigned char last_reboot_is_panic;
	unsigned char last_boot_is_fail;
	unsigned char bootloader_count;
} NT_reserve_log_header; //need align Bootloaderlogging dxe.

typedef enum {
	NT_COMPARE_HEADER_MAGIC,
	NT_RESET_HEADER,
	NT_ADD_BOOT_COUNT,
	NT_ADD_PANIC_COUNT,
	NT_SET_LAST_BOOT_IS_FAILED,
	NT_SET_LAST_REBOOT_IS_PANIC,
} NT_reserve_kernel_log_header_actions;

typedef struct
{
	u32 sig;
	u32 offset;
	u32 reserve_flag[11];
} lk_log_emmc_header;

typedef enum {
	UART_LOG = 0x00,
	LOG_INDEX = 0X01,
	PRINTK_RATELIMIT = 0X02,
	KEDUMP_CTL = 0x03,
	BOOT_STEP = 0x04,
	EMMC_STORE_FLAG_TYPE_NR,
} LK_EMMC_STORE_FLAG_TYPE;

#endif /* NOTHING_WRITEBACK_KMSG_H */

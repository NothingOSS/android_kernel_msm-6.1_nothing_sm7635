#ifndef NOTHING_CHECK_PARTS_H
#define NOTHING_CHECK_PARTS_H

#include <linux/printk.h>

#define NT_partition_info_print(fmt,arg...) \
pr_info("[nt_partition_info] "fmt, ##arg)
#define NT_partition_err_print(fmt,arg...) \
pr_err("[nt_partition_info] "fmt, ##arg)
#define MAX_STRING_LENGTH 512

struct nt_hash_cell {
	struct rb_node name_node;
	struct rb_node uuid_node;
	bool name_set;
	bool uuid_set;

	char *name;
	char *uuid;
	struct mapped_device *md;
	struct dm_table *new_map;
};

#endif /* NOTHING_CHECK_PARTS_H */

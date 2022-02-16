#include <linux/fs.h> 
#include <linux/init.h> 
#include <linux/kobject.h> 
#include <linux/module.h> 
#include <linux/string.h> 
#include <linux/sysfs.h>

static struct kobject *testmodule; 
static int variable = 0;

static ssize_t variable_show(struct kobject *kobj, 
                               struct kobj_attribute *attr, char *buf) { 
    return sprintf(buf, "%d\n", variable); 
} 
 
static ssize_t variable_store(struct kobject *kobj, 
                                struct kobj_attribute *attr, char *buf, 
                                size_t count) { 
    sscanf(buf, "%du", &variable); 
    return count; 
} 
 
static struct kobj_attribute variable_attribute = 
    __ATTR(variable, 0660, variable_show, (void *)variable_store); 

static int __init testmodule_init(void) {
    int ret = 0;
    testmodule = kobject_create_and_add("testmodule", kernel_kobj);

    if (!mymodule) {
        return -ENOMEM;
    } 
    ret = sysfs_create_file(testmodule, &variable_attribute.attr); 

    if (ret) { 
        pr_info("failed to create the variable file " 
                "in /sys/kernel/testmodule\n"); 
    }

    return ret;
}

static void __exit testmodule_exit(void) {
    kobject_put(testmodule); 
}

module_init(testmodule_init); 
module_exit(testmodule_exit); 
 
MODULE_LICENSE("GPL");
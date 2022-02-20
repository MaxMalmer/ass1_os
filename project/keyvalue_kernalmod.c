/*
 * Attempt by Oscar and Max to create a Linux Kernel Module that implements primitive key-value storage.
 * Code and ideas are based on The Linux Kernel Module Programming Guide
 *
 * It currently only works for keys of the type char and values of type int and does not have good error handling. 
 * It's also a bit annoying to work with but this was the simplest way we came up with sadly.
 *
 * To communicated with the module, compile it, insmod it then all interactions take place at /sys/kernel/testmodule/variable
 * To stora data echo "I X VAL" to /sys/kernel/testmodule/variable where X stands for the char key you want and VAL stans for a number (123)
 * To read data, first prepare the module by setting its read mode.
 * 	- echo "P" to tell the module to read the entire storage
 * 	- echo "R X" to tell the module to read the value of variable X
 * Then simply read the value by typing cat /sys/kernel/testmodule/variable
*/

#include <linux/fs.h> 
#include <linux/init.h> 
#include <linux/kobject.h> 
#include <linux/module.h> 
#include <linux/string.h> 
#include <linux/sysfs.h>
#include <linux/slab.h>
#include <linux/mutex.h>

static struct kobject *testmodule; 
static int variable = 0;
static int single = 0;
static char valueptr;

static DEFINE_MUTEX(lock);

struct data {
	char key;
	int data;
	struct data* next;
};

static struct data* head = NULL;

static ssize_t variable_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf) { 
	mutex_lock(&lock);

	if (!single) {

		char tempbuffer[50];
		struct data* temp;
		temp = head;
		while(temp != NULL) {	
			sprintf(tempbuffer, "ENTRY %c:%d\n", temp->key, temp->data);
			strcat(buf, tempbuffer); 
			temp = temp->next;
		}
		mutex_unlock(&lock);
		return strlen(buf);
	} else {
		struct data* temp;
		temp = head;
		while(temp != NULL) {
			if (temp->key == valueptr) {
				sprintf(buf, "%d\n", temp->data);
				mutex_unlock(&lock);
				return strlen(buf);
			}
			temp = temp->next;
		}
		sprintf(buf, "NO DATA FOR %c STORED. PLEASE STOP HARASSING ME\n", valueptr);
		mutex_unlock(&lock);
		return strlen(buf);
	}
} 
 
static ssize_t variable_store(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count) { 
	char key;
	int value;
	struct data* temp;
	mutex_lock(&lock);

	switch (buf[0]) {

		case 'I':
			key = buf[2];
			sscanf(buf + 4, "%d", &value);

			if (head == NULL) {
				head = kmalloc(sizeof(struct data), 0);
				head->key = key;
				head->data = value;
			} else if (head->key == key) {
				head->data = value;	
			} else {
				temp = head;

				while (temp->next != NULL) {

					if (temp->key == key) {
						temp->data = value;
						mutex_unlock(&lock);
						return count;	
					}
					temp = temp->next;
				}
				
				if (temp->key == key) {
					temp->data = value;
				} else {
					temp->next = kmalloc(sizeof(struct data), 0);
					temp->next->key = key;
					temp->next->data = value;
				}

			}	
			pr_info("INSERT\n");
			break;
		case 'R':
			single = 1;
			sscanf(buf + 2, "%c", &valueptr);
			break;
		case 'P':
			single = 0;
			break;
		default:
			pr_info("uh ohhhh\n");
			break;
	}
    sscanf(buf + 1, "%du", &variable);
	mutex_unlock(&lock); 
    return count; 
} 
 
static struct kobj_attribute variable_attribute = 
    __ATTR(variable, 0660, variable_show, (void *)variable_store); 

static int __init testmodule_init(void) {
    int ret = 0;
    testmodule = kobject_create_and_add("testmodule", kernel_kobj);

    if (!testmodule) {
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

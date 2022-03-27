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
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/udp.h>
#include <linux/netpoll.h>
#include <asm/segment.h>
#include <asm/uaccess.h>
#include <linux/buffer_head.h>

static struct netpoll* np = NULL;
static struct netpoll np_t;

static struct kobject *testmodule; 
static int variable = 0;
static int single = 0;
static char valueptr;

static struct nf_hook_ops* netfilterhook = NULL;

static DEFINE_MUTEX(lock);

struct data {
	char key;
	int data;
	struct data* next;
};

static struct data* head = NULL;

static struct file* fp = NULL;

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


static struct file *file_open(const char *path, int flags, int rights) {
    struct file *filp = NULL;
    mm_segment_t oldfs;
    int err = 0;

    filp = filp_open(path, flags, rights);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
	pr_info("Error: %d", err);
        return NULL;
    }
    return filp;
}

static void file_close(struct file *file) {
    filp_close(file, NULL);
}








static int file_sync(struct file *file) {
    vfs_fsync(file, 0);
    return 0;
}

static int insert_key(char key, int value) {
	struct data* temp;

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
				return 1;	
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
	return 0;
}

static void parse_command(char* buf, size_t len) {
	char key;
	int value;
	int returnval;
	loff_t pos = 0;
	char buffer[50] = "hello i am big noob";
	struct data* temp;
	char linelength = 0;
	mutex_lock(&lock);
	switch (buf[0]) {
		case 'I':
			key = buf[2];
			sscanf(buf + 4, "%d", &value);
			returnval = insert_key(key, value);
			if (returnval) { mutex_unlock(&lock); return; }
			break;
		case 'R':
			single = 1;
			sscanf(buf + 2, "%c", &valueptr);
			break;
		case 'P':
			single = 0;
			break;
		case 'S':
			pr_info("Trying to open file\n");
			fp = file_open("testfile.txt", O_RDWR|O_CREAT|O_TRUNC, 0);
			if (fp != NULL) {
				temp = head;
				while(temp != NULL) {	
					sprintf(buffer, "%c:%d\n", temp->key, temp->data);
					linelength = (char)strlen(buffer);
					kernel_write(fp, &linelength, 1, &pos);
					kernel_write(fp, buffer, strlen(buffer), &pos);
					temp = temp->next;
				}
				linelength = 'Q';
				kernel_write(fp, &linelength, 1, &pos);
				pr_info("Trying to close file\n");
				file_close(fp);
			}
			pr_info("Exiting file stuffs\n");
			break;
		case 'L':
			pr_info("Trying to load file\n");
			fp = file_open("testfile.txt", O_RDWR, 0);
			if (fp != NULL) {
				kernel_read(fp, buffer, 1, &pos);
				while(buffer[0] != 'Q') {	
					linelength = (int)buffer[0];
					kernel_read(fp, buffer, linelength, &pos);
					buffer[linelength] = '\0';
					key = buffer[0];
					sscanf(buffer + 2, "%d", &value);
					insert_key(key, value);
					printk("Buffer: %s", buffer);
					kernel_read(fp, buffer, 1, &pos);
				}
				pr_info("Trying to close loaded file\n");
				file_close(fp);
			}
			pr_info("Exiting file stuffs\n");
			break;
		default:
			pr_info("INVALID COMMAND\n");

	
	}
	sscanf(buf + 1, "%du", &variable); 
	mutex_unlock(&lock);
	return;
}

static ssize_t variable_store(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count) {
	parse_command(buf, count);
	return count; 
} 
 
static struct kobj_attribute variable_attribute = 
    __ATTR(variable, 0660, variable_show, (void *)variable_store); 

static unsigned int netfilterfunc(void* priv, struct sk_buff* packet, const struct nf_hook_state* state) {
	struct iphdr* iph;
	struct udphdr* udph;
	unsigned char* headerend;
	int databytes;
	char buffer[100];
	int i = 0;
	char c;
	if (!packet) { return NF_ACCEPT; }
	
	iph = ip_hdr(packet);
	if (iph->protocol == IPPROTO_UDP) {
		udph = udp_hdr(packet);
		if (ntohs(udph->dest) == 5555) {
			databytes = ntohs(udph->len) - 8;
			headerend = (unsigned char*)udph;
			headerend += 8; //size of udp header
			strncpy(buffer, headerend, databytes);
		        while ((*headerend) != '\0' && i < databytes) {
				c = *(char*)headerend;
				printk("%c", c);
				headerend++;
				i++;
			}
			parse_command(buffer, databytes);
			pr_info("correct port!!!\n");
			return NF_ACCEPT;
		}
	}
	return NF_ACCEPT;


}



//it fakkin worrkkkss!!!!
static void init_netpoll(void)
{
    np_t.name = "LRNG";
    strlcpy(np_t.dev_name, "enp0s8", IFNAMSIZ);
    np_t.local_ip.ip = htonl((unsigned long int)0x0a00030f); //10.0.3.15
    np_t.local_ip.in.s_addr = htonl((unsigned long int)0x0a00030f); //same
    np_t.remote_ip.ip = htonl((unsigned long int)0x82efb21e); //130.239.178.30
    np_t.remote_ip.in.s_addr = htonl((unsigned long int)0x82efb21e); //same
    np_t.ipv6 = 0;//no IPv6
    np_t.local_port = 5555;
    np_t.remote_port = 11111;
    memset(np_t.remote_mac, 0xff, ETH_ALEN);
    netpoll_print_options(&np_t);
    netpoll_setup(&np_t);
    np = &np_t;
}

static void sendUDP(const char* buf) {
	int len = strlen(buf);
	netpoll_send_udp(np, buf, len);
}






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


    netfilterhook = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);

    netfilterhook->hook = (nf_hookfn*)netfilterfunc;
    netfilterhook->hooknum = NF_INET_PRE_ROUTING;
    netfilterhook->pf = PF_INET;
    netfilterhook->priority = NF_IP_PRI_FIRST;

    nf_register_net_hook(&init_net, netfilterhook);
	printk("all well so far...");
    init_netpoll();
    printk("going swell...");
    sendUDP("testing....");
    printk("wooot");


    return ret;
}

static void __exit testmodule_exit(void) {
    kobject_put(testmodule);
    nf_unregister_net_hook(&init_net, netfilterhook);
    kfree(netfilterhook); 
}

module_init(testmodule_init); 
module_exit(testmodule_exit); 
 
MODULE_LICENSE("GPL");

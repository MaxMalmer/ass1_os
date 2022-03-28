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
 *
 * Data is automatically saved in a file called datafile at the same place the module is located.
 * This stores data in the format X:N where X is the key and N is the value. Pairs are delimited by a newline.
 *
 * The program uses a single mutex to lock all access to datastructures, probably not the best design but works well enough for this small project.
 *
 * Access can also be granted to the module from other computers. This is achieved by having the module look at all incoming packets (not the most efficient...), 
 * figuring out if they are sent to this module by looking at the port and message contents.
 *
 * Debug messages can also be sent to another computer by using send_debug().
 * The address for the debug messages has to be configured beforehand using init_netpoll().
 *
 *
 * **NOTE** This project has made heavy use of sites like stackoverflow and other websites.
 * It felt like we had to do a bit too much googling during this assignment but i'm not sure how else we were supposed to do it. 
 * We're not sure we remember all of them but here are atleast some of the references we used. 
 * https://sysprog21.github.io/lkmpg/ - Generell help with lkm's.
 * https://stackoverflow.com/questions/1184274/read-write-files-within-a-linux-kernel-module - Got us started with file access.
 * https://stackoverflow.com/questions/20431620/using-netpoll-on-kernel-3-10 - Got us started with netpoll
 * https://infosecwriteups.com/linux-kernel-communication-part-1-netfilter-hooks-15c07a5a5c4e - Got us started with netfilter.
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

/* The program contains a lot of global variables, probably more than neccessary.
 * However we felt it was easier to do development this way given that so many ways of communicating with the kernel seemed to need global varaibles.
 */

static struct netpoll* np = NULL;
static struct netpoll np_t;
static struct nf_hook_ops* netfilterhook = NULL;

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
static struct file* fp = NULL;

/* Function called when reading the file at sys/kernel/testmodule/variable 
 * Used to display information about the key-value store depending on its current state.
 * */
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

/*
 * Opens a file + does error handling. Returns NULL on error.
 */
static struct file *file_open(const char *path, int flags) {
    struct file *filp = NULL;
    int err = 0;

    filp = filp_open(path, flags, 0);
    if (IS_ERR(filp)) {
        err = PTR_ERR(filp);
        return NULL;
    }
    return filp;
}

/* Helper function to close a file, not really needed... */
static void file_close(struct file *file) {
    filp_close(file, NULL);
}

/* Inserts a key-value pair into the store, does this in the simplest way possible.
 * **NOTE** This function allocated memory via kmalloc 
 */
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
	return 0;
}

/* Parses the command sent to the lkm. More details at the top of the program. */
static void parse_command(char* buf, size_t len) {
	char key;
	int value;
	int returnval;
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
		default:
			pr_info("INVALID COMMAND\n");
	}
	sscanf(buf + 1, "%du", &variable); 
	mutex_unlock(&lock);
	return;
}

/* Called when data gets sent into sys/kernel/testmodule/variable. buf contains the data sent. */
static ssize_t variable_store(struct kobject *kobj, struct kobj_attribute *attr, char *buf, size_t count) {
	parse_command(buf, count);
	return count; 
} 
 
static struct kobj_attribute variable_attribute = 
    __ATTR(variable, 0660, variable_show, (void *)variable_store); 

/* 
 * Function that is called everytime the kernel gets a incoming packet.
 * If the packet is UDP and sent to port 5555 it prints the contents out and parses it as a normal command.
 * Otherwise / then it passes the packet further into the kernel.
 */
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


/* 
 * Init netpoll, sets everything up so debug messages can simply be sent using send_debug. 
 * Lots of values are hardcoded to work with this machine and its VM.
 *
 * TODO: Change hardcoded values to options? Not sure how to deal with dev_name dynamically though. 
 */
static void init_netpoll(void)
{
	np_t.name = "LRNG";
	strlcpy(np_t.dev_name, "enp0s8", IFNAMSIZ);
	np_t.local_ip.ip = htonl((unsigned long int)0x0a00030f); //10.0.3.15
	np_t.local_ip.in.s_addr = htonl((unsigned long int)0x0a00030f); //same
	np_t.remote_ip.ip = htonl((unsigned long int)0x82efb21e); //130.239.178.30
	np_t.remote_ip.in.s_addr = htonl((unsigned long int)0x82efb21e); //same
	np_t.ipv6 = 0;
	np_t.local_port = 5555;
	np_t.remote_port = 11111;
	memset(np_t.remote_mac, 0xff, ETH_ALEN);
	netpoll_print_options(&np_t);
	netpoll_setup(&np_t);
	np = &np_t;
}

/* Helper function to send debug messages */
static void send_debug(const char* buffer) {
	int length = strlen(buffer);
	netpoll_send_udp(np, buffer, length);
}

/* Init function, called when insmoding the module. Sets up stuff. */
static int __init key_value_store_init(void) {
	int ret = 0;
	char buffer[50];
	int linelength = 0;
	loff_t pos = 0;
	char key;
	int value;
	testmodule = kobject_create_and_add("testmodule", kernel_kobj);
	if (!testmodule) { return -ENOMEM; } 

	ret = sysfs_create_file(testmodule, &variable_attribute.attr); 
	if (ret) { pr_info("failed to create the variable file in /sys/kernel/testmodule\n"); }


	netfilterhook = (struct nf_hook_ops*)kcalloc(1, sizeof(struct nf_hook_ops), GFP_KERNEL);
	netfilterhook->hook = (nf_hookfn*)netfilterfunc;
	netfilterhook->hooknum = NF_INET_PRE_ROUTING;
	netfilterhook->pf = PF_INET;
	netfilterhook->priority = NF_IP_PRI_FIRST;

	nf_register_net_hook(&init_net, netfilterhook);
	init_netpoll();

	fp = file_open("datafile", O_RDWR);
	if (fp != NULL) {
		kernel_read(fp, buffer, 1, &pos);
		while(buffer[0] != 'Q') {	
			linelength = (int)buffer[0];
			kernel_read(fp, buffer, linelength, &pos);
			buffer[linelength] = '\0';
			key = buffer[0];
			sscanf(buffer + 2, "%d", &value);
			insert_key(key, value);
			kernel_read(fp, buffer, 1, &pos);
		}
		file_close(fp);
	}
	send_debug("Debug message - Startup working!");

	return ret;
}

/* Exit function, called when rmmoding the module. Simply cleans up stuff. */
static void __exit key_value_store_exit(void) {
	static struct data* temp;
	loff_t pos = 0;
	char buffer[50];
	int linelength = 0;
	send_debug("Debug message - Exit working!");
	fp = file_open("datafile", O_RDWR|O_CREAT|O_TRUNC);
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
		file_close(fp);
	}

	while (head != NULL) {
		temp = head->next;
		kfree(head);
		head = temp;
	}



	kobject_put(testmodule);
	nf_unregister_net_hook(&init_net, netfilterhook);
	kfree(netfilterhook); 
}

/* Bind the init/exit functions */
module_init(key_value_store_init); 
module_exit(key_value_store_exit); 
 
MODULE_LICENSE("GPL");

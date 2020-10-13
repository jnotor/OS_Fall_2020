// First, let’s include all the headers we’ll need
// These files contain prototypes for various kernel structures we’ll need
// to create our own modules.  Note the use of the < and > brackets denotes
// that the complier should get these headers from “standard places”.  These
// standard places will depend on exactly which kernel source you’re building
// against and are often defined in the make and/or other script files you’re
// using to direct the build process.  We’ll talk about this later.

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>

// The following block of “MODULE” documentation macros that are defined in
// <linux/module.h> are used to specify module and licensing documentation.
// These are used to set strings that document items a module user might care
// about. These fields can be viewed using the modinfo command.
// Likely the most "interesting" one is the MODULE_LICENSE() macro.  This one
// allows the module author to say what license the module is released under.
// What's interesting about it is that NOT saying "GPL" will cause the kernel
// to complain when the module is loaded.  This is intended as a means of
// allowing users to know when modules that are not released under GPL are loaded.
// There are more than I'm using here.  You can look into the modules.h header file
// to identify more

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Bender Bending Rodriquez");
MODULE_DESCRIPTION("An example module");
MODULE_SUPPORTED_DEVICE("This module support all the things")
MODULE_VERSION("0.01");
MODULE_INFO(intree, "Y");

// The following is a block of "event handlers" that we'll eventually associate
// with kernel events. We're not registering them with the kernel, this comes later.
// At a bare minimum, kernel modules must register two handlers, one for what
// the module is to do when it is loaded, and one to register what for what the
// module is to do when it is unloaded.
//
// In this case, I'm defining two handlers, hello_init() is intended to run
// when the module loads and hello_exit() is intended to run when the module
// is unloaded.  There's strangeness with these function definitions with the
// use of the __init and __exit designators in the definitions.  In a later exercise,
// we'll be asking you to explore what they mean and what happens if you replace
//
// static int __init hello_init(void)
//
// with
//
// static int hello_int(void)
//
// For now, we'll just look at what the functions do...


// hello_init() will be run when the module loads.  This example code does something
// really simple... it just prints a message into the system log file using the printk()
// function.  The first argument is the "importance level" of the message you're 
// putting in the log.  I'm using a very high level to make sure it gets in there
// even it the system administrator set up the machine to ignore "less important"
// messages.  Note that, in general, the on-load initializer can do much more complex
// stuff (which we'll talk about later).  It can register handlers for kernel events, 
// it could replace key kernel functions with new code -- literally rewriting the 
// kernel to do something else. If you're thinking this is potentially dangerous, 
// you're right.  This is why the kernel will generally be configured to take some 
// pains to let the system administrator know if "unblessed" modules are being force
// loaded.  Those unblessed modules could be quite literally "tainting" the kernel 
// to add... tainted functionality.  One better trust the author of the module... or 
// not load it at all.

static int __init hello_init(void)
   { printk(KERN_ALERT "Hello, World!\n");
     return 0;
   }

// hello_exit() will be run when the module unloads.  In this case, it too prints a 
// message into the system log.  Note... in general this function is intended (and must)
// undo ALL of the changes the "on-load" function did.  This function would be used
// to put the kernel back exactly the way it was before the module was loaded.

static void __exit hello_exit(void) 
  { printk(KERN_INFO "Goodbye, World!\n");
  } 


// The next two function call simply "register" the "on load" and "on unload" handler 
// functions.  Note they take as parameters the pointers to the desired handlers.

module_init(hello_init);
module_exit(hello_exit);




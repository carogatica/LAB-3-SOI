#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/types.h>  //for dev_t typedef
#include <linux/kdev_t.h> //for dormat_dev_t
//#include <cdev.h> //for allocate and register structures of type structures
				  //of type struc cdev to represent char devices
#include <asm/uaccess.h> 

#define DEVICE_NAME "encriptador"
#define CLASS_NAME  "encriptadorClass"   
#define MAX 250 


static int majorNumber;
static char mje[MAX]={0};
static char *mje_ptr;
static short mjeSize;
static struct class *myDevClass=NULL;
static struct device *myDevice=NULL;

static int     dev_open(struct inode *, struct file *);
//static int     dev_release(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);

static struct file_operations fops = 
{
	.read = dev_read,
	.write = dev_write,
	.open = dev_open,
};

static int __init myDev_init(void)
{
    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

	if (majorNumber < 0) {
	  printk(KERN_ALERT "Fallo el registro del dispositivo %d\n", majorNumber);
	  return majorNumber;
	}

	printk(KERN_INFO "I was assigned major number %d. To talk to\n", majorNumber);
	printk(KERN_INFO "the driver, create a dev file with\n");
	printk(KERN_INFO "'mknod /dev/%s c %d 0'.\n", DEVICE_NAME, majorNumber);
	printk(KERN_INFO "Try various minor numbers. Try to cat and echo to\n");
	printk(KERN_INFO "the device file.\n");
	printk(KERN_INFO "Remove the device file and module when done.\n");

	myDevClass = class_create(THIS_MODULE, CLASS_NAME);
	if (IS_ERR(myDevClass))
	{
		unregister_chrdev(majorNumber,DEVICE_NAME);
		printk(KERN_ALERT "Fallo al registrar la clase\n");
		return PTR_ERR(myDevClass);
	}

	printk(KERN_INFO "myDevice: se registro correctamente la clase");

	myDevice = device_create(myDevClass, NULL, MKDEV(majorNumber,0), NULL, DEVICE_NAME);
	if(IS_ERR(myDevice))
	{
		class_destroy(myDevClass);
		unregister_chrdev(majorNumber,DEVICE_NAME);
		printk(KERN_ALERT "Fallo al crear dispositivo\n");
		return PTR_ERR(myDevice);
	}

	printk(KERN_INFO "myDevice: dispositivo creado correctamente");

	return 0;
}

static void __exit myDev_exit(void)
{
	device_destroy(myDevClass, MKDEV(majorNumber, 0));     // remove the device
   	class_unregister(myDevClass);                          // unregister the device class
   	class_destroy(myDevClass);                             // remove the device class
   	unregister_chrdev(majorNumber, DEVICE_NAME);             // unregister the major number
   	printk(KERN_INFO "Encriptador: Goodbye from the LKM!\n");
}

static int dev_open(struct inode *inode, struct file *filp)
{
	printk(KERN_INFO "Encriptador: Device has been opened\n");
   	return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{

	/*int bytes_read = 0;

	if (*mje_ptr == 0)
		return 0;

	while(len && *mje_ptr)
	{
		put_user(*(mje_ptr++), buffer++)

		//copy_to_user(buffer, mje, mjeSize);

		len--;
		bytes_read++;
	}

	return bytes_read;
	*/




  /* int error_count = 0;
   // copy_to_user has the format ( * to, *from, size) and returns 0 on success
   error_count = copy_to_user(buffer, mje, mjeSize);

   if (error_count==0){            // if true then have success
      printk(KERN_INFO "EBBChar: Sent %d characters to the user\n", size_of_message);
      return (mjeSize=0);  // clear the position to the start and return 0
   }
   else {
      printk(KERN_INFO "EBBChar: Failed to send %d characters to the user\n", error_count);
      return -EFAULT;              // Failed -- return a bad address message (i.e. -14)
   }
*/

	ssize_t bytes;
	printk(KERN_INFO "encriptador_code: read()\n");
	//ret=copy_to_user(buf, virtual_device.data, len);	

	bytes = len < (MAX-(*offset)) ? len : (MAX-(*offset));
    if(copy_to_user(buffer, mje, bytes))
    {
        return -EFAULT;
    }

    (*offset) += bytes;

	return bytes;


}

static ssize_t dev_write(struct file *f, const char __user *buf, size_t len, loff_t *off)
{
	size_t index;
	printk(KERN_INFO "encriptador_code: write()\n");
	if(len > MAX) // se agrega esta sentencia para evitar la advertencia de overflow al compilar
		len = MAX;
	if (copy_from_user(mje, buf, len)) {
 		return -EFAULT;
 	}
 	*off += len;
	printk(KERN_INFO "encriptador_code: se escribio:%s.\n",mje);
	printk(KERN_INFO "encriptador_code: se actualizo solo: len=%i??\n",len);
	
	index=0;
	while(index<len){
		mje[index]=mje[index]+1;//Copia el la información encriptada
		index++;
	}
	
	while(index<MAX){
		mje[index]='\0';
		index++;
	}
 	return len;
		
}

MODULE_LICENSE("GPL");            ///< The license type -- this affects available functionality
MODULE_AUTHOR("Carolina Gatica");    ///< The author -- visible when you use modinfo
MODULE_DESCRIPTION("A simple Linux char driver for the BBB");  ///< The description -- see modinfo
MODULE_VERSION("0.1");    


module_init(encriptador_init);
module_exit(encriptador_exit);
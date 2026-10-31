#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "mychardev"
#define BUFFER_SIZE 1024

static dev_t dev_number;
static struct cdev my_cdev;
static struct class *my_class;
static struct device *my_device;

static char device_buffer[BUFFER_SIZE];
static size_t data_size = 0;

static DEFINE_MUTEX(buffer_lock);
static ssize_t device_read(struct file *file,
                           char __user *user_buffer,
                           size_t count, loff_t *offset)
{
    size_t bytes_to_read;

    if (*offset >= data_size)
        return 0;


    bytes_to_read = min(count, data_size - (size_t)*offset);

    if (copy_to_user(user_buffer,
                     device_buffer + *offset,
                     bytes_to_read))
        return -EFAULT;

    *offset += bytes_to_read;
    return bytes_to_read;
}

static ssize_t device_write(struct file *file,
                            const char __user *user_buffer,
                            size_t count, loff_t *offset)
{
    size_t bytes_to_write;

    if (*offset >= BUFFER_SIZE)
        return -ENOSPC;

    bytes_to_write = min(count, BUFFER_SIZE - (size_t)*offset);

    if (copy_from_user(device_buffer + *offset,
                       user_buffer,
                       bytes_to_write))
        return -EFAULT;

    *offset += bytes_to_write;

    if (*offset > data_size)
        data_size = *offset;

    return bytes_to_write;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
};

static int __init chardev_init(void)
{
    int result;

    result = alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME);
    if (result < 0)
        return result;

    cdev_init(&my_cdev, &fops);
    my_cdev.owner = THIS_MODULE;

    result = cdev_add(&my_cdev, dev_number, 1);
    if (result < 0)
        goto unregister_device;

    my_class = class_create(DEVICE_NAME);
    if (IS_ERR(my_class)) {
        result = PTR_ERR(my_class);
        goto delete_cdev;
    }

    my_device = device_create(my_class, NULL,
                              dev_number, NULL, DEVICE_NAME);
    if (IS_ERR(my_device)) {
        result = PTR_ERR(my_device);
        goto destroy_class;
    }

    pr_info("Character driver loaded successfully\n");
    return 0;

destroy_class:
    class_destroy(my_class);

delete_cdev:
    cdev_del(&my_cdev);

unregister_device:
    unregister_chrdev_region(dev_number, 1);
    return result;
}

static void __exit driver_exit(void)
{
    device_destroy(my_class, dev_number);
    class_destroy(my_class);
    cdev_del(&my_cdev);
    unregister_chrdev_region(dev_number, 1);

    pr_info("Character driver unloaded\n");
}

module_init(chardev_init);
module_exit(driver_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Student");
MODULE_DESCRIPTION("Simple Linux Character Device Driver");


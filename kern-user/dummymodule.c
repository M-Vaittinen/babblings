#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/fs.h>


/*
 * Stuff for creating 1.st sysfs file 
 */

static uint32_t my_attr_1_value = 0;

static ssize_t my_attr_1_show(struct device *dev, struct device_attribute *attr, char *buf)
{ 
    int *privatedata = dev_get_drvdata(dev);
    pr_info("%s: was called, private_data = %d\n",__func__,*privatedata);
    *privatedata=*privatedata+1;
	snprintf(buf, 20, "0x%x\n",my_attr_1_value);
    return strlen(buf) + 1;
}
static ssize_t my_attr_1_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int *privatedata = dev_get_drvdata(dev);
    int rval;
    pr_info("%s: was called, private_data = %d\n",__func__,*privatedata);
    *privatedata=*privatedata+1;
	if ((rval = kstrtou32(buf, 0, &my_attr_1_value))) { 
		pr_err("%s: Non numeric value entered\n", __func__);
		return rval;
	}
	return strnlen(buf,count); 
}
/* Attributes are often done via helper macros like DEVICE_ATTR */
static DEVICE_ATTR(my_attr_1, (S_IWUSR | S_IWGRP | S_IRUGO), &my_attr_1_show, &my_attr_1_store);


/*
 * Stuff for creating 2.nd sysfs file 
 */


static char my_attr_2_value[256] = {'\0'};

static ssize_t my_attr_2_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
    int ret;
    int *privatedata = dev_get_drvdata(dev);
    pr_info("%s: was called, private_data = %d\n",__func__,*privatedata);
    *privatedata=*privatedata+1;
    ret = snprintf(&(my_attr_2_value[0]), (size_t)256, "%.*s",(int)min(count, (size_t)(256 - 1)) , buf);
    ret = (ret > 256) ? 256 : ret;
    return ret;
}
static ssize_t my_attr_2_show(struct device *dev, struct device_attribute *attr, char *buf)
{
    int *privatedata = dev_get_drvdata(dev);
    pr_info("%s: was called, private_data = %d\n",__func__,*privatedata);
    *privatedata=*privatedata+1;
    return scnprintf(buf, PAGE_SIZE, "%s\n", &(my_attr_2_value[0]));
}

/* Lets do it without helper macro and show
 * what happens under the hood */

static struct device_attribute dev_attr_my_attr_2 =
{
    .attr = 
    {
        .name = "my_attr_2",
        .mode = VERIFY_OCTAL_PERMISSIONS((S_IWUSR | S_IWGRP | S_IRUGO)),
    },
    .show = &my_attr_2_show,
    .store = &my_attr_2_store,
};


/*
 * Stuff for creating 3.rd sysfs file 
 */

static unsigned long my_attr_3_value = 0;
/*
static ssize_t my_attr_3_store(struct device_driver *driver, const char *buf, size_t count)
{
    int rval;
    if(!(rval=kstrtoul(buf,0,&my_attr_3_value)))
        rval=strnlen(buf, count);
    return rval;
}*/
static ssize_t my_driver_attr_show(struct device_driver *driver, char *buf)
{
    return snprintf(buf,PAGE_SIZE, "0x%lx\n", my_attr_3_value);
}
/* Let's do one more. And let's do a driver attribute this time
 * For the sake of difference, let's not do store at all. */

static DRIVER_ATTR(my_driver_attr, S_IRUGO, my_driver_attr_show, NULL);


/* We can create sysfs entries one by one but if we have bunch of em it get's boring...
 * So we rather group em together and create all of them at one go */
static struct attribute *my_attrs[] =
{
	&dev_attr_my_attr_1.attr,
	&dev_attr_my_attr_2.attr,
    NULL,
};
/* And for grouped attributes we give common name that will also be shown in sysfs as a folder */
static struct attribute_group my_attr_group =
{
    .name = "attributes",
    .attrs = my_attrs,
};

/* We will create our own 'class' device because
 *  a) We do not have real device here
 *  b) If we had a real device, we still wouldn't want to bind our attributes to this because 
 *  it is owned by bus and not by this module. Thus if sysfs is open when module is removed the 
 *  device entry did not disappear and call to sysfs would end up in a disaster.
 *  c) it is cool to have a /sys/class/training entry in device-tree =)
 */
static struct class my_class =
{
    .name = "training",
    .owner = THIS_MODULE,
};

static dev_t my_device_number;
static struct device *my_class_device;
static const int dev_amount=1;

static int private_data_for_device = 0;



static struct of_device_id training_driver_match[] = 
{
    {
        .compatible = "my-chip-vendor,my-chip-compatible",
    }
    ,
    {
        .compatible = "another-vendor,another-compatible-chip",
    },
    {},
};
MODULE_DEVICE_TABLE(of, training_driver_match);

/* This is the function that would be called if device matching this driver was found 
 * In real driver case all initializations would be done here.
 * We have no matching device => this is never called for us.
 */
static int training_driver_probe(struct platform_device *pdev) 
{
    return 0;
}

/* This would be called if device (or module) is removed */
static int training_driver_remove(struct platform_device *pdev)
{
    return 0;
}

static struct platform_driver my_test_driver = {
    .driver = {
        .owner = THIS_MODULE,
        .name = "training-driver",
        .of_match_table = training_driver_match,
    },
    .probe  = training_driver_probe,
    .remove = training_driver_remove,
};
static int __init training_sysfs_init(void)
{
    int rval=0;
    int dev_minor=0;
    const char *devicename="training-device";
    pr_info("%s: Loading training-device\n",__func__);

    if(!(rval=platform_driver_register(&my_test_driver)))
        if(!(rval = class_register(&my_class)))
            /* Allocate a range (one) device number for us */
            if (!(rval=alloc_chrdev_region(&my_device_number, dev_minor, dev_amount, "training-device")))
                if(!(IS_ERR(my_class_device=device_create(&my_class,NULL,my_device_number,&private_data_for_device,"%s",devicename))))
                    if (!(rval = sysfs_create_group(&(my_class_device->kobj), &my_attr_group)))
                        if((rval=driver_create_file(&my_test_driver.driver, &driver_attr_my_driver_attr)))
                            goto err_driver_create_file;
                        else
                            pr_info("%s: Great Success! my class dev is %p (%ld)\n",__func__,my_class_device,PTR_ERR(my_class_device));
                    else 
                        goto err_sysfs_create_group;
                else
                    goto err_device_create;
            else
                goto err_dev_alloc;
        else
            goto err_class_reg;
    else
        goto err_platform_driver_register;    

if(0)
{    
err_driver_create_file:
    sysfs_remove_group(&(my_class_device->kobj), &my_attr_group);
err_sysfs_create_group:
    device_destroy(&my_class,my_device_number);
err_device_create:
    unregister_chrdev_region(my_device_number,dev_amount);
err_dev_alloc:
    class_unregister(&my_class);
err_class_reg:
    platform_driver_unregister(&my_test_driver);
err_platform_driver_register:    
    ;
}
    return rval;
}



static void __exit training_sysfs_exit(void)
{
    driver_remove_file(&my_test_driver.driver,&driver_attr_my_driver_attr);
    sysfs_remove_group(&(my_class_device->kobj), &my_attr_group);
    device_destroy(&my_class,my_device_number);
    unregister_chrdev_region(my_device_number,dev_amount);
    class_unregister(&my_class);
    platform_driver_unregister(&my_test_driver);
    pr_info("%s: Bye\n",__func__);
}


module_init(training_sysfs_init);
module_exit(training_sysfs_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Matti Vaittinen");



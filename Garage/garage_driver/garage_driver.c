#include "garage_gpio.h"


/* Declaration of garage_driver.c functions */


int garage_driver_init(void);
void garage_driver_exit(void);

static int garage_driver_open(struct inode *, struct file *);
static int garage_driver_release(struct inode *, struct file *);
static ssize_t garage_driver_read(struct file *, char *buf, size_t , loff_t *);
static ssize_t garage_driver_write(struct file *, const char *buf, size_t , loff_t *);

/* Structure that declares the usual file access functions. */
struct file_operations garage_driver_fops =
{
    open    :   garage_driver_open,
    release :   garage_driver_release,
    read    :   garage_driver_read,
    write   :   garage_driver_write
};



/* Declaration of the init and exit functions. */
module_init(garage_driver_init);
module_exit(garage_driver_exit);

/* Global variables of the driver */

/* Major number. */
int garage_driver_major;

int Gflag = 0;
int Rflag = 0;


/* Buffer to store data. */
#define BUF_LEN 80
static char garage_driver_buffer[BUF_LEN];

/* HRtimer vars. */

static struct hrtimer timer_green;
static struct hrtimer timer_red;

static ktime_t green;
static ktime_t red;


#define MS_TO_NS(x) ((x) * 1E6L)
#define BLINK_SEC 0
#define BLINK_NANO_SEC 1000*1000*250

static enum hrtimer_restart blink_green(struct hrtimer *param)
{
    char led_pin = (m > n) ? GPIO_16 : /* true */
                             (m > 5) ? GPIO_26 : GPIO_5;

    if(Gflag == 0)
    {
        SetGpioPin(led_pin);
        Gflag = 1;

    }
    else if(Gflag == 1)
    {
        ClearGpioPin(led_pin);
        Gflag = 0;
    }

    hrtimer_forward(&timer_green, ktime_get(), green);

    return HRTIMER_RESTART;
}

static enum hrtimer_restart blink_red(struct hrtimer *param)
{
    if(Rflag == 0)
    {
        SetGpioPin(GPIO_16);
        Rflag = 1;

    }
    else if(Rflag == 1)
    {
        ClearGpioPin(GPIO_16);
        Rflag = 0;
    }

    hrtimer_forward(&timer_red, ktime_get(), red);

    return HRTIMER_RESTART;
}

/*
 * Initialization:
 *  1. Register device driver
 *  2. Allocate buffer
 *  3. Initialize buffer
 */
int garage_driver_init(void)
{
    int result = -1;

    /* Registering device. */
    result = register_chrdev(0, "garage_driver", &garage_driver_fops);
    if (result < 0)
    {
        printk(KERN_INFO "garage_driver: cannot obtain major number %d\n", garage_driver_major);
        return result;
    }

    garage_driver_major = result;
    printk(KERN_INFO "Garage driver major number is %d\n", garage_driver_major);

    /* Initialize data buffer. */
    memset(garage_driver_buffer, 0, BUF_LEN);

    printk(KERN_INFO "Inserting garage_driver module\n");

    /* Initialize high resolution timer. */

    hrtimer_init(&timer_green, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    green = ktime_set(BLINK_SEC, BLINK_NANO_SEC);
    timer_green.function = &blink_green;
    hrtimer_start(&timer_green, green, HRTIMER_MODE_REL);

    hrtimer_init(&timer_red, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    red = ktime_set(BLINK_SEC, BLINK_NANO_SEC);
    timer_red.function = &blink_red;
    hrtimer_start(&timer_red, red, HRTIMER_MODE_REL);

    /****************************************//

    // LED PINS
    SetInternalPullUpDown(GPIO_26,PULL_DOWN);
    SetGpioPinDirection(GPIO_26,GPIO_DIRECTION_OUT);

    SetInternalPullUpDown(GPIO_16,PULL_DOWN);
    SetGpioPinDirection(GPIO_16,GPIO_DIRECTION_OUT);
    //MOTOR pins
    SetGpioPinDirection(GPIO_12,GPIO_DIRECTION_OUT);
    SetGpioPinDirection(GPIO_13,GPIO_DIRECTION_OUT);
    //INTERRUPT pins
    SetGpioPinDirection(GPIO_05,GPIO_DIRECTION_IN); // Obstacle
    SetGpioPinDirection(GPIO_06,GPIO_DIRECTION_IN); // Opened Button
    SetGpioPinDirection(GPIO_24,GPIO_DIRECTION_IN);

    gpio_request_one(GPIO_05, GPIOF_IN, "Obstacle");
    irq_gpio3 = gpio_to_irq(GPIO_03);
    if(request_irq(irq_gpio3, h_irq_gpio3,
      IRQF_TRIGGER_FALLING, "irq_gpio3", (void *)(h_irq_gpio3))) != 0)
    {
        printk("Error: ISR not registered!\n");
    }





    return 0;
}

/*
 * Cleanup:
 *  1. Unregister device driver
 *  2. Free buffer
 */
void garage_driver_exit(void)
{
    /* Freeing the major number. */
    unregister_chrdev(garage_driver_major, "garage_driver");

    printk(KERN_INFO "Removing garage_driver module\n");

    hrtimer_cancel(&timer_green);
    hrtimer_cancel(&timer_red);

    SetInternalPullUpDown(GPIO_16,PULL_NONE);
    SetInternalPullUpDown(GPIO_26,PULL_NONE);

    ClearGpioPin(GPIO_26);
    ClearGpioPin(GPIO_16);

}


/* File open function. */
static int garage_driver_open(struct inode *inode, struct file *filp)
{
    /* Initialize driver variables here. */

    /* Reset the device here. */

    /* Success. */
    return 0;
}

/* File close function. */
static int garage_driver_release(struct inode *inode, struct file *filp)
{
    /* Success. */
    return 0;
}

/*
 * File read function
 *  Parameters:
 *   filp  - a type file structure;
 *   buf   - a buffer, from which the user space function (fread) will read;
 *   len - a counter with the number of bytes to transfer, which has the same
 *           value as the usual counter in the user space function (fread);
 *   f_pos - a position of where to start reading the file;
 *  Operation:
 *   The garage_driver_read function transfers data from the driver buffer (garage_driver_buffer)
 *   to user space with the function copy_to_user.
 */
static ssize_t garage_driver_read(struct file *filp, char *buf, size_t len, loff_t *f_pos)
{
    /* Size of valid data in gpio_driver - data to send in user space. */
    int data_size = 0;
    if (*f_pos == 0)
    {
        /* Get size of valid data. */
        data_size = strlen(garage_driver_buffer);

        /*char c = 5;
        garage_drive_buffer[0] = c;
        copy_to_user(buf, garage_driver_buffer, sizeof(c));*/

        int i = 5;
        int* pi_garage_drive_buffer = (int*)garage_drive_buffer;
        pi_garage_drive_buffer[0] = i;
        copy_to_user(buf, garage_driver_buffer, sizeof(c));

        /* Send data to user space. */
        if (copy_to_user(buf, garage_driver_buffer, data_size) != 0)
        {
            return -EFAULT;
        }
        else
        {
            (*f_pos) += data_size;

            return data_size;
        }
    }
    else
    {
        return 0;
    }
}

/*
 * File write function
 *  Parameters:
 *   filp  - a type file structure;
 *   buf   - a buffer in which the user space function (fwrite) will write;
 *   len - a counter with the number of bytes to transfer, which has the same
 *           values as the usual counter in the user space function (fwrite);
 *   f_pos - a position of where to start writing in the file;
 *  Operation:
 *   The function copy_from_user transfers the data from user space to kernel space.
 */
static ssize_t garage_driver_write(struct file *filp, const char *buf, size_t len, loff_t *f_pos)
{
    /* Reset memory. */
    memset(garage_driver_buffer, 0, BUF_LEN);
    /* Get data from user space.*/
    if (copy_from_user(garage_driver_buffer, buf, len) != 0)
    {
        return -EFAULT;
    }
	else
	{
        return len;
    }

}

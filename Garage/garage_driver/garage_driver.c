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

int check;


/* Buffer to store data. */
#define BUF_LEN 80
static char garage_driver_buffer[BUF_LEN];
static char state_buffer[BUF_LEN];

/* HRtimer vars. */

static struct hrtimer signal_timer;

static ktime_t rising_edge;
static ktime_t falling_edge;

#define MS_TO_NS(x) ((x) * 1E6L)
#define TIMER_SEC  15
#define TIMER_NANO_SEC  0
#define STOP_SEC 5
#define STOP_NANO_SEC 0

/* IRQ number. */
static int irq_gpio25 = -1;
static int irq_gpio26 = -1;



static irqreturn_t obstacleIrq(int irq, void *data)  // GPIO_25 interrupt obstacle DETECTED
{
    check = strcmp(state_buffer,"Closing");
    if(check == 0)
    {
        SetGpioPin(GPIO_24);   //  SEND TO ARDUINO  GPIO_24
        strcpy(state_buffer, "Obstacle");
        ClearGpioPin(GPIO_24);
    }
    return IRQ_HANDLED;
}

static irqreturn_t stateIrq(int irq, void *data)
{
    if(GetGpioPinValue(GPIO_26))
    {
        rising_edge = ktime_get();
    }
    else
    {
        falling_edge = ktime_get();
    }
}


/*static enum hrtimer_restart signal_callback(struct hrtimer *param)
{

    return HRTIMER_NORESTART;
}

*/
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
    memset(state_buffer,0,BUF_LEN);
    //**************************//


    printk(KERN_INFO "Inserting garage_driver module\n");

    /* Initialize high resolution timer. */

    /*hrtimer_init(&signal_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL);
    rising_edge = ktime_set(TIMER_SEC, TIMER_NANO_SEC);
    falling_edge = ktime_set(STOP_SEC,STOP_NANO_SEC);
    garage_timer.function = &garage_timer_callback;
    */
    /**************************************************/
    /************ Initialize GPIO pins. ***************/
    /**************************************************/

    /************* OPEN, CLOSE, STOP ************/

    // 1 0 - OPEN
    // 0 1 - CLOSE
    // 1 1 - STOP.

    SetGpioPinDirection(GPIO_22, GPIO_DIRECTION_OUT);
    SetGpioPinDirection(GPIO_23, GPIO_DIRECTION_OUT);

    /********** OBSTACLE DETECTED ******/

    SetGpioPinDirection(GPIO_24, GPIO_DIRECTION_OUT);

    /********** Initialization pin ******/

    SetGpioPinDirection(GPIO_27, GPIO_DIRECTION_OUT);


    /* Initialize gpio 25 ISR. OBSTACLE */

    /************ O B S T A C L E *************/

    gpio_request_one(GPIO_25, GPIOF_IN, "irq_gpio25");
    irq_gpio25 = gpio_to_irq(GPIO_25);
    if(request_irq(irq_gpio25, obstacleIrq, IRQF_TRIGGER_RISING , "irq_gpio25", (void *)(obstacleIrq)) != 0)
    {
        printk("Error: obstacle ISR not registered!\n");
    }

    /* Initialize gpio 26 ISR.*/

    /**************** S T A T E *************/

    gpio_request_one(GPIO_26, GPIOF_IN, "irq_gpio26");
    irq_gpio26 = gpio_to_irq(GPIO_26);
    if(request_irq(irq_gpio26, stateIrq, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "irq_gpio26", (void *)(stateIrq)) != 0)
    {
        printk("Error: state ISR not registered!\n");
    }

    /**************** Initialization *************/

    strcpy(state_buffer,"Closed");  // Garage initial state
    SetGpioPin(GPIO_27); // SEND to ARDUINO
    ClearGpioPin(GPIO_27);

    /*********************************************/


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

    /* Release IRQ nad handler. */
    disable_irq(irq_gpio25);
    free_irq(irq_gpio25, obstacleIrq);
    gpio_free(GPIO_25);

    disable_irq(irq_gpio26);
    free_irq(irq_gpio26, stateIrq);
    gpio_free(GPIO_26);

    //hrtimer_cancel(&garage_timer);

    /* Clear GPIO pins. */
    ClearGpioPin(GPIO_22);
    ClearGpioPin(GPIO_23);
    ClearGpioPin(GPIO_24);
    ClearGpioPin(GPIO_27);


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
        data_size = strlen(state_buffer);
        printk("Vreme %d\n",rising_edge - falling_edge);


        /* Send data to user space. */
        if (copy_to_user(buf, state_buffer, data_size) != 0)
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
		garage_driver_buffer[len] = '\0';

		check = strcmp(garage_driver_buffer,"Open");
		if(check == 0)
		{
            SetGpioPin(GPIO_22);
            ClearGpioPin(GPIO_23);
			strcpy(state_buffer,"Opening");
		}

		check = strcmp(garage_driver_buffer,"Close");
		if(check == 0)
		{
            ClearGpioPin(GPIO_22);
            SetGpioPin(GPIO_23);
			strcpy(state_buffer,"Closing");
		}

		check = strcmp(garage_driver_buffer,"Stop");
		if(check == 0)
		{
			SetGpioPin(GPIO_22);
            SetGpioPin(GPIO_23);
		}

	    return len;
	}

}

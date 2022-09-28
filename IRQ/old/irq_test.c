#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/mutex.h>

// pointer to task behind our kernel thread
struct task_struct *task;

//Size of data we write
#define DATA_SIZE 2000
//Storage for data accessed by thread and irq
static char global_test_array[DATA_SIZE] = {'0'};

/* Locks to try */
static DEFINE_MUTEX(irq_mutex);
static DEFINE_SPINLOCK(irq_lock);

/* 
 * Thread function performing data copying in loops.
 * Outer loop index 'i' is used to select value that is used for this loop.
 * Inner loop index 'j' is used to advance array index for writing
 * So thread first fills the array with '0', then with '1', ...
 *
 * Try adding locks to thread and irq handler so that data writing is protected and check function below passes. Try using both the spin lock and mutex. Try mutex_lock/mutex_unlock, spin_lock/spin_unlock, spin_lock_irqsave/spin_unlock_irqrestore. What do you observe and why?
 */
int thread_function(void *data)
{
    unsigned int i,j;
    char fill[10]={'0','1','2','3','4','5','6','7','8','9'};
    for(i=0;1;i++)
        if(kthread_should_stop())
            break;
        else
        {
            //A: lock here?
            if(!(i%1000))
                schedule();
            else
            {
                //B: lock here?
                for(j=0;j<DATA_SIZE;j++)
                    global_test_array[j]=fill[i%10];
                //B: unlock here?
            }
            //A: unlock here?
        }
    return 0;
}
// Struct which could be used to deliver data in irq.
static struct irqdata
{
    int irqno;
}g_irqdata={.irqno=0};


/*
 * To demonstrate concurrency problem we do copy the array when interrupt occurs and hope that array is filled with constant value (yes, clumsy I know - well, please invent a better excercise yourself and hand it over to me =]).  Eg, we check if array contains only one type of character. Eg, array full of '3' is Ok, but array where we have both '3' and '4' is not. This function checks that we have only one type of characters stored in array
 */
static void check_data(char *data,size_t size)
{
    int i;
    int ok=1;
    char c;
    if(size>0)
        for(i=1,c=*data;i<size;i++)
            if(data[i]!=c)
            {
                if(ok)
                    pr_err("Inconsistent data, found '%c' and '%c'\n",c,data[i]);
                ok=0;
            }
    if(ok)
        pr_info("Great, data check passed\n");
}

/* Our interrupt handler. When rtc interupt occurs we copy the array and check the contents. Without any locking we expect to see (at least occasionally) non concurrent data */
static irqreturn_t test_interrupt(int irq, void *p)
{
    char data[DATA_SIZE];
    //lock here
    memcpy(data,global_test_array,DATA_SIZE);
    //unlock here
    check_data(data,DATA_SIZE);
    return IRQ_HANDLED;
}

static int __init training_init(void)
{
    int rval;	
	rval = request_irq(8, test_interrupt,
			IRQF_SHARED, "test_interrupt",
			&g_irqdata);

    task = kthread_run(&thread_function,NULL,"locker");
    return rval;

}
static void __exit training_exit(void)
{
    kthread_stop(task);
    free_irq(8,&g_irqdata);
}

module_exit(training_exit);
module_init(training_init);

MODULE_AUTHOR("Matti Vaittinen <matti.vaittinent@nokia.com>");
MODULE_DESCRIPTION("TRAINING Device Driver ");
MODULE_LICENSE("GPL");

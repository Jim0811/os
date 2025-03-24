#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int simple_init(void);
void simple_exit(void);

/* 當模組載入時執行 */
int simple_init(void)
{
    printk(KERN_INFO "Loading Kernel Module A1115528\n");
    return 0;
}

/* 當模組移除時執行 */
void simple_exit(void)
{
    printk(KERN_INFO "Removing Kernel Module A1115528\n");
}

/* 註冊模組的載入與移除函式 */
module_init(simple_init);
module_exit(simple_exit);

/* 模組的元資訊 */
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");

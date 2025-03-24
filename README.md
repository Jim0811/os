# Problem 2

## C program(基本上跟講義中一模一樣)
```C
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

```
### Linux/init.h: No such file or directory 解決方法

1.找找看有沒有init.h
```bash
cd /usr/src/linux-hwe-6.11-headers-6.11.0-19/include/linux
ll init.h
```

2.有的話搬到linux裡，這樣標頭檔就吃的到了
```bash
sudo cp init.h /usr/include/linux #要加sudo不然權限不夠
```
沒有的話可以
```bash
sudo apt install linux-headers-$(uname -r)
```

3.有些標頭檔中也含有其他標頭檔，建議ctrl+滑鼠點擊去看看有沒有什麼標頭檔沒吃到

搞到標頭檔沒報錯才開始makefile

## makefile
1.在與.c檔同一個資料夾下創建makefile檔
```makefile
obj-m += simple.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```

2.make
```bash
make
sudo insmod simple.ko
sudo rmmod simple
sudo dmesg | tail -20
```

如果執行完覺得dmesg日誌太雜可以
```bash
sudo dmesg -C
```
清空日誌
然後再做一次insmod跟rmmod

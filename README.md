# Problem 1

1. 將 hostname 修改成 csc061
```bash
sudo hostnemactl set-hostname csc061
sudo nano /etc/hosts
```
確認terminal的output是否跟下面一樣
```bash
127.0.0.1	localhost
127.0.1.1	csc061
```
如果一樣就可以儲存了
```bash
sudo systemctl restart systemd-hostnamed
```
2. 建立學號帳號
```bash
sudo adduser yourstudentnumber
```
系統會要求輸入密碼與相關資訊，依需求輸入或按 Enter 跳過(可以全都按enter跳過)

給學號帳號 sudo 權限：
```bash
sudo usermod -aG sudo yourstudentnumber
```

3. 切換到學號帳號
```bash
su - yourstudentnumber
```
或者回到登入介面再重新登入學號帳號
接著應該能在terminal看到開頭變:
```bash
yourstudentnumber@csc061:~$
```
我是有正常顯示，如果沒有可以試一下下面的prompt:
```bash
exec bash
```

# Problem 2
*可以試試看不動標頭檔看看能不能做出來，因為我Problem 3沒動一樣能做*
## C program (跟pdf中的有小改 下面註解標出)
```C
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

int simple_init(void); //增加，不然make會報錯
void simple_exit(void); //增加，不然make會報錯

int simple_init(void)
{
    printk(KERN_INFO "Loading Kernel Module A1115528\n");
    return 0;
}

void simple_exit(void)
{
    printk(KERN_INFO "Removing Kernel Module A1115528\n");
}

module_init(simple_init);
module_exit(simple_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Simple Module");
MODULE_AUTHOR("SGG");
```
### Linux/init.h: No such file or directory 解決方法

1. 找找看有沒有init.h
```bash
cd /usr/src/linux-hwe-6.11-headers-6.11.0-19/include/linux
ll init.h
```

2. 有的話搬到linux裡，這樣標頭檔就吃的到了
```bash
sudo cp init.h /usr/include/linux #要加sudo不然權限不夠
```
沒有的話可以
```bash
sudo apt install linux-headers-$(uname -r)
```

3. 有些標頭檔中也含有其他標頭檔，建議ctrl+滑鼠點擊去看看有沒有什麼標頭檔沒吃到

搞到標頭檔沒報錯才開始makefile

## makefile
1.在與.c檔同一個資料夾下創建makefile檔
**makefile要是大寫(Makefile)**
```makefile
obj-m += your_cfilename.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```

2. make
```bash
make
```
> [!WARNING]
>```bash
>make -C /lib/modules/6.11.0-19-generic/build M=/home/a1115528/os modules
>make[1]: Entering directory '/usr/src/linux-headers-6.11.0-19-generic'
>warning: the compiler differs from the one used to build the kernel
>  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
>  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
>  CC [M]  /home/a1115528/os/simple.o
>  MODPOST /home/a1115528/os/Module.symvers
>  CC [M]  /home/a1115528/os/simple.mod.o
>  LD [M]  /home/a1115528/os/simple.ko
>  BTF [M] /home/a1115528/os/simple.ko
>Skipping BTF generation for /home/a1115528/os/simple.ko due to unavailability of vmlinux
>make[1]: Leaving directory '/usr/src/linux-headers-6.11.0-19-generic'
>```
>要解決skipping BTF的問題
>```bash
>sudo apt install pahole
>```
>chatgpt還有提供另一種方式(我沒試過)
>```bash
>Skipping BTF generation for /home/a1115528/os/simple.ko due to unavailability of vmlinux
>```

```bash
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

做完後
```bash
make clean
```
可以將編譯過程中產生的臨時文件刪除
檔案看起來不會那麼亂

# Problem 3

## C program

```C (跟pdf中的有小改 下面註解標出)
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>

#define BUFFER_SIZE 128
#define PROC_NAME "hello"

int proc_init(void); //增加，不加後面make報錯
void proc_exit(void); //增加，不加後面make報錯

ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos);

static struct proc_ops proc_ops = {  //由 static struct file_operation proc_ops 改 (linux 5.6後改的)
    .proc_read = proc_read,
};

/* This function is called when the module is loaded. */
int proc_init(void)
{
    /* creates the /proc/hello entry */
    proc_create(PROC_NAME, 0666, NULL, &proc_ops);
    return 0;
}

/* This function is called when the module is removed. */
void proc_exit(void)
{
    /* removes the /proc/hello entry */
    remove_proc_entry(PROC_NAME, NULL);
}

/* This function is called each time /proc/hello is read */
ssize_t proc_read(struct file *file, char __user *usr_buf, size_t count, loff_t *pos)
{
    int rv = 0;
    char buffer[BUFFER_SIZE];
    static int completed = 0;

    if (completed) {
        completed = 0;
        return 0;
    }

    completed = 1;
    rv = snprintf(buffer, BUFFER_SIZE, "Hello World A1115528\n");

    if(copy_to_user(usr_buf,buffer,rv)){ //改，不然後面make有報錯
        return -EFAULT;
    }

    return rv;
}

module_init(proc_init);
module_exit(proc_exit);

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Hello Module");
MODULE_AUTHOR("SGG");

```

## makefile
1. 與 Problem2 相同建makefile檔
```makefile
obj-m += your_cfilename.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD  := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
```
2. make
```bash
make
```
> [!WARNING 1]
>```bash
>make -C /lib/modules/6.11.0-19-generic/build M=/home/a1115528/os/Problem3 modules
>make[1]: Entering directory '/usr/src/linux-headers-6.11.0-19-generic'
>warning: the compiler differs from the one used to build the kernel
>  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
>  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
>scripts/Makefile.build:41: /home/a1115528/os/Problem3/Makefile: No such file or directory
>make[3]: *** No rule to make target '/home/a1115528/os/Problem3/Makefile'.  Stop.
>make[2]: *** [/usr/src/linux-headers-6.11.0-19-generic/Makefile:1931: /home/a1115528/os/Problem3] Error 2
>make[1]: *** [Makefile:224: __sub-make] Error 2
>make[1]: Leaving directory '/usr/src/linux-headers-6.11.0-19-generic'
>make: *** [makefile:7: all] Error 2
>```
>遇到的第一個 error 就是因為 makefile 是小寫
>所以注意 **makefile 一定要大寫 ( Makefile )**

> [!WARNING 2]
>```bash
>make -C /lib/modules/6.11.0-19-generic/build M=/home/a1115528/os/Problem3 modules
>make[1]: Entering directory '/usr/src/linux-headers-6.11.0-19-generic'
>warning: the compiler differs from the one used to build the kernel
>  The kernel was built by: x86_64-linux-gnu-gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
>  You are using:           gcc-13 (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0
>  CC [M]  /home/a1115528/os/Problem3/hellomodule.o
>  MODPOST /home/a1115528/os/Problem3/Module.symvers
>  CC [M]  /home/a1115528/os/Problem3/hellomodule.mod.o
>  LD [M]  /home/a1115528/os/Problem3/hellomodule.ko
>  BTF [M] /home/a1115528/os/Problem3/hellomodule.ko
>Skipping BTF generation for /home/a1115528/os/Problem3/hellomodule.ko due to unavailability of vmlinux
>make[1]: Leaving directory '/usr/src/linux-headers-6.11.0-19-generic'
>```
>一樣是skipping BTF的問題
>```bash
>sudo apt install pahole
>```
>chatgpt還有提供另一種方式(我沒試過)
>```bash
>Skipping BTF generation for /home/a1115528/os/simple.ko due to unavailability of vmlinux
>```

```bash
sudo insmod hellomodule.ko
cat /proc/hello
```
做到這裡應該就會有
```bash
Hello World A1115528
```
最後做收拾的部分
```bash
sudo rmmod hellomodule
make clean
```

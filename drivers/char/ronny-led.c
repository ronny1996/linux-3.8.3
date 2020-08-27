#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/io.h>
#include <linux/kernel.h>
#include <linux/miscdevice.h>
#include <linux/module.h>
#include <linux/pci.h>
#include <linux/uaccess.h>

#include <asm/irq.h>
// #include <mach/gpio.h>
#include <mach/hardware.h>
#include <mach/map.h>
#include <mach/regs-gpio.h>
#include <plat/gpio-cfg.h>

#define DEVICE_NAME "led"
#define LED_MAJOR 240

#define LED_ON 0
#define LED_OFF 1

static int led_open(struct inode *inode, struct file *file) {
  unsigned int i;
  for (i = 0; i < 4; i++) {
    s3c_gpio_cfgpin(S3C64XX_GPM(i), S3C_GPIO_OUTPUT);
    printk("The GPMCON %x is %x \n", i, s3c_gpio_getcfg(S3C64XX_GPM(i)));
  }
  printk("Led open...\n");
  return 0;
}

static long led_ioctl(struct file *filp, unsigned int cmd, unsigned long argv) {
  if (argv > 4) return -EINVAL;
  printk("Led ioctl...\n");
  switch ((cmd)) {
    case LED_ON:
      gpio_set_value(S3C64XX_GPM(argv), 0);
      printk("LED ON \n");
      printk("S3C64XX_GPM(i) = %x\n", gpio_get_value(S3C64XX_GPM(argv)));
      break;
    case LED_OFF:
      gpio_set_value(S3C64XX_GPM(argv), 1);
      printk("LED OFF \n");
      printk("S3C64XX_GPM(i) = %x\n", gpio_get_value(S3C64XX_GPM(argv)));
      break;
    default:
      return -EINVAL;
      break;
  }
  return 0;
}

static int led_release(struct inode *inode, struct file *filp) {
  printk("ronny >> s3c6410_led release \ns");
  return 0;
}

struct file_operations led_fops = {
    .owner = THIS_MODULE,
    .open = led_open,
    .unlocked_ioctl = led_ioctl,
    .release = led_release,
};

static int __init led_init(void) {
  int rc;
  printk("LED Init...\n");
  rc = register_chrdev(LED_MAJOR, "led", &led_fops);
  if (rc < 0) {
    printk("register %s char dev error\n", "led");
    return -1;
  }
  printk("OK!\n");
  return 0;
}

static void __exit led_exit(void) {
  unregister_chrdev(LED_MAJOR, "led");
  printk("LED exit...\n");
}

module_init(led_init);
module_exit(led_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("ronny");
MODULE_DESCRIPTION("OK6410 [S3C6410] LED Driver");

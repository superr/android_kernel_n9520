/* Copyright (c) 2010-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#define pr_fmt(fmt)	"%s: " fmt, __func__

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/platform_device.h>
#include <linux/leds.h>
#include <linux/workqueue.h>
#include <linux/err.h>

#include <linux/mfd/pm8xxx/core.h>
#include <linux/mfd/pm8xxx/pwm.h>
#include <linux/mfd/pm8xxx/mpp.h>
#include <linux/leds-pm8xxx.h>

#include <linux/ctype.h>	/* zte for isspace() zte-ccb-20120110 */
#define LED_TAG	"ledlog_kn  "

/*
pm8xxx_pwm_lut_config(led->pwm_dev,
				led->pwm_period_us,                                
				led->pwm_duty_cycles->duty_pcts,
				led->pwm_duty_cycles->duty_ms, //para1
				20,  //para2
				1, //para3
				0,  //para4
				0, //para5
				PM8XXX_LED_PWM_FLAGS);
*/

static int zte_OnTime = 500;//PWM peroid
module_param_named(
	zte_OnTime, zte_OnTime, int, S_IRUGO | S_IWUSR | S_IWGRP
);

//static int zte_OffTime = 500;//PWM peroid
static int zte_OffTime = 7000;//zte liyuanfei for 8930-kk LED
module_param_named(
	zte_OffTime, zte_OffTime, int, S_IRUGO | S_IWUSR | S_IWGRP
);

static int zte_brightness = 2;//PWM peroid
module_param_named(
	zte_brightness, zte_brightness, int, S_IRUGO | S_IWUSR | S_IWGRP
);


static int para_duty=0;
static int para_start_idx=0;
static int para_idx_len=0;
static int para_low_pause=0;
static int para_hi_pause=0;

module_param_named(
	para_duty, para_duty, int, S_IRUGO | S_IWUSR | S_IWGRP
);
module_param_named(
	para_start_idx, para_start_idx, int, S_IRUGO | S_IWUSR | S_IWGRP
);
module_param_named(
	para_idx_len, para_idx_len, int, S_IRUGO | S_IWUSR | S_IWGRP
);
module_param_named(
	para_low_pause, para_low_pause, int, S_IRUGO | S_IWUSR | S_IWGRP
);
module_param_named(
	para_hi_pause, para_hi_pause, int, S_IRUGO | S_IWUSR | S_IWGRP
); 

#define SSBI_REG_ADDR_DRV_KEYPAD	0x48
#define PM8XXX_DRV_KEYPAD_BL_MASK	0xf0
#define PM8XXX_DRV_KEYPAD_BL_SHIFT	0x04

#define SSBI_REG_ADDR_FLASH_DRV0        0x49
#define PM8XXX_DRV_FLASH_MASK           0xf0
#define PM8XXX_DRV_FLASH_SHIFT          0x04

#define SSBI_REG_ADDR_FLASH_DRV1        0xFB

#define SSBI_REG_ADDR_LED_CTRL_BASE	0x131
#define SSBI_REG_ADDR_LED_CTRL(n)	(SSBI_REG_ADDR_LED_CTRL_BASE + (n))
#define PM8XXX_DRV_LED_CTRL_MASK	0xf8
#define PM8XXX_DRV_LED_CTRL_SHIFT	0x03

#define SSBI_REG_ADDR_WLED_CTRL_BASE	0x25A
#define SSBI_REG_ADDR_WLED_CTRL(n)	(SSBI_REG_ADDR_WLED_CTRL_BASE + (n) - 1)

/* wled control registers */
#define WLED_MOD_CTRL_REG		SSBI_REG_ADDR_WLED_CTRL(1)
#define WLED_MAX_CURR_CFG_REG(n)	SSBI_REG_ADDR_WLED_CTRL(n + 2)
#define WLED_BRIGHTNESS_CNTL_REG1(n)	SSBI_REG_ADDR_WLED_CTRL((2 * n) + 5)
#define WLED_BRIGHTNESS_CNTL_REG2(n)	SSBI_REG_ADDR_WLED_CTRL((2 * n) + 6)
#define WLED_SYNC_REG			SSBI_REG_ADDR_WLED_CTRL(11)
#define WLED_OVP_CFG_REG		SSBI_REG_ADDR_WLED_CTRL(13)
#define WLED_BOOST_CFG_REG		SSBI_REG_ADDR_WLED_CTRL(14)
#define WLED_HIGH_POLE_CAP_REG		SSBI_REG_ADDR_WLED_CTRL(16)

#define WLED_STRINGS			0x03
#define WLED_OVP_VAL_MASK		0x30
#define WLED_OVP_VAL_BIT_SHFT		0x04
#define WLED_BOOST_LIMIT_MASK		0xE0
#define WLED_BOOST_LIMIT_BIT_SHFT	0x05
#define WLED_BOOST_OFF			0x00
#define WLED_EN_MASK			0x01
#define WLED_CP_SELECT_MAX		0x03
#define WLED_CP_SELECT_MASK		0x03
#define WLED_DIG_MOD_GEN_MASK		0x70
#define WLED_CS_OUT_MASK		0x0E
#define WLED_CTL_DLY_STEP		200
#define WLED_CTL_DLY_MAX		1400
#define WLED_CTL_DLY_MASK		0xE0
#define WLED_CTL_DLY_BIT_SHFT		0x05
#define WLED_MAX_CURR			25
#define WLED_MAX_CURR_MASK		0x1F
#define WLED_OP_FDBCK_MASK		0x1C
#define WLED_OP_FDBCK_BIT_SHFT		0x02

#define WLED_MAX_LEVEL			255
#define WLED_8_BIT_MASK			0xFF
#define WLED_8_BIT_SHFT			0x08
#define WLED_MAX_DUTY_CYCLE		0xFFF

#define WLED_SYNC_VAL			0x07
#define WLED_SYNC_RESET_VAL		0x00
#define WLED_SYNC_MASK			0xF8

#define ONE_WLED_STRING			1
#define TWO_WLED_STRINGS		2
#define THREE_WLED_STRINGS		3

#define WLED_CABC_ONE_STRING		0x01
#define WLED_CABC_TWO_STRING		0x03
#define WLED_CABC_THREE_STRING		0x07

#define WLED_CABC_SHIFT			3

#define SSBI_REG_ADDR_RGB_CNTL1		0x12D
#define SSBI_REG_ADDR_RGB_CNTL2		0x12E

#define PM8XXX_DRV_RGB_RED_LED		BIT(2)
#define PM8XXX_DRV_RGB_GREEN_LED	BIT(1)
#define PM8XXX_DRV_RGB_BLUE_LED		BIT(0)

#define MAX_FLASH_LED_CURRENT		300
#define MAX_LC_LED_CURRENT		40
#define MAX_KP_BL_LED_CURRENT		300

#define PM8XXX_ID_LED_CURRENT_FACTOR	2  /* Iout = x * 2mA */
#define PM8XXX_ID_FLASH_CURRENT_FACTOR	20 /* Iout = x * 20mA */

#define PM8XXX_FLASH_MODE_DBUS1		1
#define PM8XXX_FLASH_MODE_DBUS2		2
#define PM8XXX_FLASH_MODE_PWM		3

#define MAX_LC_LED_BRIGHTNESS		20
#define MAX_FLASH_BRIGHTNESS		15
#define MAX_KB_LED_BRIGHTNESS		15

#define PM8XXX_LED_OFFSET(id) ((id) - PM8XXX_ID_LED_0)

#define PM8XXX_LED_PWM_FLAGS	(0x37)
//#define PM_PWM_LUT_LOOP			0x01
//#define PM_PWM_LUT_RAMP_UP		0x02
//#define PM_PWM_LUT_REVERSE			0x04
//#define PM_PWM_LUT_PAUSE_HI_EN	0x10
//#define PM_PWM_LUT_PAUSE_LO_EN	0x20

#define LED_MAP(_version, _kb, _led0, _led1, _led2, _flash_led0, _flash_led1, \
	_wled, _rgb_led_red, _rgb_led_green, _rgb_led_blue,_led_mpp3,_led_mpp4)\
	{\
		.version = _version,\
		.supported = _kb << PM8XXX_ID_LED_KB_LIGHT | \
			_led0 << PM8XXX_ID_LED_0 | _led1 << PM8XXX_ID_LED_1 | \
			_led2 << PM8XXX_ID_LED_2  | \
			_flash_led0 << PM8XXX_ID_FLASH_LED_0 | \
			_flash_led1 << PM8XXX_ID_FLASH_LED_1 | \
			_wled << PM8XXX_ID_WLED | \
			_rgb_led_red << PM8XXX_ID_RGB_LED_RED | \
			_rgb_led_green << PM8XXX_ID_RGB_LED_GREEN | \
			_rgb_led_blue << PM8XXX_ID_RGB_LED_BLUE | \
			_led_mpp3 << PM8XXX_ID_LED_MPP3 | \
			_led_mpp4 << PM8XXX_ID_LED_MPP4 , \
	}

/**
 * supported_leds - leds supported for each PMIC version
 * @version - version of PMIC
 * @supported - which leds are supported on version
 */

struct supported_leds {
	enum pm8xxx_version version;
	u32 supported;
};

static const struct supported_leds led_map[] = {
	LED_MAP(PM8XXX_VERSION_8058, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0),
	LED_MAP(PM8XXX_VERSION_8921, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0),
	LED_MAP(PM8XXX_VERSION_8018, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
	LED_MAP(PM8XXX_VERSION_8922, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0),
	LED_MAP(PM8XXX_VERSION_8038, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0),
};

/**
 * struct pm8xxx_led_data - internal led data structure
 * @led_classdev - led class device
 * @id - led index
 * @work - workqueue for led
 * @lock - to protect the transactions
 * @reg - cached value of led register
 * @pwm_dev - pointer to PWM device if LED is driven using PWM
 * @pwm_channel - PWM channel ID
 * @pwm_period_us - PWM period in micro seconds
 * @pwm_duty_cycles - struct that describes PWM duty cycles info
 */
struct pm8xxx_led_data {
	struct led_classdev	cdev;
	int			id;
	u8			reg;
	u8			wled_mod_ctrl_val;
	struct device		*dev;
	struct work_struct	work;
	struct mutex		lock;
	struct pwm_device	*pwm_dev;
	int			pwm_channel;
	u32			pwm_period_us;
	struct pm8xxx_pwm_duty_cycles *pwm_duty_cycles;
	struct wled_config_data *wled_cfg;
	int			max_current;
	int			blink;//zhengchao
};

static void led_kp_set(struct pm8xxx_led_data *led, enum led_brightness value)
{
	int rc;
	u8 level;

	level = (value << PM8XXX_DRV_KEYPAD_BL_SHIFT) & PM8XXX_DRV_KEYPAD_BL_MASK;
	led->reg &= ~PM8XXX_DRV_KEYPAD_BL_MASK;
	led->reg |= level;
	rc = pm8xxx_writeb(led->dev->parent, SSBI_REG_ADDR_DRV_KEYPAD,led->reg);
	if (rc < 0)
		dev_err(led->cdev.dev,"can't set keypad backlight level rc=%d\n", rc);
}

static void led_lc_set(struct pm8xxx_led_data *led, enum led_brightness value)
{
	int rc, offset;
	u8 level;

	level = (value << PM8XXX_DRV_LED_CTRL_SHIFT) &PM8XXX_DRV_LED_CTRL_MASK;
	offset = PM8XXX_LED_OFFSET(led->id);
	led->reg &= ~PM8XXX_DRV_LED_CTRL_MASK;
	led->reg |= level;

	rc = pm8xxx_writeb(led->dev->parent, SSBI_REG_ADDR_LED_CTRL(offset),led->reg);
	if (rc)
		dev_err(led->cdev.dev, "can't set (%d) led value rc=%d\n",led->id, rc);
}

static void led_flash_set(struct pm8xxx_led_data *led, enum led_brightness value)
{
	int rc;
	u8 level;
	u16 reg_addr;

	level = (value << PM8XXX_DRV_FLASH_SHIFT) & PM8XXX_DRV_FLASH_MASK;
	led->reg &= ~PM8XXX_DRV_FLASH_MASK;
	led->reg |= level;

	if (led->id == PM8XXX_ID_FLASH_LED_0)
		reg_addr = SSBI_REG_ADDR_FLASH_DRV0;
	else
		reg_addr = SSBI_REG_ADDR_FLASH_DRV1;

	rc = pm8xxx_writeb(led->dev->parent, reg_addr, led->reg);
	if (rc < 0)
		dev_err(led->cdev.dev, "can't set flash led%d level rc=%d\n", led->id, rc);
}

static int led_wled_set(struct pm8xxx_led_data *led, enum led_brightness new_value)
{
	int rc, duty,value;
	u8 val, i, num_wled_strings;
	
#if (defined(CONFIG_MACH_APOLLO) || defined(CONFIG_MACH_COEUS)||defined(CONFIG_MACH_NEX))	
	int change_flag=0;
#endif	

	value = new_value;
	if (value > WLED_MAX_LEVEL)
		value = WLED_MAX_LEVEL;

	if (value == 0) 
	{
		rc = pm8xxx_writeb(led->dev->parent, WLED_MOD_CTRL_REG,	WLED_BOOST_OFF);
		if (rc) 
		{
			dev_err(led->dev->parent, "can't write wled ctrl config register rc=%d\n", rc);
			return rc;
		}
	}
	else 
	{
		rc = pm8xxx_writeb(led->dev->parent, WLED_MOD_CTRL_REG,	led->wled_mod_ctrl_val);
		if (rc) 
		{
			dev_err(led->dev->parent, "can't write wled ctrl config register rc=%d\n", rc);
			return rc;
		}
	}

#if (defined(CONFIG_MACH_APOLLO) || defined(CONFIG_MACH_COEUS)||defined(CONFIG_MACH_NEX))
	if (value < 25) 
	{
		if (led->max_current != 5)
		{
			led->max_current =5;
			change_flag = 1;		
		}	
		value <<=2;
	}
	else
	{
		if (led->max_current != 20)
		{
			led->max_current =20;
			change_flag = 1;
		}
	}
	//printk("\n panmingdong final wled val =%d",value);
	if (change_flag )
	{
		change_flag = 0;
		rc = pm8xxx_readb(led->dev->parent,WLED_MAX_CURR_CFG_REG(0), &val);	
		if (rc)
		{
			dev_err(led->dev->parent, "can't read wled max current config register rc=%d\n", rc);	
			return rc;	
		}	
		if ((led->wled_cfg->ctrl_delay_us % WLED_CTL_DLY_STEP) ||(led->wled_cfg->ctrl_delay_us > WLED_CTL_DLY_MAX)) 
		{
			dev_err(led->dev->parent, "Invalid control delay\n");	
			return rc;	
		}	
	val = val / WLED_CTL_DLY_STEP;	
	val = (val & ~WLED_CTL_DLY_MASK) |(led->wled_cfg->ctrl_delay_us << WLED_CTL_DLY_BIT_SHFT);
	
	if ((led->max_current > WLED_MAX_CURR)) 
	{	
		dev_err(led->dev->parent, "Invalid max current\n");	
		return -EINVAL;	
	}	
	val = (val & ~WLED_MAX_CURR_MASK) | led->max_current;	
	rc = pm8xxx_writeb(led->dev->parent,WLED_MAX_CURR_CFG_REG(0), val);	
	if (rc)
	{
		dev_err(led->dev->parent, "can't write wled max current  config register rc=%d\n", rc);	
		return rc;	
	}
}
#endif

	duty = (WLED_MAX_DUTY_CYCLE * value) / WLED_MAX_LEVEL;
	num_wled_strings = led->wled_cfg->num_strings;
	/* program brightness control registers */
	for (i = 0; i < num_wled_strings; i++) {
		rc = pm8xxx_readb(led->dev->parent,WLED_BRIGHTNESS_CNTL_REG1(i), &val);
		if (rc) 
		{
			dev_err(led->dev->parent, "can't read wled brightnes ctrl register1 rc=%d\n", rc);
			return rc;
		}

		val = (val & ~WLED_MAX_CURR_MASK) | (duty >> WLED_8_BIT_SHFT);
		rc = pm8xxx_writeb(led->dev->parent,WLED_BRIGHTNESS_CNTL_REG1(i), val);
		if (rc) 
		{
			dev_err(led->dev->parent, "can't write wled brightness ctrl register1 rc=%d\n", rc);
			return rc;
		}

		val = duty & WLED_8_BIT_MASK;
		rc = pm8xxx_writeb(led->dev->parent,WLED_BRIGHTNESS_CNTL_REG2(i), val);
		if (rc) 
		{
			dev_err(led->dev->parent, "can't write wled brightness ctrl register2 rc=%d\n", rc);
			return rc;
		}
	}
	
	rc = pm8xxx_readb(led->dev->parent, WLED_SYNC_REG, &val);
	if (rc) 
	{
		dev_err(led->dev->parent, "can't read wled sync register rc=%d\n", rc);
		return rc;
	}
	/* sync */
	val &= WLED_SYNC_MASK;
	val |= WLED_SYNC_VAL;
	rc = pm8xxx_writeb(led->dev->parent, WLED_SYNC_REG, val);
	if (rc) 
	{
		dev_err(led->dev->parent,	"can't read wled sync register rc=%d\n", rc);
		return rc;
	}
	val &= WLED_SYNC_MASK;
	val |= WLED_SYNC_RESET_VAL;
	rc = pm8xxx_writeb(led->dev->parent, WLED_SYNC_REG, val);
	if (rc) 
	{
		dev_err(led->dev->parent,	"can't read wled sync register rc=%d\n", rc);
		return rc;
	}
	return 0;
}

static void wled_dump_regs(struct pm8xxx_led_data *led)
{
	int i;
	u8 val;

	for (i = 1; i < 17; i++) {
		pm8xxx_readb(led->dev->parent,
				SSBI_REG_ADDR_WLED_CTRL(i), &val);
		pr_debug("WLED_CTRL_%d = 0x%x\n", i, val);
	}
}

static void led_rgb_write(struct pm8xxx_led_data *led, u16 addr, enum led_brightness value)
{
	int rc;
	u8 val, mask;

	if (led->id != PM8XXX_ID_RGB_LED_BLUE &&
		led->id != PM8XXX_ID_RGB_LED_RED &&
		led->id != PM8XXX_ID_RGB_LED_GREEN)
		return;

	rc = pm8xxx_readb(led->dev->parent, addr, &val);
	if (rc) 
	{
		dev_err(led->cdev.dev, "can't read rgb ctrl register rc=%d\n",rc);
		return;
	}

	switch (led->id) {
	case PM8XXX_ID_RGB_LED_RED:
		mask = PM8XXX_DRV_RGB_RED_LED;
		break;
	case PM8XXX_ID_RGB_LED_GREEN:
		mask = PM8XXX_DRV_RGB_GREEN_LED;
		break;
	case PM8XXX_ID_RGB_LED_BLUE:
		mask = PM8XXX_DRV_RGB_BLUE_LED;
		break;
	default:
		return;
	}

	if (value)
		val |= mask;
	else
		val &= ~mask;

	rc = pm8xxx_writeb(led->dev->parent, addr, val);
	if (rc < 0)
		dev_err(led->cdev.dev, "can't set rgb led %d level rc=%d\n", led->id, rc);
}

static void led_rgb_set(struct pm8xxx_led_data *led, enum led_brightness value)
{
	if (value)
	{
		led_rgb_write(led, SSBI_REG_ADDR_RGB_CNTL1, value);
		//led_rgb_write(led, SSBI_REG_ADDR_RGB_CNTL2, value);
	} else 
	{
		//led_rgb_write(led, SSBI_REG_ADDR_RGB_CNTL2, value);
		led_rgb_write(led, SSBI_REG_ADDR_RGB_CNTL1, value);
	}
}
struct pm8xxx_mpp_config_data 		mpp_config={ 
		PM8XXX_MPP_TYPE_SINK, 
		PM8XXX_MPP_CS_OUT_5MA, 
		PM8XXX_MPP_CS_CTRL_DISABLE
		} ;
#define PM8038_MPP_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + 152+38)

static void
led_mpp_set(struct pm8xxx_led_data *led, enum led_brightness value)
{

	printk(LED_TAG "led_mpp_set : %s %d(1-5MA, 2-10MA, 3-15MA, 4-20MA, 5-25MA, 6-30MA, 7-35MA, 8-40MA)\n", led->cdev.name, value);
	if (value==0)
	{
		mpp_config.control = PM8XXX_MPP_CS_CTRL_DISABLE;
	}
	else
	{
		mpp_config.control = PM8XXX_MPP_CS_CTRL_ENABLE;
		if(value<=8)
			mpp_config.level = value-1;
		else
			mpp_config.level = PM8XXX_MPP_CS_OUT_40MA;//7		
	}

	pm8xxx_mpp_config(PM8038_MPP_PM_TO_SYS(led->id-8),&mpp_config);
}



static int pm8xxx_led_pwm_work(struct pm8xxx_led_data *led)
{	
	int duty_us;
	int rc = 0;
		
	if (led->pwm_duty_cycles == NULL) 
	{
		duty_us = (led->pwm_period_us * led->cdev.brightness) /LED_FULL;		
		rc = pwm_config(led->pwm_dev, duty_us, led->pwm_period_us);
		printk(LED_TAG "bright=%d; duty_us=%d; period=%d\n",led->cdev.brightness,duty_us,led->pwm_period_us);	//zte-ccb-20130110

		if (led->cdev.brightness) 
		{	
			led_rgb_write(led, SSBI_REG_ADDR_RGB_CNTL1,led->cdev.brightness);	
			rc = pwm_enable(led->pwm_dev);	
		} 
		else 
		{	
			pwm_disable(led->pwm_dev);	
			led_rgb_write(led, SSBI_REG_ADDR_RGB_CNTL1,led->cdev.brightness);	
		}		
	} 
	else 
	{	
		if (led->blink == 1) 
		{//blink
				para_duty=1;
				para_start_idx=0;//start index
				para_idx_len=zte_brightness;
				para_low_pause=zte_OffTime;
				para_hi_pause=zte_OnTime;
		}
		else if (led->blink == 0) 
		{//always on
				para_duty=1;
				para_start_idx=0;//start index
				para_idx_len=zte_brightness;
				para_low_pause=0;
				para_hi_pause=1000;//high pause time
		}
		else
		{
			//all para_xxx value set by adb command. easy to debug.
		}
		
		printk(LED_TAG "brightness=%d blink=%d duty=%d start=%d len=%d low=%d hi=%d. %s\n",led->cdev.brightness,

		led->blink,para_duty,para_start_idx,para_idx_len,para_low_pause,para_hi_pause,led->cdev.name);

		pm8xxx_pwm_lut_enable(led->pwm_dev, 0);			
		pm8xxx_pwm_lut_config(led->pwm_dev, led->pwm_period_us,led->pwm_duty_cycles->duty_pcts,	
				para_duty,
				para_start_idx,para_idx_len, 
				para_low_pause, para_hi_pause,	
				PM8XXX_LED_PWM_FLAGS);
		//0x37 means support ramp up and down ,enable pause timer
		
		if (led->cdev.brightness)		
			led_rgb_write(led, SSBI_REG_ADDR_RGB_CNTL1,led->cdev.brightness);
		
		rc = pm8xxx_pwm_lut_enable(led->pwm_dev, led->cdev.brightness);
		
		if (!led->cdev.brightness)		
			led_rgb_write(led, SSBI_REG_ADDR_RGB_CNTL1,	led->cdev.brightness);
		
	}		
	return rc;
}


static void __pm8xxx_led_work(struct pm8xxx_led_data *led,enum led_brightness level)
{	
	int rc;	
	mutex_lock(&led->lock);
		
	switch (led->id) {	
	case PM8XXX_ID_LED_KB_LIGHT:	
		led_kp_set(led, level);	
		break;	
	case PM8XXX_ID_LED_0:	
	case PM8XXX_ID_LED_1:	
	case PM8XXX_ID_LED_2:	
		led_lc_set(led, level);	
		break;	
	case PM8XXX_ID_FLASH_LED_0:	
	case PM8XXX_ID_FLASH_LED_1:	
		led_flash_set(led, level);	
		break;	
	case PM8XXX_ID_WLED:
		rc = led_wled_set(led, level);	
		if (rc < 0)	
			pr_err("wled brightness set failed %d\n", rc);	
		break;
		
	case PM8XXX_ID_RGB_LED_RED:	
	case PM8XXX_ID_RGB_LED_GREEN:	
	case PM8XXX_ID_RGB_LED_BLUE:	
		led_rgb_set(led, level);	
		break;	
	case PM8XXX_ID_LED_MPP3:	
	case PM8XXX_ID_LED_MPP4:	
		led_mpp_set(led,level);	
		break;	
	default:	
		dev_err(led->cdev.dev, "unknown led id %d", led->id);	
		break;	
	}

		
	mutex_unlock(&led->lock);
	}


static void pm8xxx_led_work(struct work_struct *work)
{
	int rc;
	struct pm8xxx_led_data *led = container_of(work,struct pm8xxx_led_data, work);
		
	if (led->pwm_dev == NULL) {	
		__pm8xxx_led_work(led, led->cdev.brightness);	
	} 
	else 
	{	
		rc = pm8xxx_led_pwm_work(led);	
		if (rc)				
			pr_err("could not configure PWM mode for LED:%d\n",led->id);		
	}
}


static void pm8xxx_led_set(struct led_classdev *led_cdev,enum led_brightness value)
{
	struct	pm8xxx_led_data *led;	
	led = container_of(led_cdev, struct pm8xxx_led_data, cdev);	
	if (value < LED_OFF || value > led->cdev.max_brightness) 
	{	
		dev_err(led->cdev.dev, "Invalid brightness value exceeds");	
		return;	
	}
		
	led->cdev.brightness = value;	
	schedule_work(&led->work);
}

static int pm8xxx_set_led_mode_and_max_brightness(struct pm8xxx_led_data *led,enum pm8xxx_led_modes led_mode, int max_current)
{	
	switch (led->id) {	
	case PM8XXX_ID_LED_0:	
	case PM8XXX_ID_LED_1:	
	case PM8XXX_ID_LED_2:	
	case PM8XXX_ID_LED_MPP3:	
	case PM8XXX_ID_LED_MPP4:	
		led->cdev.max_brightness = max_current /PM8XXX_ID_LED_CURRENT_FACTOR;	
		if (led->cdev.max_brightness > MAX_LC_LED_BRIGHTNESS)	
			led->cdev.max_brightness = MAX_LC_LED_BRIGHTNESS;
		
		led->reg = led_mode;	
		break;
		
	case PM8XXX_ID_LED_KB_LIGHT:	
	case PM8XXX_ID_FLASH_LED_0:	
	case PM8XXX_ID_FLASH_LED_1:	
		led->cdev.max_brightness = max_current /PM8XXX_ID_FLASH_CURRENT_FACTOR;
		
		if (led->cdev.max_brightness > MAX_FLASH_BRIGHTNESS)	
			led->cdev.max_brightness = MAX_FLASH_BRIGHTNESS;
		
		switch (led_mode) {	
		case PM8XXX_LED_MODE_PWM1:	
		case PM8XXX_LED_MODE_PWM2:	
		case PM8XXX_LED_MODE_PWM3:	
			led->reg = PM8XXX_FLASH_MODE_PWM;	
			break;	
		case PM8XXX_LED_MODE_DTEST1:	
			led->reg = PM8XXX_FLASH_MODE_DBUS1;	
			break;	
		case PM8XXX_LED_MODE_DTEST2:	
			led->reg = PM8XXX_FLASH_MODE_DBUS2;	
			break;	
		default:	
			led->reg = PM8XXX_LED_MODE_MANUAL;	
			break;	
		}	
		break;	
	case PM8XXX_ID_WLED:	
		led->cdev.max_brightness = WLED_MAX_LEVEL;	
		break;	
	case PM8XXX_ID_RGB_LED_RED:	
	case PM8XXX_ID_RGB_LED_GREEN:	
	case PM8XXX_ID_RGB_LED_BLUE:
		led->cdev.max_brightness = LED_FULL;	
		break;	
	default:	
		dev_err(led->cdev.dev, "LED Id is invalid");	
		return -EINVAL;	
	}
	return 0;
}

static enum led_brightness pm8xxx_led_get(struct led_classdev *led_cdev)
{	
	struct pm8xxx_led_data *led;
	led = container_of(led_cdev, struct pm8xxx_led_data, cdev);
	return led->cdev.brightness;
}

static int __devinit init_wled(struct pm8xxx_led_data *led)
{	
	int rc, i;	
	u8 val, num_wled_strings;

	num_wled_strings = led->wled_cfg->num_strings;	
	/* program over voltage protection threshold */	
	if (led->wled_cfg->ovp_val > WLED_OVP_37V) 
	{	
		dev_err(led->dev->parent, "Invalid ovp value");	
		return -EINVAL;	
	}
	
	rc = pm8xxx_readb(led->dev->parent, WLED_OVP_CFG_REG, &val);	
	if (rc) 
	{	
		dev_err(led->dev->parent, "can't read wled ovp config  register rc=%d\n", rc);	
		return rc;		
	}
	
	val = (val & ~WLED_OVP_VAL_MASK) |(led->wled_cfg->ovp_val << WLED_OVP_VAL_BIT_SHFT);

	
	rc = pm8xxx_writeb(led->dev->parent, WLED_OVP_CFG_REG, val);	
	if (rc) 
	{	
		dev_err(led->dev->parent, "can't write wled ovp config register rc=%d\n", rc);	
		return rc;	
	}

		
	/* program current boost limit and output feedback*/	
	if (led->wled_cfg->boost_curr_lim > WLED_CURR_LIMIT_1680mA) 
	{
		dev_err(led->dev->parent, "Invalid boost current limit");	
		return -EINVAL;	
	}
		
	rc = pm8xxx_readb(led->dev->parent, WLED_BOOST_CFG_REG, &val);	
	if (rc)
	{	
		dev_err(led->dev->parent, "can't read wled boost config register rc=%d\n", rc);	
		return rc;	
	}
		
	val = (val & ~WLED_BOOST_LIMIT_MASK) |(led->wled_cfg->boost_curr_lim << WLED_BOOST_LIMIT_BIT_SHFT);	
	val = (val & ~WLED_OP_FDBCK_MASK) |	(led->wled_cfg->op_fdbck << WLED_OP_FDBCK_BIT_SHFT);	
	rc = pm8xxx_writeb(led->dev->parent, WLED_BOOST_CFG_REG, val);	
	if (rc) {	
		dev_err(led->dev->parent, "can't write wled boost config register rc=%d\n", rc);	
		return rc;	
	}
		
	/* program high pole capacitance */	
	if (led->wled_cfg->cp_select > WLED_CP_SELECT_MAX) 
	{	
		dev_err(led->dev->parent, "Invalid pole capacitance");	
		return -EINVAL;	
	}
		
	rc = pm8xxx_readb(led->dev->parent, WLED_HIGH_POLE_CAP_REG, &val);	
	if (rc) 
	{
		dev_err(led->dev->parent, "can't read wled high pole capacitance register rc=%d\n", rc);	
		return rc;	
	}
	
	val = (val & ~WLED_CP_SELECT_MASK) | led->wled_cfg->cp_select;	
	rc = pm8xxx_writeb(led->dev->parent, WLED_HIGH_POLE_CAP_REG, val);	
	if (rc)
	{	
		dev_err(led->dev->parent, "can't write wled high pole capacitance register rc=%d\n", rc);	
		return rc;	
	}
		
	/* program activation delay and maximum current */		
	for (i = 0; i < num_wled_strings; i++) {
	
	rc = pm8xxx_readb(led->dev->parent,WLED_MAX_CURR_CFG_REG(i), &val);	
	if (rc)
	{	
		dev_err(led->dev->parent, "can't read wled max current config register rc=%d\n", rc);	
		return rc;	
	}
	
	if ((led->wled_cfg->ctrl_delay_us % WLED_CTL_DLY_STEP) ||(led->wled_cfg->ctrl_delay_us > WLED_CTL_DLY_MAX))
	{	
		dev_err(led->dev->parent, "Invalid control delay\n");	
		return rc;	
	}

	val = val / WLED_CTL_DLY_STEP;	
	val = (val & ~WLED_CTL_DLY_MASK) |(led->wled_cfg->ctrl_delay_us << WLED_CTL_DLY_BIT_SHFT);

	
	if ((led->max_current > WLED_MAX_CURR)) {	
		dev_err(led->dev->parent, "Invalid max current\n");	
		return -EINVAL;	
	}
	
	val = (val & ~WLED_MAX_CURR_MASK) | led->max_current;	
	rc = pm8xxx_writeb(led->dev->parent,WLED_MAX_CURR_CFG_REG(i), val);	
	if (rc) 
	{	
		dev_err(led->dev->parent, "can't write wled max current config register rc=%d\n", rc);	
		return rc;	
	}
	
}

	
if (led->wled_cfg->cabc_en) {
	
	rc = pm8xxx_readb(led->dev->parent, WLED_SYNC_REG, &val);
	
	if (rc) {
	
		dev_err(led->dev->parent,
	
			"can't read cabc register rc=%d\n", rc);
	
		return rc;
	
	}

	
	switch (num_wled_strings) {	
	case ONE_WLED_STRING:	
		val |= (WLED_CABC_ONE_STRING << WLED_CABC_SHIFT);	
		break;	
	case TWO_WLED_STRINGS:	
		val |= (WLED_CABC_TWO_STRING << WLED_CABC_SHIFT);	
		break;	
	case THREE_WLED_STRINGS:	
		val |= (WLED_CABC_THREE_STRING << WLED_CABC_SHIFT);	
		break;	
	default:	
		break;	
	}	
	
	rc = pm8xxx_writeb(led->dev->parent, WLED_SYNC_REG, val);	
	if (rc) 
	{	
		dev_err(led->dev->parent,	"can't write to enable cabc rc=%d\n", rc);	
		return rc;	
	}	
}

	
/* program digital module generator, cs out and enable the module */	
rc = pm8xxx_readb(led->dev->parent, WLED_MOD_CTRL_REG, &val);	
if (rc) {
	dev_err(led->dev->parent, "can't read wled module ctrlregister rc=%d\n", rc);	
	return rc;	
}
	
if (led->wled_cfg->dig_mod_gen_en)	
	val |= WLED_DIG_MOD_GEN_MASK;
	
if (led->wled_cfg->cs_out_en)	
	val |= WLED_CS_OUT_MASK;
	
val |= WLED_EN_MASK;

	
rc = pm8xxx_writeb(led->dev->parent, WLED_MOD_CTRL_REG, val);	
if (rc) {	
	dev_err(led->dev->parent, "can't write wled module ctrl register rc=%d\n", rc);	
	return rc;	
}	
led->wled_mod_ctrl_val = val;
rc = pm8xxx_writeb(led->dev->parent, 0x268, 0x3F); 	
if (rc) { 	
	dev_err(led->dev->parent, "can't write register 0x268 value register rc=%d\n", rc); 	
	return rc; 
} 
	
/* dump wled registers */	
wled_dump_regs(led);	
return 0;
}


static int __devinit get_init_value(struct pm8xxx_led_data *led, u8 *val)
{	
	int rc, offset;
	u16 addr;
	
	switch (led->id) {	
	case PM8XXX_ID_LED_KB_LIGHT:	
		addr = SSBI_REG_ADDR_DRV_KEYPAD;
		break;	
	case PM8XXX_ID_LED_0:	
	case PM8XXX_ID_LED_1:	
	case PM8XXX_ID_LED_2:	
		offset = PM8XXX_LED_OFFSET(led->id);	
		addr = SSBI_REG_ADDR_LED_CTRL(offset);	
		break;	
	case PM8XXX_ID_FLASH_LED_0:	
		addr = SSBI_REG_ADDR_FLASH_DRV0;	
		break;	
	case PM8XXX_ID_FLASH_LED_1:	
		addr = SSBI_REG_ADDR_FLASH_DRV1;	
		break;	
	case PM8XXX_ID_WLED:
		rc = init_wled(led);	
		if (rc)	
			dev_err(led->cdev.dev, "can't initialize wled rc=%d\n",rc);	
		return rc;	
	case PM8XXX_ID_RGB_LED_RED:	
	case PM8XXX_ID_RGB_LED_GREEN:	
	case PM8XXX_ID_RGB_LED_BLUE:	
		addr = SSBI_REG_ADDR_RGB_CNTL1;	
		break;
		
	case PM8XXX_ID_LED_MPP3:	
	case PM8XXX_ID_LED_MPP4:	
		dev_err(led->cdev.dev, "MPP LED ID %d", led->id);	
		return 0;	
	default:	
		dev_err(led->cdev.dev, "unknown led id %d", led->id);	
		return -EINVAL;	
	}

	rc = pm8xxx_readb(led->dev->parent, addr, val);		
	if (rc)	
		dev_err(led->cdev.dev, "can't get led(%d) level rc=%d\n",led->id, rc);	
	return rc;
}


static int pm8xxx_led_pwm_configure(struct pm8xxx_led_data *led)
{
	int start_idx, idx_len, duty_us, rc;	
	led->pwm_dev = pwm_request(led->pwm_channel,led->cdev.name);
		
	if (IS_ERR_OR_NULL(led->pwm_dev)) 
	{	
		pr_err("could not acquire PWM Channel %d, error %ld\n", led->pwm_channel,PTR_ERR(led->pwm_dev));
		led->pwm_dev = NULL;	
		return -ENODEV;	
	}	
	
	if (led->pwm_duty_cycles != NULL) 
	{	
		start_idx = led->pwm_duty_cycles->start_idx;	
		idx_len = led->pwm_duty_cycles->num_duty_pcts;
		
			if (idx_len >= PM_PWM_LUT_SIZE && start_idx) 
			{
				pr_err("Wrong LUT size or index\n");	
				return -EINVAL;	
			}
		
			if ((start_idx + idx_len) > PM_PWM_LUT_SIZE) 
			{	
				pr_err("Exceed LUT limit\n");	
				return -EINVAL;	
			}
		
		rc = pm8xxx_pwm_lut_config(led->pwm_dev, led->pwm_period_us,	
					led->pwm_duty_cycles->duty_pcts,
					led->pwm_duty_cycles->duty_ms,
					start_idx, idx_len, 0, 0,
					PM8XXX_LED_PWM_FLAGS);
	}
	else
	{
		duty_us = led->pwm_period_us;
		rc = pwm_config(led->pwm_dev, duty_us, led->pwm_period_us);
	}

	return rc;
}

static ssize_t blink_show(struct device *dev,
		struct device_attribute *attr, char *buf)
{
	ssize_t ret = 0;

	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	
	struct pm8xxx_led_data *led;
	led = container_of(led_cdev, struct pm8xxx_led_data, cdev);

	sprintf(buf, "%d \n", led->blink);
	ret = strlen(buf) + 1;

	return ret;
}

static ssize_t blink_store(struct device *dev,struct device_attribute *attr, const char *buf, size_t size)
{
	char *after;
	unsigned int state;
	ssize_t ret = -EINVAL;
	size_t count;

	struct led_classdev *led_cdev = dev_get_drvdata(dev);
	
	struct pm8xxx_led_data *led;
	led = container_of(led_cdev, struct pm8xxx_led_data, cdev);
	
	state = simple_strtoul(buf, &after, 10);
	count = after - buf;

	if (*after && isspace(*after))
		count++;

	if (count == size) 
	{
		ret = count;
		if (state!=0) 
			led->blink = 1;
		else
			led->blink = 0;
	}
	
	printk(LED_TAG "blink_store: %s blink=%d \n",led_cdev->name,led->blink);	
	return ret;
}



static DEVICE_ATTR(blink, 0644, blink_show, blink_store);

static int __devinit pm8xxx_led_probe(struct platform_device *pdev)
{
	const struct pm8xxx_led_platform_data *pdata = pdev->dev.platform_data;
	const struct led_platform_data *pcore_data;
	struct led_info *curr_led;
	struct pm8xxx_led_data *led, *led_dat;
	struct pm8xxx_led_config *led_cfg;
	enum pm8xxx_version version;
	bool found = false;
	int rc, i, j;

	if (pdata == NULL) {
		dev_err(&pdev->dev, "platform data not supplied\n");
		return -EINVAL;
	}

	pcore_data = pdata->led_core;

	if (pcore_data->num_leds != pdata->num_configs) {
		dev_err(&pdev->dev, "#no. of led configs and #no. of led"
				"entries are not equal\n");
		return -EINVAL;
	}

	led = kcalloc(pcore_data->num_leds, sizeof(*led), GFP_KERNEL);
	if (led == NULL) {
		dev_err(&pdev->dev, "failed to alloc memory\n");
		return -ENOMEM;
	}

	for (i = 0; i < pcore_data->num_leds; i++)
	{
		curr_led	= &pcore_data->leds[i];
		led_dat		= &led[i];
		led_cfg		= &pdata->configs[i];
		led_dat->id     = led_cfg->id;
		led_dat->pwm_channel = led_cfg->pwm_channel;
		led_dat->pwm_period_us = led_cfg->pwm_period_us;
		led_dat->pwm_duty_cycles = led_cfg->pwm_duty_cycles;
		led_dat->wled_cfg = led_cfg->wled_cfg;
		led_dat->max_current = led_cfg->max_current;

		if (!((led_dat->id >= PM8XXX_ID_LED_KB_LIGHT) &&(led_dat->id < PM8XXX_ID_MAX))) 
		{
			dev_err(&pdev->dev, "invalid LED ID(%d) specified\n",led_dat->id);
			rc = -EINVAL;
			goto fail_id_check;
		}

		found = false;
		version = pm8xxx_get_version(pdev->dev.parent);
		for (j = 0; j < ARRAY_SIZE(led_map); j++) {
			if (version == led_map[j].version
			&& (led_map[j].supported & (1 << led_dat->id))) {
				found = true;
				break;
			}
		}

		if (!found) 
		{
			dev_err(&pdev->dev, "invalid LED ID(%d) specified\n",
				led_dat->id);
			rc = -EINVAL;
			goto fail_id_check;
		}

		led_dat->cdev.name		= curr_led->name;
		led_dat->cdev.default_trigger   = curr_led->default_trigger;
		led_dat->cdev.brightness_set    = pm8xxx_led_set;
		led_dat->cdev.brightness_get    = pm8xxx_led_get;
		led_dat->cdev.brightness	= 35;  //according to the new curve LED_HALF;	//LED_OFF;
		led_dat->cdev.flags		= curr_led->flags;
		led_dat->dev			= &pdev->dev;

		rc =  get_init_value(led_dat, &led_dat->reg);
		if (rc < 0)
			goto fail_id_check;

		rc = pm8xxx_set_led_mode_and_max_brightness(led_dat,
					led_cfg->mode, led_cfg->max_current);
		if (rc < 0)
			goto fail_id_check;

		mutex_init(&led_dat->lock);
		INIT_WORK(&led_dat->work, pm8xxx_led_work);

		rc = led_classdev_register(&pdev->dev, &led_dat->cdev);
		if (rc) 
		{
			dev_err(&pdev->dev, "unable to register led %d,rc=%d\n",led_dat->id, rc);
			goto fail_id_check;
		}
		
		rc =   device_create_file(led_dat->cdev.dev, &dev_attr_blink);
		if (rc) 
		{
			printk(LED_TAG "DBG device_create_file blink for %s led failed\n",led_dat->cdev.name);
		}

		/* configure default state */
		if (led_cfg->default_state)
			led->cdev.brightness = led_dat->cdev.max_brightness;
		else			
			#if(defined (CONFIG_FB_MSM_MIPI_WARPLTE_OTM_VIDEO_WVGA_PT_PANEL)||defined (CONFIG_MACH_HERA)||defined (CONFIG_MACH_IRIS) || defined (CONFIG_MACH_OCEANUS)|| defined (CONFIG_MACH_STORMER))
				led->cdev.brightness = LED_OFF;
			#else
				led->cdev.brightness = 35;  //according to the new curve LED_HALF;
			#endif

		printk(LED_TAG "%s defaultState=%d br=%d \n",led_dat->cdev.name,led_cfg->default_state,led->cdev.brightness);//zte


		if (led_cfg->mode != PM8XXX_LED_MODE_MANUAL) 
		{
			if (led_dat->id == PM8XXX_ID_RGB_LED_RED ||led_dat->id == PM8XXX_ID_RGB_LED_GREEN ||led_dat->id == PM8XXX_ID_RGB_LED_BLUE)
				__pm8xxx_led_work(led_dat, 0);
			else
				__pm8xxx_led_work(led_dat,led_dat->cdev.max_brightness);

			if (led_dat->pwm_channel != -1) 
			{
				led_dat->cdev.max_brightness = LED_FULL;
				rc = pm8xxx_led_pwm_configure(led_dat);
				if (rc) 
				{
					dev_err(&pdev->dev, "failed to configure LED, error: %d\n", rc);
					goto fail_id_check;
				}				
				schedule_work(&led->work);
			}
		}
		else 
		{
			if(led_dat->id == PM8XXX_ID_LED_MPP3)
				__pm8xxx_led_work(led_dat, 0);
			else
				__pm8xxx_led_work(led_dat, led->cdev.brightness);
		}
	}

	platform_set_drvdata(pdev, led);
	return 0;

fail_id_check:
	if (i > 0) {
		for (i = i - 1; i >= 0; i--) {
			mutex_destroy(&led[i].lock);
			led_classdev_unregister(&led[i].cdev);
			if (led[i].pwm_dev != NULL)
				pwm_free(led[i].pwm_dev);
		}
	}
	kfree(led);
	return rc;
}

static int __devexit pm8xxx_led_remove(struct platform_device *pdev)
{
	int i;
	const struct led_platform_data *pdata =	pdev->dev.platform_data;
	struct pm8xxx_led_data *led = platform_get_drvdata(pdev);

	for (i = 0; i < pdata->num_leds; i++) {
		cancel_work_sync(&led[i].work);
		mutex_destroy(&led[i].lock);
		led_classdev_unregister(&led[i].cdev);
		if (led[i].pwm_dev != NULL)
			pwm_free(led[i].pwm_dev);
	}
	kfree(led);
	return 0;
}

static struct platform_driver pm8xxx_led_driver = {
	.probe		= pm8xxx_led_probe,
	.remove		= __devexit_p(pm8xxx_led_remove),
	.driver		= {
		.name	= PM8XXX_LEDS_DEV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init pm8xxx_led_init(void)
{
	return platform_driver_register(&pm8xxx_led_driver);
}
subsys_initcall(pm8xxx_led_init);

static void __exit pm8xxx_led_exit(void)
{
	platform_driver_unregister(&pm8xxx_led_driver);
}
module_exit(pm8xxx_led_exit);

MODULE_DESCRIPTION("PM8XXX LEDs driver");
MODULE_LICENSE("GPL v2");
MODULE_VERSION("1.0");
MODULE_ALIAS("platform:pm8xxx-led");

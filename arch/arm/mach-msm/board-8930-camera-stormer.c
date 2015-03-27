/* Copyright (c) 2011-2012, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

/*
 * Created by ZTE_JIA_20140311 jia.jia
 */

#include <asm/mach-types.h>
#include <linux/gpio.h>
#include <mach/socinfo.h>
#include <mach/camera.h>
#include <mach/msm_bus_board.h>
#include <mach/gpiomux.h>
#include "devices.h"
#include "board-8930.h"

#ifdef CONFIG_MSM_CAMERA

#if (defined(CONFIG_GPIO_SX150X) || defined(CONFIG_GPIO_SX150X_MODULE)) && \
	defined(CONFIG_I2C)

static struct i2c_board_info cam_expander_i2c_info[] = {
	{
		I2C_BOARD_INFO("sx1508q", 0x22),
		.platform_data = &msm8930_sx150x_data[SX150X_CAM]
	},
};

static struct msm_cam_expander_info cam_expander_info[] = {
	{
		cam_expander_i2c_info,
		MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	},
};
#endif

static struct gpiomux_setting cam_settings[] = {
	{
		.func = GPIOMUX_FUNC_GPIO, /*suspend*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_DOWN,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 1*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*active 2*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_1, /*active 3*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_NONE,
	},

	{
		.func = GPIOMUX_FUNC_5, /*active 4*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_6, /*active 5*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_2, /*active 6*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_3, /*active 7*/
		.drv = GPIOMUX_DRV_8MA,
		.pull = GPIOMUX_PULL_UP,
	},

	{
		.func = GPIOMUX_FUNC_GPIO, /*i2c suspend*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_KEEPER,
	},
	{
		.func = GPIOMUX_FUNC_2, /*active 9*/
		.drv = GPIOMUX_DRV_2MA,
		.pull = GPIOMUX_PULL_NONE,
	},

};

#if defined(CONFIG_OV8835_BAYER) || defined(CONFIG_OV9740)
static struct msm_gpiomux_config msm8930_cam_common_configs[] = {
	{
		.gpio = 4,  // MCLK, Front-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[9],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 5,  // MCLK, Back-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 9,  // DVDD
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 15,  // VAAM
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 18,  // Flash Strobe
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 53,  // Standby, Front-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 54,  // Standby, Back-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 55,  // AVDD
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 76,  // Reset, Front-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 91,  // Flash Torch Mode
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 107,  // Reset, Back-end
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
};
#else
static struct msm_gpiomux_config msm8930_cam_common_configs[] = {
	{
		.gpio = 2,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 3,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 4,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[9],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 5,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[1],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 76,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 107,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
	{
		.gpio = 54,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
};
#endif /* defined(CONFIG_OV8835_BAYER) || defined(CONFIG_OV9740) */

static struct msm_gpiomux_config msm8930_evt_cam_configs[] = {
	{
		.gpio = 75,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[2],
			[GPIOMUX_SUSPENDED] = &cam_settings[0],
		},
	},
};

#if defined(CONFIG_OV8835_BAYER) || defined(CONFIG_OV9740)
static struct msm_gpiomux_config msm8930_cam_2d_configs[] = {
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
};
#else
static struct msm_gpiomux_config msm8930_cam_2d_configs[] = {
	{
		.gpio = 18,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	{
		.gpio = 19,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	{
		.gpio = 20,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	{
		.gpio = 21,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
};
#endif /* defined(CONFIG_OV8835_BAYER) || defined(CONFIG_OV9740) */

static struct msm_gpiomux_config msm8930_evt_cam_2d_configs[] = {
	{
		.gpio = 36,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
	{
		.gpio = 37,
		.settings = {
			[GPIOMUX_ACTIVE]    = &cam_settings[3],
			[GPIOMUX_SUSPENDED] = &cam_settings[8],
		},
	},
};

#define VFE_CAMIF_TIMER1_GPIO 2
#define VFE_CAMIF_TIMER2_GPIO 3
#define VFE_CAMIF_TIMER3_GPIO_INT 4
static struct msm_camera_sensor_strobe_flash_data strobe_flash_xenon = {
	.flash_trigger = VFE_CAMIF_TIMER2_GPIO,
	.flash_charge = VFE_CAMIF_TIMER1_GPIO,
	.flash_charge_done = VFE_CAMIF_TIMER3_GPIO_INT,
	.flash_recharge_duration = 50000,
	.irq = MSM_GPIO_TO_INT(VFE_CAMIF_TIMER3_GPIO_INT),
};

#ifdef CONFIG_MSM_CAMERA_FLASH
static struct msm_camera_sensor_flash_src msm_flash_src = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = VFE_CAMIF_TIMER1_GPIO,
	._fsrc.ext_driver_src.led_flash_en = VFE_CAMIF_TIMER2_GPIO,
	._fsrc.ext_driver_src.flash_id = MAM_CAMERA_EXT_LED_FLASH_TPS61310,
};

static struct msm_camera_sensor_flash_src msm_flash_src_led = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_LED1,
	._fsrc.ext_driver_src.led_en = VFE_CAMIF_TIMER1_GPIO,
	._fsrc.ext_driver_src.led_flash_en = VFE_CAMIF_TIMER2_GPIO,
};

static struct msm_camera_sensor_flash_src msm_flash_src_lm3642 = {
	.flash_sr_type = MSM_CAMERA_FLASH_SRC_EXT,
	._fsrc.ext_driver_src.led_en = 91, // Flash Torch Mode
	._fsrc.ext_driver_src.led_flash_en = 18, // Flash Strobe
	._fsrc.ext_driver_src.flash_id = MAM_CAMERA_EXT_LED_FLASH_LM3642,
};
#endif

static struct msm_bus_vectors cam_init_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_preview_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 27648000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_video_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 800000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
};

static struct msm_bus_vectors cam_snapshot_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 600000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_zsl_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 800000000,
		.ib  = 2656000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 0,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_video_ls_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 800000000,
		.ib  = 3522000000UL,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 0,
		.ib  = 1350000000,
	},
};

static struct msm_bus_vectors cam_dual_vectors[] = {
	{
		.src = MSM_BUS_MASTER_VFE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 302071680,
		.ib  = 1208286720,
	},
	{
		.src = MSM_BUS_MASTER_VPE,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 206807040,
		.ib  = 488816640,
	},
	{
		.src = MSM_BUS_MASTER_JPEG_ENC,
		.dst = MSM_BUS_SLAVE_EBI_CH0,
		.ab  = 540000000,
		.ib  = 1350000000,
	},
};


static struct msm_bus_paths cam_bus_client_config[] = {
	{
		ARRAY_SIZE(cam_init_vectors),
		cam_init_vectors,
	},
	{
		ARRAY_SIZE(cam_preview_vectors),
		cam_preview_vectors,
	},
	{
		ARRAY_SIZE(cam_video_vectors),
		cam_video_vectors,
	},
	{
		ARRAY_SIZE(cam_snapshot_vectors),
		cam_snapshot_vectors,
	},
	{
		ARRAY_SIZE(cam_zsl_vectors),
		cam_zsl_vectors,
	},
	{
		ARRAY_SIZE(cam_video_ls_vectors),
		cam_video_ls_vectors,
	},
	{
		ARRAY_SIZE(cam_dual_vectors),
		cam_dual_vectors,
	},
};

static struct msm_bus_scale_pdata cam_bus_client_pdata = {
		cam_bus_client_config,
		ARRAY_SIZE(cam_bus_client_config),
		.name = "msm_camera",
};

static struct msm_camera_device_platform_data msm_camera_csi_device_data[] = {
	{
		.csid_core = 0,
		.is_vpe    = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
	{
		.csid_core = 1,
		.is_vpe    = 1,
		.cam_bus_scale_table = &cam_bus_client_pdata,
	},
};

#if defined(CONFIG_OV8835_BAYER) || defined(CONFIG_OV9740)
/*
 * 'cam_vio'/'cam_vana'/'cam_vdig'/'cam_vaf' DISUSED here
 * 'cam_vio' connected to 'VREG_L11_1P8'
 * 'cam_vdig' connected to 'VREG_L11_1P8'
 * 'cam_vana'/'cam_vaf' connected to 'VPH_PWR'
 *
 * refer to 'msm8930_rpm_regulator_init_data'
 * refer to 'ov8825_veg_seq'
 * refer to 'ov8825_power_seq'
 * refer to 'ov9740_veg_seq'
 * refer to 'ov9740_power_seq'
 */
static struct camera_vreg_t msm_8930_cam_vreg[] = {
	// Do nothing here
};
#else
static struct camera_vreg_t msm_8930_cam_vreg[] = {
	{"cam_vdig", REG_LDO, 1200000, 1200000, 105000},
	{"cam_vio", REG_VS, 0, 0, 0},
	{"cam_vana", REG_LDO, 2800000, 2850000, 85600},
	{"cam_vaf", REG_LDO, 2800000, 2850000, 300000},
};
#endif /* defined(CONFIG_OV8835_BAYER) || defined(CONFIG_OV9740) */

static struct camera_vreg_t msm_8930_evt_cam_vreg[] = {
	{"cam_vdig", REG_LDO, 1500000, 1500000, 105000},
	{"cam_vio", REG_VS, 0, 0, 0},
	{"cam_vana", REG_LDO, 2800000, 2850000, 85600},
	{"cam_vaf", REG_LDO, 2800000, 2850000, 300000},
};

static struct gpio msm8930_common_cam_gpio[] = {
	{20, GPIOF_DIR_IN, "CAMIF_I2C_DATA"},
	{21, GPIOF_DIR_IN, "CAMIF_I2C_CLK"},
};

static struct gpio msm8930_evt_common_cam_gpio[] = {
	{36, GPIOF_DIR_IN, "CAMIF_I2C_DATA"},
	{37, GPIOF_DIR_IN, "CAMIF_I2C_CLK"},
};

#if defined(CONFIG_OV9740)
static struct gpio msm8930_front_cam_gpio[] = {
	{55, GPIOF_DIR_OUT, "CAM_AVDD_EN"},
	{9, GPIOF_DIR_OUT, "CAM_DVDD_EN"},
	{4, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{76, GPIOF_DIR_OUT, "CAM_RESET"},
	{53, GPIOF_DIR_OUT, "CAM_STBY_N"},
};
#else
static struct gpio msm8930_front_cam_gpio[] = {
	{4, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{76, GPIOF_DIR_OUT, "CAM_RESET"},
};
#endif /* defined(CONFIG_OV9740) */

static struct gpio msm8930_evt_front_cam_gpio[] = {
	{4, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{76, GPIOF_DIR_OUT, "CAM_RESET"},
	{75, GPIOF_DIR_OUT, "CAM_STBY_N"},
};

#if defined(CONFIG_OV8835_BAYER)
static struct gpio msm8930_back_cam_gpio[] = {
	{55, GPIOF_DIR_OUT, "CAM_AVDD_EN"},
	{9, GPIOF_DIR_OUT, "CAM_DVDD_EN"},
	{5, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{107, GPIOF_DIR_OUT, "CAM_RESET"},
	{54, GPIOF_DIR_OUT, "CAM_STBY_N"},
};
#else
static struct gpio msm8930_back_cam_gpio[] = {
	{5, GPIOF_DIR_IN, "CAMIF_MCLK"},
	{107, GPIOF_DIR_OUT, "CAM_RESET"},
	{54, GPIOF_DIR_OUT, "CAM_STBY_N"},
};
#endif /* defined(CONFIG_OV8835_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_gpio_set_tbl msm8930_front_cam_gpio_set_tbl[] = {
	{55, GPIOF_OUT_INIT_HIGH, 0},
	{9, GPIOF_OUT_INIT_HIGH, 0},
	{53, GPIOF_OUT_INIT_HIGH, 1},
	{53, GPIOF_OUT_INIT_LOW, 1},
	{76, GPIOF_OUT_INIT_HIGH, 1},
	{76, GPIOF_OUT_INIT_LOW, 1},
	{76, GPIOF_OUT_INIT_HIGH, 1},
};
#else
static struct msm_gpio_set_tbl msm8930_front_cam_gpio_set_tbl[] = {
	{76, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},
};
#endif /* defined(CONFIG_OV9740) */

static struct msm_gpio_set_tbl msm8930_evt_front_cam_gpio_set_tbl[] = {
	{75, GPIOF_OUT_INIT_LOW, 1000},
	{75, GPIOF_OUT_INIT_HIGH, 4000},
	{76, GPIOF_OUT_INIT_LOW, 1000},
	{76, GPIOF_OUT_INIT_HIGH, 4000},
};

#if defined(CONFIG_OV8835_BAYER)
static struct msm_gpio_set_tbl msm8930_back_cam_gpio_set_tbl[] = {
	{107, GPIOF_OUT_INIT_LOW, 0},
	{54, GPIOF_OUT_INIT_LOW, 0},
	{55, GPIOF_OUT_INIT_HIGH, 0},
	{9, GPIOF_OUT_INIT_HIGH, 0},
	{54, GPIOF_OUT_INIT_HIGH, 0},
	{107, GPIOF_OUT_INIT_HIGH, 0},
};
#else
static struct msm_gpio_set_tbl msm8930_back_cam_gpio_set_tbl[] = {
	{54, GPIOF_OUT_INIT_LOW, 1000},
	{54, GPIOF_OUT_INIT_HIGH, 4000},
	{107, GPIOF_OUT_INIT_LOW, 1000},
	{107, GPIOF_OUT_INIT_HIGH, 4000},
};
#endif /* defined(CONFIG_OV8835_BAYER) */

static struct msm_camera_gpio_conf msm_8930_front_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio),
	.cam_gpio_set_tbl = msm8930_front_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_front_cam_gpio_set_tbl),
};

static struct msm_camera_gpio_conf msm_8930_evt__front_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_evt_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_evt_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_evt_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_evt_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_evt_front_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_evt_front_cam_gpio),
	.cam_gpio_set_tbl = msm8930_evt_front_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_evt_front_cam_gpio_set_tbl),
};

static struct msm_camera_gpio_conf msm_8930_back_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = msm8930_back_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio_set_tbl),
};

static struct msm_camera_gpio_conf msm_8930_evt_back_cam_gpio_conf = {
	.cam_gpiomux_conf_tbl = msm8930_evt_cam_2d_configs,
	.cam_gpiomux_conf_tbl_size = ARRAY_SIZE(msm8930_evt_cam_2d_configs),
	.cam_gpio_common_tbl = msm8930_evt_common_cam_gpio,
	.cam_gpio_common_tbl_size = ARRAY_SIZE(msm8930_evt_common_cam_gpio),
	.cam_gpio_req_tbl = msm8930_back_cam_gpio,
	.cam_gpio_req_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio),
	.cam_gpio_set_tbl = msm8930_back_cam_gpio_set_tbl,
	.cam_gpio_set_tbl_size = ARRAY_SIZE(msm8930_back_cam_gpio_set_tbl),
};

#if defined(CONFIG_OV8835_BAYER)
static struct i2c_board_info msm_act_main_cam_i2c_info = {
	I2C_BOARD_INFO("msm_actuator", 0x18>>1),
};
#else
static struct i2c_board_info msm_act_main_cam_i2c_info = {
	I2C_BOARD_INFO("msm_actuator", 0x11),
};
#endif /* defined(CONFIG_OV8835_BAYER) */

#if defined(CONFIG_OV8835_BAYER)
static struct msm_actuator_info msm_act_main_cam_2_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name       = MSM_ACTUATOR_MAIN_CAM_2,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 15,
	.vcm_enable     = 1,
};
#endif /* defined(CONFIG_OV8835_BAYER) */

static struct msm_actuator_info msm_act_main_cam_3_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name       = MSM_ACTUATOR_MAIN_CAM_3,
	/* bus_id GSBI8 is specific to 8930 sglte evt */
	.bus_id         = MSM_8930_GSBI8_QUP_I2C_BUS_ID,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
};

static struct msm_camera_sensor_flash_data flash_ov8825 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src = &msm_flash_src_led
#endif
};

#if defined(CONFIG_OV8835_BAYER)
static struct msm_camera_sensor_flash_data flash_ov8835 = {
	.flash_type = MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src = &msm_flash_src_lm3642
#endif
};
#endif /* defined(CONFIG_OV8835_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_camera_sensor_flash_data flash_ov9740 = {
	.flash_type = MSM_CAMERA_FLASH_NONE,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src = NULL
#endif
};
#endif /* defined(CONFIG_OV9740) */

static struct msm_camera_csi_lane_params ov8825_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};

#if defined(CONFIG_OV8835_BAYER)
static struct msm_camera_csi_lane_params ov8835_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};
#endif /* defined(CONFIG_OV8835_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_camera_csi_lane_params ov9740_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};
#endif /* defined(CONFIG_OV9740) */

static struct msm_camera_sensor_platform_info sensor_board_info_ov8825 = {
	.mount_angle = 90,
	.cam_vreg = msm_8930_evt_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_evt_cam_vreg),
	.gpio_conf = &msm_8930_evt_back_cam_gpio_conf,
	.csi_lane_params = &ov8825_csi_lane_params,
};

#if defined(CONFIG_OV8835_BAYER)
static struct msm_camera_sensor_platform_info sensor_board_info_ov8835 = {
	.mount_angle = 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &ov8835_csi_lane_params,
};
#endif /* defined(CONFIG_OV8835_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_camera_sensor_platform_info sensor_board_info_ov9740 = {
	.mount_angle = 270,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &ov9740_csi_lane_params,
};
#endif /* defined(CONFIG_OV9740) */

static struct msm_camera_sensor_info msm_camera_sensor_ov8825_data = {
	.sensor_name = "ov8825",
	.pdata = &msm_camera_csi_device_data[0],
	.flash_data = &flash_ov8825,
	.sensor_platform_info = &sensor_board_info_ov8825,
	.csi_if = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_3_info,
};

#if defined(CONFIG_OV8835_BAYER)
static struct msm_camera_sensor_info msm_camera_sensor_ov8835_data = {
	.sensor_name = "ov8835",
	.pdata = &msm_camera_csi_device_data[0],
	.flash_data = &flash_ov8835,
	.sensor_platform_info = &sensor_board_info_ov8835,
	.csi_if = 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_2_info,
};
#endif /* defined(CONFIG_OV8835_BAYER) */

#if defined(CONFIG_OV9740)
static struct msm_camera_sensor_info msm_camera_sensor_ov9740_data = {
	.sensor_name = "ov9740",
	.pdata = &msm_camera_csi_device_data[1],
	.flash_data = &flash_ov9740,
	.sensor_platform_info = &sensor_board_info_ov9740,
	.csi_if = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
	.actuator_info = NULL,
};
#endif /* defined(CONFIG_OV9740) */

static struct msm_camera_sensor_flash_data flash_ov9724 = {
	.flash_type = MSM_CAMERA_FLASH_NONE
};

static struct msm_camera_csi_lane_params ov9724_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov9724 = {
	.mount_angle = 90,
	.cam_vreg = msm_8930_evt_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_evt_cam_vreg),
	.gpio_conf = &msm_8930_evt__front_cam_gpio_conf,
	.csi_lane_params = &ov9724_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov9724_data = {
	.sensor_name = "ov9724",
	.pdata = &msm_camera_csi_device_data[1],
	.flash_data = &flash_ov9724,
	.sensor_platform_info = &sensor_board_info_ov9724,
	.csi_if = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
};

static struct msm_actuator_info msm_act_main_cam_0_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name   = MSM_ACTUATOR_MAIN_CAM_0,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
};

static struct msm_camera_sensor_flash_data flash_imx074 = {
	.flash_type	= MSM_CAMERA_FLASH_LED,
#ifdef CONFIG_MSM_CAMERA_FLASH
	.flash_src	= &msm_flash_src
#endif
};

static struct msm_camera_csi_lane_params imx074_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_platform_info sensor_board_info_imx074 = {
	.mount_angle	= 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &imx074_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_imx074_data = {
	.sensor_name	= "imx074",
	.pdata	= &msm_camera_csi_device_data[0],
	.flash_data	= &flash_imx074,
	.strobe_flash_data = &strobe_flash_xenon,
	.sensor_platform_info = &sensor_board_info_imx074,
	.csi_if	= 1,
	.camera_type = BACK_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
	.actuator_info = &msm_act_main_cam_0_info,
};

static struct msm_camera_sensor_flash_data flash_mt9m114 = {
	.flash_type = MSM_CAMERA_FLASH_NONE
};

static struct msm_camera_csi_lane_params mt9m114_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x1,
};

static struct msm_camera_sensor_platform_info sensor_board_info_mt9m114 = {
	.mount_angle = 270,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &mt9m114_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_mt9m114_data = {
	.sensor_name = "mt9m114",
	.pdata = &msm_camera_csi_device_data[1],
	.flash_data = &flash_mt9m114,
	.sensor_platform_info = &sensor_board_info_mt9m114,
	.csi_if = 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = YUV_SENSOR,
};

static struct msm_camera_sensor_flash_data flash_ov2720 = {
	.flash_type	= MSM_CAMERA_FLASH_NONE,
};

static struct msm_camera_csi_lane_params ov2720_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0x3,
};

static struct msm_camera_sensor_platform_info sensor_board_info_ov2720 = {
	.mount_angle	= 0,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_front_cam_gpio_conf,
	.csi_lane_params = &ov2720_csi_lane_params,
};

static struct msm_camera_sensor_info msm_camera_sensor_ov2720_data = {
	.sensor_name	= "ov2720",
	.pdata	= &msm_camera_csi_device_data[1],
	.flash_data	= &flash_ov2720,
	.sensor_platform_info = &sensor_board_info_ov2720,
	.csi_if	= 1,
	.camera_type = FRONT_CAMERA_2D,
	.sensor_type = BAYER_SENSOR,
};

static struct msm_camera_sensor_flash_data flash_s5k3l1yx = {
	.flash_type = MSM_CAMERA_FLASH_LED,
	.flash_src = &msm_flash_src
};

static struct msm_camera_csi_lane_params s5k3l1yx_csi_lane_params = {
	.csi_lane_assign = 0xE4,
	.csi_lane_mask = 0xF,
};

static struct msm_camera_sensor_platform_info sensor_board_info_s5k3l1yx = {
	.mount_angle  = 90,
	.cam_vreg = msm_8930_cam_vreg,
	.num_vreg = ARRAY_SIZE(msm_8930_cam_vreg),
	.gpio_conf = &msm_8930_back_cam_gpio_conf,
	.csi_lane_params = &s5k3l1yx_csi_lane_params,
};

#if defined(CONFIG_OV8835_BAYER)
// Do nothing here
#else
static struct msm_actuator_info msm_act_main_cam_2_info = {
	.board_info     = &msm_act_main_cam_i2c_info,
	.cam_name       = MSM_ACTUATOR_MAIN_CAM_2,
	.bus_id         = MSM_8930_GSBI4_QUP_I2C_BUS_ID,
	.vcm_pwd        = 0,
	.vcm_enable     = 0,
};
#endif /* defined(CONFIG_OV8835_BAYER) */

static struct msm_camera_sensor_info msm_camera_sensor_s5k3l1yx_data = {
	.sensor_name          = "s5k3l1yx",
	.pdata                = &msm_camera_csi_device_data[0],
	.flash_data           = &flash_s5k3l1yx,
	.sensor_platform_info = &sensor_board_info_s5k3l1yx,
	.csi_if               = 1,
	.camera_type          = BACK_CAMERA_2D,
	.sensor_type          = BAYER_SENSOR,
	.actuator_info        = &msm_act_main_cam_2_info,
};

static struct platform_device msm_camera_server = {
	.name = "msm_cam_server",
	.id = 0,
};

#ifdef CONFIG_I2C
struct i2c_board_info msm8930_camera_i2c_boardinfo[] = {
	{
	I2C_BOARD_INFO("imx074", 0x1A),
	.platform_data = &msm_camera_sensor_imx074_data,
	},
	{
	I2C_BOARD_INFO("ov2720", 0x6C),
	.platform_data = &msm_camera_sensor_ov2720_data,
	},
	{
	I2C_BOARD_INFO("mt9m114", 0x48),
	.platform_data = &msm_camera_sensor_mt9m114_data,
	},
	{
	I2C_BOARD_INFO("s5k3l1yx", 0x20),
	.platform_data = &msm_camera_sensor_s5k3l1yx_data,
	},
	{
	I2C_BOARD_INFO("tps61310", 0x66),
	},

#if defined(CONFIG_OV8835_BAYER)
	{
	I2C_BOARD_INFO("ov8835", 0x6C>>1),
	.platform_data = &msm_camera_sensor_ov8835_data,
	},
#endif /* defined(CONFIG_OV8835_BAYER) */

#if defined(CONFIG_OV9740)
	{
	I2C_BOARD_INFO("ov9740", 0x20>>1),
	.platform_data = &msm_camera_sensor_ov9740_data,
	},
#endif /* defined(CONFIG_OV9740) */

#if defined(CONFIG_LM3642)
	{
	I2C_BOARD_INFO("lm3642", 0xC6>>1),
	},
#endif /* defined(CONFIG_LM3642) */
};

/* 8930 SGLTE device */
struct i2c_board_info msm8930_evt_camera_i2c_boardinfo[] = {
	{
	I2C_BOARD_INFO("ov8825", 0x6c>>1),
	.platform_data = &msm_camera_sensor_ov8825_data,
	},
	{
	I2C_BOARD_INFO("ov9724", 0x20>>1),
	.platform_data = &msm_camera_sensor_ov9724_data,
	},
};

struct msm_camera_board_info msm8930_camera_board_info = {
	.board_info = msm8930_camera_i2c_boardinfo,
	.num_i2c_board_info = ARRAY_SIZE(msm8930_camera_i2c_boardinfo),
};
#endif

void __init msm8930_init_cam(void)
{
	msm_gpiomux_install(msm8930_cam_common_configs,
			ARRAY_SIZE(msm8930_cam_common_configs));

	if (machine_is_msm8930_evt() &&
			(socinfo_get_platform_subtype() ==
			 PLATFORM_SUBTYPE_SGLTE)) {
		msm_gpiomux_install(msm8930_evt_cam_configs,
				ARRAY_SIZE(msm8930_evt_cam_configs));

		/* Load ov8825 & ov9724 only for SGLTE device */
		msm8930_camera_board_info.board_info =
			msm8930_evt_camera_i2c_boardinfo;
		msm8930_camera_board_info.num_i2c_board_info =
			ARRAY_SIZE(msm8930_evt_camera_i2c_boardinfo);
	}

	if (machine_is_msm8930_cdp()) {
		struct msm_camera_sensor_info *s_info;
		s_info = &msm_camera_sensor_s5k3l1yx_data;
		s_info->sensor_platform_info->mount_angle = 0;
#if defined(CONFIG_I2C) && (defined(CONFIG_GPIO_SX150X) || \
	defined(CONFIG_GPIO_SX150X_MODULE))
		msm_flash_src._fsrc.ext_driver_src.led_en =
			GPIO_CAM_GP_LED_EN1;
		msm_flash_src._fsrc.ext_driver_src.led_flash_en =
			GPIO_CAM_GP_LED_EN2;

		msm_flash_src._fsrc.ext_driver_src.expander_info =
			cam_expander_info;
#endif
	}

	platform_device_register(&msm_camera_server);
	platform_device_register(&msm8960_device_csiphy0);
	platform_device_register(&msm8960_device_csiphy1);
	platform_device_register(&msm8960_device_csid0);
	platform_device_register(&msm8960_device_csid1);
	platform_device_register(&msm8960_device_ispif);
	platform_device_register(&msm8960_device_vfe);
	platform_device_register(&msm8960_device_vpe);
}
#endif

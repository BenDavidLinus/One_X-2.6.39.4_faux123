/*
 * arch/arm/mach-tegra/board-harmony-sdhci.c
 *
 * Copyright (C) 2010 Google, Inc.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/resource.h>
#include <linux/platform_device.h>
#include <linux/wlan_plat.h>
#include <linux/delay.h>
#include <linux/gpio.h>
#include <linux/clk.h>
#include <linux/err.h>
#include <linux/mmc/host.h>

#include <asm/mach-types.h>
#include <mach/irqs.h>
#include <mach/iomap.h>
#include <mach/sdhci.h>

#include "gpio-names.h"
#include "board.h"

#define WHISTLER_WLAN_PWR	TEGRA_GPIO_PK5
#define WHISTLER_WLAN_RST	TEGRA_GPIO_PK6

#define WHISTLER_EXT_SDCARD_DETECT	TEGRA_GPIO_PI5

static void (*wifi_status_cb)(int card_present, void *dev_id);
static void *wifi_status_cb_devid;

static int whistler_wifi_status_register(
		void (*sdhcicallback)(int card_present, void *dev_id),
		void *dev_id)
{
	if (wifi_status_cb)
		return -EAGAIN;
	wifi_status_cb = sdhcicallback;
	wifi_status_cb_devid = dev_id;
	return 0;
}

static int whistler_wifi_set_carddetect(int val)
{
	pr_debug("%s: %d\n", __func__, val);
	if (wifi_status_cb)
		wifi_status_cb(val, wifi_status_cb_devid);
	else
		pr_warning("%s: Nobody to notify\n", __func__);
	return 0;
}

static int whistler_wifi_power(int on)
{
	gpio_set_value(WHISTLER_WLAN_PWR, on);
	mdelay(100);
	gpio_set_value(WHISTLER_WLAN_RST, on);
	mdelay(200);

	return 0;
}

static int whistler_wifi_reset(int on)
{
	pr_debug("%s: do nothing\n", __func__);
	return 0;
}


static struct wifi_platform_data whistler_wifi_control = {
	.set_power      = whistler_wifi_power,
	.set_reset      = whistler_wifi_reset,
	.set_carddetect = whistler_wifi_set_carddetect,
};

static struct platform_device whistler_wifi_device = {
	.name           = "bcm4329_wlan",
	.id             = 1,
	.dev            = {
		.platform_data = &whistler_wifi_control,
	},
};

static struct resource sdhci_resource0[] = {
	[0] = {
		.start  = INT_SDMMC1,
		.end    = INT_SDMMC1,
		.flags  = IORESOURCE_IRQ,
	},
	[1] = {
		.start	= TEGRA_SDMMC1_BASE,
		.end	= TEGRA_SDMMC1_BASE + TEGRA_SDMMC1_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};
static struct resource sdhci_resource1[] = {
	[0] = {
		.start  = INT_SDMMC2,
		.end    = INT_SDMMC2,
		.flags  = IORESOURCE_IRQ,
	},
	[1] = {
		.start	= TEGRA_SDMMC2_BASE,
		.end	= TEGRA_SDMMC2_BASE + TEGRA_SDMMC2_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};


static struct resource sdhci_resource2[] = {
	[0] = {
		.start  = INT_SDMMC3,
		.end    = INT_SDMMC3,
		.flags  = IORESOURCE_IRQ,
	},
	[1] = {
		.start	= TEGRA_SDMMC3_BASE,
		.end	= TEGRA_SDMMC3_BASE + TEGRA_SDMMC3_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct resource sdhci_resource3[] = {
	[0] = {
		.start  = INT_SDMMC4,
		.end    = INT_SDMMC4,
		.flags  = IORESOURCE_IRQ,
	},
	[1] = {
		.start	= TEGRA_SDMMC4_BASE,
		.end	= TEGRA_SDMMC4_BASE + TEGRA_SDMMC4_SIZE-1,
		.flags	= IORESOURCE_MEM,
	},
};

static struct tegra_sdhci_platform_data tegra_sdhci_platform_data0 = {
	.cd_gpio = -1,
	.wp_gpio = -1,
	.power_gpio = -1,
};

static struct embedded_sdio_data embedded_sdio_data1 = {
	.cccr   = {
		.sdio_vsn       = 2,
		.multi_block    = 1,
		.low_speed      = 0,
		.wide_bus       = 0,
		.high_power     = 1,
		.high_speed     = 1,
	},
	.cis  = {
		.vendor         = 0x02d0,
		.device         = 0x4329,
	},
};

static struct tegra_sdhci_platform_data tegra_sdhci_platform_data1 = {
	.mmc_data = {
		.register_status_notify	= whistler_wifi_status_register,
		.embedded_sdio = &embedded_sdio_data1,
	},
	.cd_gpio = -1,
	.wp_gpio = -1,
	.power_gpio = -1,
};

static struct tegra_sdhci_platform_data tegra_sdhci_platform_data2 = {
	.cd_gpio = WHISTLER_EXT_SDCARD_DETECT,
	.wp_gpio = -1,
	.power_gpio = -1,
};

static struct tegra_sdhci_platform_data tegra_sdhci_platform_data3 = {
	.cd_gpio = -1,
	.wp_gpio = -1,
	.power_gpio = -1,
};

static struct platform_device tegra_sdhci_device0 = {
	.name		= "sdhci-tegra",
	.id		= 0,
	.resource	= sdhci_resource0,
	.num_resources	= ARRAY_SIZE(sdhci_resource0),
	.dev = {
		.platform_data = &tegra_sdhci_platform_data0,
	},
};

static struct platform_device tegra_sdhci_device1 = {
	.name		= "sdhci-tegra",
	.id		= 1,
	.resource	= sdhci_resource1,
	.num_resources	= ARRAY_SIZE(sdhci_resource1),
	.dev = {
		.platform_data = &tegra_sdhci_platform_data1,
	},
};

static struct platform_device tegra_sdhci_device2 = {
	.name		= "sdhci-tegra",
	.id		= 2,
	.resource	= sdhci_resource2,
	.num_resources	= ARRAY_SIZE(sdhci_resource2),
	.dev = {
		.platform_data = &tegra_sdhci_platform_data2,
	},
};

static struct platform_device tegra_sdhci_device3 = {
	.name		= "sdhci-tegra",
	.id		= 3,
	.resource	= sdhci_resource3,
	.num_resources	= ARRAY_SIZE(sdhci_resource3),
	.dev = {
		.platform_data = &tegra_sdhci_platform_data3,
	},
};

static int __init whistler_wifi_init(void)
{
	gpio_request(WHISTLER_WLAN_PWR, "wlan_power");
	gpio_request(WHISTLER_WLAN_RST, "wlan_rst");

	tegra_gpio_enable(WHISTLER_WLAN_PWR);
	tegra_gpio_enable(WHISTLER_WLAN_RST);

	gpio_direction_output(WHISTLER_WLAN_PWR, 0);
	gpio_direction_output(WHISTLER_WLAN_RST, 0);

	platform_device_register(&whistler_wifi_device);
	return 0;
}
int __init whistler_sdhci_init(void)
{
	int ret;

	tegra_gpio_enable(WHISTLER_EXT_SDCARD_DETECT);

	platform_device_register(&tegra_sdhci_device3);
	platform_device_register(&tegra_sdhci_device2);
	platform_device_register(&tegra_sdhci_device1);
	platform_device_register(&tegra_sdhci_device0);

	whistler_wifi_init();
	return 0;
}
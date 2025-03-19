// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2024 Willsemi Co. Ltd.
 *
 */

#include <linux/err.h>
#include <linux/gpio/consumer.h>
#include <linux/i2c.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <linux/gpio.h>
#include <linux/regmap.h>
#include <linux/regulator/driver.h>
#include <linux/regulator/machine.h>
#include <linux/regulator/of_regulator.h>
#include <linux/kernel.h>

#define WL28681_DEVICE_D1		0x00
#define WL28681_DEVICE_D2		0x01
#define WL28681_ILIM			0x02
#define WL28681_LDO_EN			0x03

#define WL28681_LDO1_VOUT		0x04
#define WL28681_LDO2_VOUT		0x05
#define WL28681_LDO3_VOUT		0x06
#define WL28681_LDO4_VOUT		0x07
#define WL28681_LDO5_VOUT		0x08
#define WL28681_LDO6_VOUT		0x09
#define WL28681_LDO7_VOUT		0x0a

#define WL28681_LDO1_LDO2_SEQ		0x0b
#define WL28681_LDO3_LDO4_SEQ		0x0c
#define WL28681_LDO5_LDO6_SEQ		0x0d
#define WL28681_LDO7_SEQ			0x0e
#define WL28681_SEQ_STATUS			0x0f

#define WL28681_DISCHARGE_RESISTORS		0x10
#define WL28681_RESET					0x11
#define WL28681_REPROGRAMMABLE_I2C_ADDR	0x12

#define WL28681_LDO1234_COMP	0x13
#define WL28681_LDO567_COMP		0x14

#define WL28681_UVP_INT			0x15
#define WL28681_OCP_INT			0x16
#define WL28681_TSD_UVLO_INT	0x17
#define WL28681_UVP_INT_STATUS			0x18
#define WL28681_OCP_INT_STATUS			0x19
#define WL28681_TSD_UVLO_INT_STATUS		0x1a
#define WL28681_SUSD_STATUS				0x1b
#define WL28681_UVP_INT_MASK			0x1c
#define WL28681_OCP_INT_MASK			0x1d
#define WL28681_TSD_UVLO_INT_MASK		0x1e
#define WL28681_MAX_REG_NO				0x1f

#define WL28681_VSEL_MASK		0xff
#define WL28681_DEFAULT_SLAVE_ADDR  0x35
#define WL28681_RESET_NUMBER        318
#define WL28681_RESET_J_NUMBER      319
#define WL28681_LDO12_OFFSET      488


enum wl28681_regulators {
	WL28681_REGULATOR_LDO1 = 0,
	WL28681_REGULATOR_LDO2,
	WL28681_REGULATOR_LDO3,
	WL28681_REGULATOR_LDO4,
	WL28681_REGULATOR_LDO5,
	WL28681_REGULATOR_LDO6,
	WL28681_REGULATOR_LDO7,
	WL28681_MAX_REGULATORS,
};

/* ldo1~7  0-current,1-current */
/*
 *	LDO1	1460000, 2000000
 *	LDO2	1460000, 2000000
 *	LDO3	500000, 700000
 *	LDO4	500000, 700000
 *	LDO5	740000, 950000
 *	LDO6	500000, 700000
 *	LDO7	740000, 950000
 */
static const unsigned int wl28681_crtable1[] = {1460000, 2000000};
static const unsigned int wl28681_crtable2[] = {1460000, 2000000};
static const unsigned int wl28681_crtable3[] = {500000, 700000};
static const unsigned int wl28681_crtable4[] = {500000, 700000};
static const unsigned int wl28681_crtable5[] = {740000, 950000};
static const unsigned int wl28681_crtable6[] = {500000, 700000};
static const unsigned int wl28681_crtable7[] = {740000, 950000};

struct wl28681 {
	struct device *dev;
	struct regmap *regmap;
	struct regulator_dev *rdev;
	struct gpio_desc *reset_gpio;
	int min_dropout_uv;
	int ldo_vout[7];
	int ldo_en;
};

static struct wl28681 *pDefault_wl2868 = NULL;

static int wl2868_regulator_disable_regmap(struct regulator_dev *rdev);
static int wl2868_regulator_enable_regmap(struct regulator_dev *rdev);

static const struct regulator_ops wl28681_reg_ops = {
	.list_voltage		= regulator_list_voltage_linear,
	.map_voltage		= regulator_map_voltage_linear,
	.get_voltage_sel	= regulator_get_voltage_sel_regmap,
	.set_voltage_sel	= regulator_set_voltage_sel_regmap,
	.set_current_limit  = regulator_set_current_limit_regmap,
	.enable			= wl2868_regulator_enable_regmap,
	.disable		= wl2868_regulator_disable_regmap,
	.is_enabled		= regulator_is_enabled_regmap,
};

#define WL28681_DESC(_id, _match, _supply, _min, _max, _step, _vreg, _vmask,	\
	 _ereg, _emask, _enval, _disval, _curr_table, _creg, _cmask, _minsel)	\
	{									\
		.id		= (_id),					\
		.name		= (_match),					\
		.of_match	= of_match_ptr(_match),				\
		.supply_name	= (_supply),					\
		.min_uV		= (_min) * 1000,				\
		.uV_step	= (_step) * 1000,			\
		.n_voltages	= (((_max) - (_min)) / (_step) + _minsel),	\
		.n_current_limits = ARRAY_SIZE(_curr_table),		\
		.regulators_node = of_match_ptr("regulators"),		\
		.type		= REGULATOR_VOLTAGE,			\
		.vsel_reg	= (_vreg),				\
		.vsel_mask	= (_vmask),				\
		.csel_reg	= (_creg),				\
		.csel_mask	= (_cmask),				\
		.enable_reg	= (_ereg),				\
		.enable_mask	= (_emask),				\
		.enable_val     = (_enval),				\
		.disable_val     = (_disval),				\
		.ops		= &wl28681_reg_ops,			\
		.curr_table = _curr_table,				\
		.linear_min_sel = _minsel,				\
		.owner		= THIS_MODULE,				\
	}

static const struct regulator_desc wl28681_reg[] = {
	WL28681_DESC(WL28681_REGULATOR_LDO1, "WL_LDO1", "vin12", 496, 2048, 8,
		     WL28681_LDO1_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(0),
			 BIT(0), 0,	wl28681_crtable1, WL28681_ILIM, BIT(0), 61),
	WL28681_DESC(WL28681_REGULATOR_LDO2, "WL_LDO2", "vin12", 496, 2048, 8,
		     WL28681_LDO2_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(1),
			 BIT(1), 0, wl28681_crtable2, WL28681_ILIM, BIT(1), 61),
	WL28681_DESC(WL28681_REGULATOR_LDO3, "WL_LDO3", "vin34", 1372, 3412, 8,
		     WL28681_LDO3_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(2),
			 BIT(2), 0, wl28681_crtable3, WL28681_ILIM, BIT(2), 1),
	WL28681_DESC(WL28681_REGULATOR_LDO4, "WL_LDO4", "vin34", 1372, 3412, 8,
		     WL28681_LDO4_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(3),
			 BIT(3), 0, wl28681_crtable4, WL28681_ILIM, BIT(3), 1),
	WL28681_DESC(WL28681_REGULATOR_LDO5, "WL_LDO5", "vin5", 1372, 3412, 8,
		     WL28681_LDO5_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(4),
			 BIT(4), 0, wl28681_crtable5, WL28681_ILIM, BIT(4), 1),
	WL28681_DESC(WL28681_REGULATOR_LDO6, "WL_LDO6", "vin6", 1372, 3412, 8,
		     WL28681_LDO6_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(5),
			 BIT(5), 0, wl28681_crtable6, WL28681_ILIM, BIT(5), 1),
	WL28681_DESC(WL28681_REGULATOR_LDO7, "WL_LDO7", "vin7", 1372, 3412, 8,
		     WL28681_LDO7_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(6),
			 BIT(6), 0, wl28681_crtable7, WL28681_ILIM, BIT(6), 1),
};

static const struct regulator_desc wl28681_reg_j[] = {
	WL28681_DESC(WL28681_REGULATOR_LDO1 + WL28681_MAX_REGULATORS, "WL_LDO1_j", "vin12", 496, 2048, 8,
		     WL28681_LDO1_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(0),
			 BIT(0), 0,	wl28681_crtable1, WL28681_ILIM, BIT(0), 61),
	WL28681_DESC(WL28681_REGULATOR_LDO2 + WL28681_MAX_REGULATORS, "WL_LDO2_j", "vin12", 496, 2048, 8,
		     WL28681_LDO2_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(1),
			 BIT(1), 0, wl28681_crtable2, WL28681_ILIM, BIT(1), 61),
	WL28681_DESC(WL28681_REGULATOR_LDO3 + WL28681_MAX_REGULATORS, "WL_LDO3_j", "vin34", 1372, 3412, 8,
		     WL28681_LDO3_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(2),
			 BIT(2), 0, wl28681_crtable3, WL28681_ILIM, BIT(2), 1),
	WL28681_DESC(WL28681_REGULATOR_LDO4 + WL28681_MAX_REGULATORS, "WL_LDO4_j", "vin34", 1372, 3412, 8,
		     WL28681_LDO4_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(3),
			 BIT(3), 0, wl28681_crtable4, WL28681_ILIM, BIT(3), 1),
	WL28681_DESC(WL28681_REGULATOR_LDO5 + WL28681_MAX_REGULATORS, "WL_LDO5_j", "vin5", 1372, 3412, 8,
		     WL28681_LDO5_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(4),
			 BIT(4), 0, wl28681_crtable5, WL28681_ILIM, BIT(4), 1),
	WL28681_DESC(WL28681_REGULATOR_LDO6 + WL28681_MAX_REGULATORS, "WL_LDO6_j", "vin6", 1372, 3412, 8,
		     WL28681_LDO6_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(5),
			 BIT(5), 0, wl28681_crtable6, WL28681_ILIM, BIT(5), 1),
	WL28681_DESC(WL28681_REGULATOR_LDO7 + WL28681_MAX_REGULATORS, "WL_LDO7_j", "vin7", 1372, 3412, 8,
		     WL28681_LDO7_VOUT, WL28681_VSEL_MASK, WL28681_LDO_EN, BIT(6),
			 BIT(6), 0, wl28681_crtable7, WL28681_ILIM, BIT(6), 1),
};

static const struct regmap_range wl28681_writeable_ranges[] = {
	regmap_reg_range(WL28681_ILIM, WL28681_LDO567_COMP),
};

static const struct regmap_range wl28681_readable_ranges[] = {
	regmap_reg_range(WL28681_DEVICE_D1, WL28681_TSD_UVLO_INT_MASK),
};

static const struct regmap_range wl28681_volatile_ranges[] = {
	regmap_reg_range(WL28681_DEVICE_D1, WL28681_TSD_UVLO_INT_MASK),
};

static const struct regmap_access_table wl28681_writeable_table = {
	.yes_ranges   = wl28681_writeable_ranges,
	.n_yes_ranges = ARRAY_SIZE(wl28681_writeable_ranges),
};

static const struct regmap_access_table wl28681_readable_table = {
	.yes_ranges   = wl28681_readable_ranges,
	.n_yes_ranges = ARRAY_SIZE(wl28681_readable_ranges),
};

static const struct regmap_access_table wl28681_volatile_table = {
	.yes_ranges   = wl28681_volatile_ranges,
	.n_yes_ranges = ARRAY_SIZE(wl28681_volatile_ranges),
};

static const struct regmap_config wl28681_regmap_config = {
	.reg_bits = 8,
	.val_bits = 8,
	.max_register = WL28681_MAX_REG_NO,
	.wr_table = &wl28681_writeable_table,
	.rd_table = &wl28681_readable_table,
	.cache_type = REGCACHE_RBTREE,
	.volatile_table = &wl28681_volatile_table,
};

static int wl2868_regulator_disable_regmap(struct regulator_dev *rdev)
{
	struct device *dev = rdev->dev.parent;
	struct i2c_client *client = to_i2c_client(dev);
	struct wl28681 *wl28681 = i2c_get_clientdata(client);
	int ret = 0;
	unsigned int val;

	ret = regulator_disable_regmap(rdev);
	if (ret) {
		dev_err(dev, "Set reg disable error %d", ret);
	} else {
		val = rdev->desc->enable_val;
		if (!val)
			val = rdev->desc->enable_mask;

		wl28681->ldo_en &= (~val);
		dev_dbg(dev, "Set disable val %d", val);
	}
	// if (0 == (wl28681->ldo_en & 0x7f)) {
	// 	ret = regmap_write(rdev->regmap, WL28681_LDO_EN, 0);
	// 	if (ret) {
	// 		dev_err(dev, "sysen set 0 error ret %d", ret);
	// 	}
	// }

	return ret;
}

static int wl2868_regulator_enable_regmap(struct regulator_dev *rdev)
{
	struct device *dev = rdev->dev.parent;
	struct i2c_client *client = to_i2c_client(dev);
	struct wl28681 *wl28681 = i2c_get_clientdata(client);
	int ret = 0;
	unsigned int val;
	int count = 0;
	int tmp_data = 0;
	const int max_read_count = 10;

	// if (0 == (wl28681->ldo_en & 0x7f)) {
	// 	ret = regmap_write(rdev->regmap, WL28681_LDO_EN, 0x80);
	// 	if (ret) {
	// 		dev_err(dev, "sysen set 1 error ret %d", ret);
	// 	}
	// }
	ret = regulator_enable_regmap(rdev);
	if (ret) {
		dev_err(dev, "Set reg enable error %d", ret);
	} else {
		if (rdev->desc->enable_is_inverted) {
			val = rdev->desc->disable_val;
		} else {
			val = rdev->desc->enable_val;
			if (!val)
				val = rdev->desc->enable_mask;
		}
		wl28681->ldo_en |= val;
		dev_dbg(dev, "Set enable val %d", val);
	}

	do {
		ret = regmap_read(rdev->regmap, WL28681_LDO_EN, &tmp_data);
		if (ret) {
			dev_err(dev, "i2c LDO_EN read fail %d\n", ret);
		} else {
			if ((tmp_data & wl28681->ldo_en) == wl28681->ldo_en) {
				break;
			}
			dev_info(dev, "wait 1ms for LDO_EN read_data %0x ldo_en 0x%x\n",
				tmp_data, wl28681->ldo_en);
		}
		usleep_range(1000, 1100);
		count++;
		if (count == (max_read_count / 2)) {
			dev_err(dev, "retry 5 time no enable reg so enable ldo again");
			ret = regulator_enable_regmap(rdev);
			if (ret) {
				dev_err(dev, "Set reg enable error %d", ret);
				break;
			}
		}
	} while(count < max_read_count);

	return ret;
}

static void wl28681_reset(struct wl28681 *wl28681)
{
	gpiod_set_value_cansleep(wl28681->reset_gpio, 0);	//set H
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(wl28681->reset_gpio, 1);	//set L
	usleep_range(10000, 11000);
	gpiod_set_value_cansleep(wl28681->reset_gpio, 0);
	usleep_range(10000, 11000);
}

static void wl28681_i2c_disable(struct wl28681 *wl28681)
{
	gpiod_set_value_cansleep(wl28681->reset_gpio, 1);	//set L
	dev_info(wl28681->dev, "set dev i2c disable\n");
	usleep_range(10000, 11000);
}

static void wl28681_i2c_enable(struct wl28681 *wl28681)
{
	int i, ret;
	int tmp_data;

	gpiod_set_value_cansleep(wl28681->reset_gpio, 0);	//set H
	dev_info(wl28681->dev, "set dev i2c enable\n");
	usleep_range(10000, 11000);
	for(i = 0; i < 3; i++)
	{
		ret = regmap_read(wl28681->regmap, WL28681_REPROGRAMMABLE_I2C_ADDR, &tmp_data);
		if (ret) {
			dev_err(wl28681->dev, "i2c enable set error, start rest retry %d\n", i);
			wl28681_reset(wl28681);
		} else {
			dev_info(wl28681->dev, "i2c enable set success, I2C ADDR REG: 0x%x\n", tmp_data);
			break;
		}
	}
}

static int wl28681_i2c_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct regulator_config config = {};
	struct regulator_dev *rdev;
	const struct regulator_desc *regulators;
	struct wl28681 *wl28681;
	int ret, i;
	unsigned int gpio_number;
	unsigned short current_addr;
	int tmp_data;
	static atomic_t probe_cnt = ATOMIC_INIT(0);
	int sleep_cnt = 0;

	wl28681 = devm_kzalloc(dev, sizeof(struct wl28681), GFP_KERNEL);
	if (!wl28681)
		return -ENOMEM;

	if (1 < atomic_add_return(1, &probe_cnt)) {
		//wait for other probe
		do {
			msleep(10);
			sleep_cnt++;
			dev_info(dev, "wait 10ms for other wl2868 probe cnt: %d\n", sleep_cnt);
			if (1 >= atomic_read(&probe_cnt))
			{
				dev_info(dev, "wait success for other wl2868 probe\n");
				break;
			}
		} while(sleep_cnt < 50);
	}

	wl28681->ldo_en = 0;
	wl28681->reset_gpio = devm_gpiod_get(dev, "reset", GPIOD_OUT_LOW);
	if (IS_ERR(wl28681->reset_gpio)) {
		ret = PTR_ERR(wl28681->reset_gpio);
		dev_err(dev, "failed to request reset GPIO: %d\n", ret);
		return ret;
	}
	gpio_number = desc_to_gpio(wl28681->reset_gpio);
	dev_info(dev, "RESET GPIO number is %u\n", gpio_number);

	if (NULL != pDefault_wl2868)
	{
		wl28681_i2c_disable(pDefault_wl2868);
		dev_info(dev, "Disable default wl2868 I2C start\n");
	}

	wl28681_reset(wl28681);

	if (WL28681_DEFAULT_SLAVE_ADDR != client->addr)
	{
		current_addr   = client->addr;
		client->addr   = WL28681_DEFAULT_SLAVE_ADDR;

		if (0 > i2c_smbus_write_byte_data(client, WL28681_REPROGRAMMABLE_I2C_ADDR, 0x02))
		{
			dev_err(dev, "write i2c addr 0x%x failed!", client->addr);
		}

		regulators = wl28681_reg_j;
		client->addr = current_addr;
	}
	else
	{
		regulators = wl28681_reg;
	}

	if (NULL != pDefault_wl2868)
	{
		wl28681_i2c_enable(pDefault_wl2868);
		dev_info(dev, "Disable default wl2868 I2C end\n");
	}

	i2c_set_clientdata(client, wl28681);
	wl28681->dev = dev;
	wl28681->regmap = devm_regmap_init_i2c(client, &wl28681_regmap_config);
	if (IS_ERR(wl28681->regmap)) {
		ret = PTR_ERR(wl28681->regmap);
		dev_err(dev, "Failed to allocate register map: %d\n", ret);
		atomic_sub(1, &probe_cnt);
		return ret;
	}

	config.dev = &client->dev;
	config.regmap = wl28681->regmap;

	/* Instantiate the regulators */
	for (i = 0; i < WL28681_MAX_REGULATORS; i++) {
		rdev = devm_regulator_register(&client->dev,
					       &regulators[i], &config);
		if (IS_ERR(rdev)) {
			dev_err(&client->dev,
				"failed to register %d regulator\n", i);
			atomic_sub(1, &probe_cnt);
			return PTR_ERR(rdev);
		}
	}

	if (WL28681_DEFAULT_SLAVE_ADDR == client->addr)
	{
		pDefault_wl2868 = wl28681;
	}

	/*Inital related mask for interrupt here*/
	ret = regmap_read(wl28681->regmap, WL28681_UVP_INT_MASK, &tmp_data);
	dev_info(dev, "WL28681_UVP_INT_MASK = %x ret = %d",tmp_data ,ret);
	ret = regmap_read(wl28681->regmap, WL28681_OCP_INT_MASK, &tmp_data);
	dev_info(dev, "WL28681_OCP_INT_MASK = %x ret = %d",tmp_data ,ret);
	ret = regmap_read(wl28681->regmap, WL28681_TSD_UVLO_INT_MASK, &tmp_data);
	dev_info(dev, "WL28681_TSD_UVLO_INT_MASK = %x ret = %d",tmp_data ,ret);
	regmap_write(wl28681->regmap, WL28681_UVP_INT_MASK, 0);
	regmap_write(wl28681->regmap, WL28681_OCP_INT_MASK, 0);
	regmap_write(wl28681->regmap, WL28681_TSD_UVLO_INT_MASK, 0);

	for (i = 0; i < ARRAY_SIZE(wl28681->ldo_vout); i++)
	{
		regmap_read(wl28681->regmap, WL28681_LDO1_VOUT + i,
			&wl28681->ldo_vout[i]);
		dev_info(dev, "wl28681 WL_LDO%d value is 0x%x\n", i, wl28681->ldo_vout[i]);
	}
	atomic_sub(1, &probe_cnt);

	return 0;
}

static void wl28681_regulator_shutdown(struct i2c_client *client)
{
	struct wl28681 *wl28681 = i2c_get_clientdata(client);
	int ret = 0;
	if (system_state == SYSTEM_POWER_OFF)
		ret = regmap_write(wl28681->regmap, WL28681_LDO_EN, 0x80);
}

static int __maybe_unused wl28681_suspend(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct wl28681 *wl28681 = i2c_get_clientdata(client);
	int i;
	int ret = 0;

	ret = regmap_read(wl28681->regmap, WL28681_LDO_EN, &wl28681->ldo_en);
	for (i = 0; i < ARRAY_SIZE(wl28681->ldo_vout); i++)
		regmap_read(wl28681->regmap, WL28681_LDO1_VOUT + i,
			    &wl28681->ldo_vout[i]);

	return 0;
}

static int __maybe_unused wl28681_resume(struct device *dev)
{
	struct i2c_client *client = to_i2c_client(dev);
	struct wl28681 *wl28681 = i2c_get_clientdata(client);
	int i;
	int ret =0;

	// wl28681_reset(wl28681);
	for (i = 0; i < ARRAY_SIZE(wl28681->ldo_vout); i++)
		regmap_write(wl28681->regmap, WL28681_LDO1_VOUT + i,
			     wl28681->ldo_vout[i]);
	ret = regmap_write(wl28681->regmap, WL28681_LDO_EN, wl28681->ldo_en);

	return 0;
}

static SIMPLE_DEV_PM_OPS(wl28681_pm_ops, wl28681_suspend, wl28681_resume);

static const struct i2c_device_id wl28681_i2c_id[] = {
	{ "wl28681", 0 },
	{ }
};

MODULE_DEVICE_TABLE(i2c, wl28681_i2c_id);

static const struct of_device_id wl28681_of_match[] = {
	{ .compatible = "willsemi,wl28681" },
	{}
};
MODULE_DEVICE_TABLE(of, wl28681_of_match);

static struct i2c_driver wl28681_i2c_driver = {
	.driver = {
		.name = "wl28681",
		.of_match_table = of_match_ptr(wl28681_of_match),
		.pm = &wl28681_pm_ops,
	},
	.id_table = wl28681_i2c_id,
	.probe	= wl28681_i2c_probe,
	.shutdown = wl28681_regulator_shutdown,
};

module_i2c_driver(wl28681_i2c_driver);

MODULE_DESCRIPTION("WL28681 regulator driver");
MODULE_LICENSE("GPL");

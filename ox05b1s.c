
// SPDX-License-Identifier: GPL-2.0-or-later
/*
 * OX05B1S RGB‑IR sensor sub‑device driver for NXP i.MX8 MIPI CSI‑2 hosts
 */
#include <linux/module.h>
#include <linux/i2c.h>
#include <linux/regmap.h>
#include <linux/clk.h>
#include <linux/delay.h>
#include <media/v4l2-async.h>
#include <media/v4l2-ctrls.h>
#include <media/v4l2-subdev.h>

#include "ox05b1s.h"

#define REG_MODE_SELECT 0x0100
#define MODE_STREAMING  0x01
#define MODE_STANDBY    0x00

struct ox05b1s_priv {
    struct v4l2_subdev sd;
    struct media_pad pad;
    struct v4l2_ctrl_handler hdl;
    struct regmap *regmap;
    struct clk *xvclk;
    const struct ox05b1s_mode *cur_mode;
    struct mutex lock; /* serialize I2C */
    struct v4l2_mbus_framefmt fmt;
};

static const struct regmap_config ox05b1s_regmap_cfg = {
    .reg_bits = 16,
    .val_bits = 8,
    .max_register = 0xFFFF,
};

static int ox05b1s_write_array(struct ox05b1s_priv *priv,
                               const struct ox05b1s_regval *list,
                               unsigned int num)
{
    unsigned int i;
    int ret;
    for (i = 0; i < num; i++) {
        ret = regmap_write(priv->regmap, list[i].addr, list[i].val);
        if (ret)
            return ret;
    }
    return 0;
}

static int ox05b1s_set_stream(struct v4l2_subdev *sd, int enable)
{
    struct ox05b1s_priv *priv = container_of(sd, struct ox05b1s_priv, sd);
    int ret = 0;
    mutex_lock(&priv->lock);
    if (enable) {
        ret = ox05b1s_write_array(priv, priv->cur_mode->list,
                                  priv->cur_mode->num_regs);
        if (!ret)
            ret = regmap_write(priv->regmap, REG_MODE_SELECT, MODE_STREAMING);
    } else {
        ret = regmap_write(priv->regmap, REG_MODE_SELECT, MODE_STANDBY);
    }
    mutex_unlock(&priv->lock);
    return ret;
}

/* pad ops */
static int ox05b1s_get_fmt(struct v4l2_subdev *sd,
                           struct v4l2_subdev_state *state,
                           struct v4l2_subdev_format *fmt)
{
    struct ox05b1s_priv *priv = container_of(sd, struct ox05b1s_priv, sd);
    fmt->format = priv->fmt;
    return 0;
}

static const struct v4l2_subdev_pad_ops ox05b1s_pad_ops = {
    .get_fmt = ox05b1s_get_fmt,
};

static const struct v4l2_subdev_video_ops ox05b1s_video_ops = {
    .s_stream = ox05b1s_set_stream,
};

static const struct v4l2_subdev_ops ox05b1s_ops = {
    .pad = &ox05b1s_pad_ops,
    .video = &ox05b1s_video_ops,
};

/* probe */
static int ox05b1s_probe(struct i2c_client *client,
                         const struct i2c_device_id *id)
{
    struct device *dev = &client->dev;
    struct ox05b1s_priv *priv;
    unsigned int val_h, val_l;
    int ret;

    priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
    if (!priv)
        return -ENOMEM;

    mutex_init(&priv->lock);

    priv->regmap = devm_regmap_init_i2c(client, &ox05b1s_regmap_cfg);
    if (IS_ERR(priv->regmap))
        return dev_err_probe(dev, PTR_ERR(priv->regmap), "regmap init failed");

    regmap_read(priv->regmap, OX05B1S_REG_CHIP_ID_H, &val_h);
    regmap_read(priv->regmap, OX05B1S_REG_CHIP_ID_L, &val_l);
    if (((val_h << 8) | val_l) != OX05B1S_CHIP_ID)
        return dev_err_probe(dev, -ENODEV, "chip id mismatch");

    priv->cur_mode = &ox05b1s_2592x1944_60fps_mode;
    priv->fmt.code = MEDIA_BUS_FMT_SRGGB12_1X12;
    priv->fmt.field = V4L2_FIELD_NONE;
    priv->fmt.width = priv->cur_mode->width;
    priv->fmt.height = priv->cur_mode->height;

    v4l2_i2c_subdev_init(&priv->sd, client, &ox05b1s_ops);
    priv->sd.flags |= V4L2_SUBDEV_FL_HAS_DEVNODE;

    priv->pad.flags = MEDIA_PAD_FL_SOURCE;
    ret = media_entity_pads_init(&priv->sd.entity, 1, &priv->pad);
    if (ret)
        return ret;

    ret = v4l2_async_register_subdev(&priv->sd);
    if (ret)
        media_entity_cleanup(&priv->sd.entity);
    dev_info(dev, "OX05B1S sensor probed on i.MX8");
    return ret;
}

static void ox05b1s_remove(struct i2c_client *client)
{
    struct v4l2_subdev *sd = i2c_get_clientdata(client);
    v4l2_async_unregister_subdev(sd);
    media_entity_cleanup(&sd->entity);
}

static const struct i2c_device_id ox05b1s_id_table[] = {
    { "ox05b1s", 0 },
    { }
};
MODULE_DEVICE_TABLE(i2c, ox05b1s_id_table);

static const struct of_device_id ox05b1s_of_match[] = {
    { .compatible = "oxsemi,ox05b1s" },
    { }
};
MODULE_DEVICE_TABLE(of, ox05b1s_of_match);

static struct i2c_driver ox05b1s_driver = {
    .driver = {
        .name = "ox05b1s",
        .of_match_table = ox05b1s_of_match,
    },
    .probe = ox05b1s_probe,
    .remove = ox05b1s_remove,
    .id_table = ox05b1s_id_table,
};
module_i2c_driver(ox05b1s_driver);

MODULE_AUTHOR("Your Name <you@example.com>");
MODULE_DESCRIPTION("OX05B1S camera driver for NXP i.MX8");
MODULE_LICENSE("GPL");

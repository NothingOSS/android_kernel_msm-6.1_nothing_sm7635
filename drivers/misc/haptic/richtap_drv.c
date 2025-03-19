/*
 * drivers/haptic/richtap_drv.c
 *
 * Copyright (c) 2022 ICSense Semiconductor CO., LTD
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation (version 2 of the License only).
 */
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/i2c.h>
#include <linux/of_gpio.h>
#include <linux/delay.h>
#include <linux/device.h>
#include <linux/firmware.h>
#include <linux/slab.h>
#include <linux/version.h>
#include <linux/input.h>
#include <linux/interrupt.h>
#include <linux/debugfs.h>
#include <linux/miscdevice.h>
#include <linux/kfifo.h>
#include <linux/syscalls.h>
#include <linux/power_supply.h>
#include <linux/pm_qos.h>
#include <linux/fb.h>
#include <linux/vmalloc.h>
#include <linux/regmap.h>
#include <linux/mman.h>

#include "richtap_drv.h"
#include "haptic_drv.h"

static void richtap_clean_buffer(struct aac_richtap_data *richtap_data, int status)
{
	struct mmap_buf_format *opbuf = richtap_data->start_buf;
	int i = 0;

	for(i = 0; i < RICHTAP_MMAP_BUF_SUM; i++)
	{
		memset(opbuf->data, 0, RICHTAP_MMAP_BUF_SIZE);
		opbuf->status = status;
		opbuf = opbuf->kernel_next;
	}
}

static void richtap_update_fifo_data(struct ics_haptic_data *haptic_data, uint32_t fifo_len)
{
	struct aac_richtap_data *richtap_data = (struct aac_richtap_data *)(haptic_data->richtap_data);
	int32_t samples_left = 0, pos = 0, retry = 3;

	do
	{
		if(richtap_data->curr_buf->status == MMAP_BUF_DATA_VALID) 
		{
			samples_left = richtap_data->curr_buf->length - richtap_data->pos;
			if(samples_left < fifo_len)
			{
				memcpy(&richtap_data->richtap_ptr[pos], &richtap_data->curr_buf->data[richtap_data->pos], samples_left);
				pos += samples_left;
				fifo_len -= samples_left;
				richtap_data->curr_buf->status = MMAP_BUF_DATA_INVALID;
				richtap_data->curr_buf->length = 0;
				richtap_data->curr_buf = richtap_data->curr_buf->kernel_next;
				richtap_data->pos = 0;
			}
			else
			{
				memcpy(&richtap_data->richtap_ptr[pos], &richtap_data->curr_buf->data[richtap_data->pos], fifo_len);
				richtap_data->pos += fifo_len;
				pos += fifo_len;
				fifo_len = 0;
			}
		}
		else if(richtap_data->curr_buf->status == MMAP_BUF_DATA_FINISHED)
		{
			break;
		}
		else
		{
			if(retry-- <= 0)
			{
				pr_info("invalid data\n");
				break;
			}
			else
			{
				usleep_range(1000,1000);
			}
		}
	} while((fifo_len > 0) && atomic_read(&richtap_data->richtap_stream_mode));

	ics_dbg("update fifo len %d\n", pos);
	haptic_data->func->set_stream_data(haptic_data, richtap_data->richtap_ptr, pos);
}

static void richtap_stream_work_routine(struct work_struct *work)
{
	struct ics_haptic_data *haptic_data = container_of(work, struct ics_haptic_data, richtap_stream_work);
	struct aac_richtap_data *richtap_data = (struct aac_richtap_data *)(haptic_data->richtap_data);
	struct mmap_buf_format *opbuf = NULL;
	uint32_t fifo_size = haptic_data->chip_config.fifo_size;
	uint32_t retry = 30, len = 0;

	opbuf = richtap_data->start_buf;
	do
	{
		if(opbuf->status == MMAP_BUF_DATA_VALID)
		{
			if((len + opbuf->length) <= fifo_size)
			{
				memcpy(&richtap_data->richtap_ptr[len], opbuf->data, opbuf->length);
				len += opbuf->length;
				opbuf->status = MMAP_BUF_DATA_INVALID;
				opbuf->length = 0;
				opbuf = opbuf->kernel_next;
				richtap_data->pos = 0;
			}
			else
			{
				memcpy(&richtap_data->richtap_ptr[len], opbuf->data, (fifo_size - len));
				richtap_data->pos = fifo_size - len;
				len = fifo_size;
			}

			richtap_data->curr_buf = opbuf;
		}
		else if(opbuf->status == MMAP_BUF_DATA_FINISHED)
		{
			break;
		}
		else
		{
			usleep_range(1000, 1000);
		}
	}
	while ((retry-- > 0) && (len < fifo_size));

	ics_dbg("stream len = %d, retry = %d, fifo_size = %d\n", len, retry, fifo_size);

	haptic_data->func->clear_stream_fifo(haptic_data);
	haptic_data->func->get_irq_state(haptic_data);
	haptic_data->func->set_play_mode(haptic_data, PLAY_MODE_STREAM);
	haptic_data->func->play_go(haptic_data);
	haptic_data->func->set_stream_data(haptic_data, richtap_data->richtap_ptr, len);
	atomic_set(&richtap_data->richtap_stream_mode, true);
}

static int richtap_file_open(struct inode *inode, struct file *file)
{
	if (!try_module_get(THIS_MODULE))
	{
		return -ENODEV;
	}

	return 0;
}

static int richtap_file_release(struct inode *inode, struct file *file)
{
	module_put(THIS_MODULE);

	return 0;
}

static long richtap_file_unlocked_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret = 0, val;
	struct ics_haptic_data *haptic_data = dev_get_drvdata(((struct miscdevice *)filp->private_data)->this_device);
	struct aac_richtap_data *richtap_data = (struct aac_richtap_data *)haptic_data->richtap_data;

	switch(cmd)
	{
		case RICHTAP_GET_HWINFO:
			val = RICHTAP_ICS_RT6010;
			if(copy_to_user((void __user *)arg, &val, sizeof(int)))
			{
				ret = -EFAULT;
			}
			break;
		case RICHTAP_RTP_MODE:
			haptic_data->func->play_stop(haptic_data);
			if(copy_from_user(richtap_data->richtap_ptr, (void __user *)arg, RICHTAP_RTP_BUF_SIZE))
			{
				ret = -EFAULT;
				break;
			}
			val = *((int32_t*)richtap_data->richtap_ptr);
			if(val > (RICHTAP_RTP_BUF_SIZE - 4))
			{
				ics_err("%s: invalid data length for rtp mode %d\n", __func__, val);
				ret = -EINVAL;
				break;
			}
			haptic_data->func->clear_stream_fifo(haptic_data);
			haptic_data->func->set_play_mode(haptic_data, PLAY_MODE_STREAM);
			haptic_data->func->play_go(haptic_data);
			haptic_data->func->set_stream_data(haptic_data, &richtap_data->richtap_ptr[4], val);
			break;
		case RICHTAP_OFF_MODE:
			break;
		case RICHTAP_GET_F0:
			val = haptic_data->chip_config.f0;
			if(copy_to_user((void __user *)arg, &val, sizeof(int32_t)))
			{
				ret = -EFAULT;
			}
			break;
		case RICHTAP_SETTING_GAIN:
			if(arg > 0x80)
			{
				arg = 0x80;
			}
			haptic_data->func->set_gain(haptic_data, (uint32_t)arg);
			break;
		case RICHTAP_STREAM_MODE:
			ics_info("%s: richtap ioctl cmd=0x%x, arg=0x%lx\n", __func__, cmd, arg);
			richtap_clean_buffer(richtap_data, MMAP_BUF_DATA_INVALID);
			haptic_data->func->get_irq_state(haptic_data);
			haptic_data->func->play_stop(haptic_data);
			atomic_set(&richtap_data->richtap_stream_mode, false);
			schedule_work(&haptic_data->richtap_stream_work);
			break;
		case RICHTAP_STOP_MODE:
			richtap_clean_buffer(richtap_data, MMAP_BUF_DATA_FINISHED);
			haptic_data->func->get_irq_state(haptic_data);
			haptic_data->func->play_stop(haptic_data);
			atomic_set(&richtap_data->richtap_stream_mode, false);
			break;
		case RICHTAP_DEBUG_INFO:
			ics_info("%s, TODO: RICHTAP_DEBUG_INFO cmd\n", __func__);
			break;
		default:
			ics_err("%s, richtap ioctl unknown cmd\n", __func__);
			break;
	}

	return ret;
}

static int richtap_file_mmap(struct file *filp, struct vm_area_struct *vma)
{
	int ret = 0;
	unsigned long phys;
	struct ics_haptic_data *haptic_data = dev_get_drvdata(((struct miscdevice *)filp->private_data)->this_device);
	struct aac_richtap_data *richtap_data = (struct aac_richtap_data *)haptic_data->richtap_data;

#if LINUX_VERSION_CODE > KERNEL_VERSION(4,7,0)
	//only accept PROT_READ, PROT_WRITE and MAP_SHARED from the API of mmap
	vm_flags_t vm_flags = calc_vm_prot_bits(PROT_READ|PROT_WRITE, 0) | calc_vm_flag_bits(MAP_SHARED);
	vm_flags |= current->mm->def_flags | VM_MAYREAD | VM_MAYWRITE | VM_MAYEXEC| VM_SHARED | VM_MAYSHARE;
	if(vma && (pgprot_val(vma->vm_page_prot) != pgprot_val(vm_get_page_prot(vm_flags))))
		return -EPERM;

	if(vma && ((vma->vm_end - vma->vm_start) != (PAGE_SIZE << RICHTAP_MMAP_PAGE_ORDER)))
		return -ENOMEM;
#endif 
	phys = virt_to_phys(richtap_data->start_buf);

	ret = remap_pfn_range(vma, vma->vm_start, (phys >> PAGE_SHIFT), (vma->vm_end - vma->vm_start), vma->vm_page_prot);
	if(ret)
	{
		ics_err("%s: failed to mmap for richtap\n", __func__);
		return ret;
	}

	return ret;
}

static ssize_t richtap_read(struct file *file,
		char __user *user_buf, size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t richtap_write(struct file *file,
		const char __user *user_buf, size_t count, loff_t *ppos)
{
	return count;
}

static struct file_operations richtap_misc_fops =
{
	.owner = THIS_MODULE,
	.read = richtap_read,
	.write = richtap_write,
	.mmap = richtap_file_mmap,
	.unlocked_ioctl = richtap_file_unlocked_ioctl,
	.open = richtap_file_open,
	.release = richtap_file_release,
};

static struct miscdevice richtap_misc_dev =
{
	.minor = MISC_DYNAMIC_MINOR,
	.name = DEFAULT_RICHTAP_NAME,
	.fops = &richtap_misc_fops,
};

int32_t richtap_misc_register(struct ics_haptic_data *haptic_data)
{
	int32_t ret = 0;
	struct aac_richtap_data *richtap_data = NULL;

	ics_info("%s: start register richtap misc dev\n", __func__);
	richtap_data = devm_kzalloc(&haptic_data->client->dev, sizeof(struct aac_richtap_data), GFP_KERNEL);
	if (richtap_data == NULL)
	{
		ics_err("%s: failed to kmalloc memory to richtap haptic data!\n", __func__);
		return -ENOMEM;
	}
	haptic_data->richtap_data = richtap_data;

	richtap_data->richtap_ptr = devm_kzalloc(&haptic_data->client->dev, RICHTAP_RTP_BUF_SIZE, GFP_KERNEL);
	if(richtap_data->richtap_ptr == NULL)
	{
		ics_err("%s: failed to kmalloc rtp memory failed\n", __func__);
		goto richtap_err;
	}

	richtap_data->start_buf = (struct mmap_buf_format *)__get_free_pages(GFP_KERNEL, RICHTAP_MMAP_PAGE_ORDER);
	if(richtap_data->start_buf == NULL)
	{
		ics_err("%s: Error __get_free_pages failed\n", __func__);
		goto richtap_err;
	}
	SetPageReserved(virt_to_page(richtap_data->start_buf));
	{
		struct mmap_buf_format *temp;
		uint32_t i = 0;
		temp = richtap_data->start_buf;
		for( i = 1; i < RICHTAP_MMAP_BUF_SUM; i++)
		{
			temp->kernel_next = (richtap_data->start_buf + i);
			temp = temp->kernel_next;
		}
		temp->kernel_next = richtap_data->start_buf;
	}
	INIT_WORK(&haptic_data->richtap_stream_work, richtap_stream_work_routine);

	richtap_misc_dev.name = haptic_data->richtap_misc_name;
	ret = misc_register(&richtap_misc_dev);
	if (ret != 0)
	{
		ics_err("%s: failed to register misc device\n", __func__);
		goto richtap_err;
	}
	dev_set_drvdata(richtap_misc_dev.this_device, haptic_data);

	ics_info("%s: end register richtap misc dev\n", __func__);
	return 0;

richtap_err:
	richtap_misc_remove(haptic_data);
	return ret;
}

int32_t richtap_misc_remove(struct ics_haptic_data *haptic_data)
{
	struct aac_richtap_data *richtap_data = haptic_data->richtap_data;

	if (richtap_data != NULL)
	{
		cancel_work_sync(&haptic_data->richtap_stream_work);
		if (richtap_data->richtap_ptr != NULL)
		{
			devm_kfree(&haptic_data->client->dev, richtap_data->richtap_ptr);
		}
		if (richtap_data->start_buf != NULL)
		{
			ClearPageReserved(virt_to_page(richtap_data->start_buf));
			free_pages((unsigned long)richtap_data->start_buf, RICHTAP_MMAP_PAGE_ORDER);
		}

		devm_kfree(&haptic_data->client->dev, richtap_data);
	}
	misc_deregister(&richtap_misc_dev);
	return 0;
}

int32_t richtap_irq_handler(void *data)
{
	struct ics_haptic_data *haptic_data = (struct ics_haptic_data *)data;
	struct aac_richtap_data *richtap_data = haptic_data->richtap_data;
	uint32_t available_size = haptic_data->chip_config.fifo_size - haptic_data->chip_config.fifo_ae;

	if(atomic_read(&richtap_data->richtap_stream_mode))
	{
		if(haptic_data->func->is_irq_fifo_ae(haptic_data))
		{
			//ics_dbg("%s: irq state fifo almost empty\n", __func__);
			richtap_update_fifo_data(haptic_data, available_size);
		}

		if(richtap_data->curr_buf->status == MMAP_BUF_DATA_INVALID)
		{
			atomic_set(&richtap_data->richtap_stream_mode, false);
		}

		return 0;
	}

	return -1;
}

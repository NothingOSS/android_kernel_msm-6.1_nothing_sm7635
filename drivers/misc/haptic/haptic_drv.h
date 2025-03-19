#ifndef __ICS_HAPTIC_DRV_H__
#define __ICS_HAPTIC_DRV_H__

#include <linux/kfifo.h>
#include <linux/mutex.h>
#include <linux/workqueue.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>
#include <linux/device.h>
#include <linux/types.h>
#include <linux/leds.h>

#define DEBUG
#define AAC_RICHTAP_SUPPORT

#define ics_err(format, ...) \
	pr_err("[ics_haptic]" format, ##__VA_ARGS__)

#define ics_info(format, ...) \
	pr_err("[ics_haptic]" format, ##__VA_ARGS__)

#define ics_dbg(format, ...) \
	pr_err("[ics_haptic]" format, ##__VA_ARGS__)

#define check_error_return(ret)	\
	if (ret < 0) {	\
		pr_err("%s: check_error_return! ret = %d\n", __func__, ret);	\
		return ret;	\
	}	\

#ifdef TIMED_OUTPUT
#include "../staging/android/timed_output.h"
typedef struct timed_output_dev vib_dev_t;
#else
typedef struct led_classdev vib_dev_t;
#endif

#define ICS_HAPTIC_VERSION     "v0.9.8"
#define ICS_HAPTIC_NAME		"haptic_rt"
#define MAX_STREAM_FIFO_SIZE	4096
#define MAX_PRESET_NAME_LEN	 64
#define DEFAULT_DEV_NAME		"vibrator"
#define GP_BUFFER_SIZE          1536

#define DEFAULT_F0              1700
#define INVALID_DATA	-400
#define MAX_DAQ_COUNT           256
#define DAQ_UNIT_SIZE           6
#define MAX_DAQ_BUF_SIZE        (MAX_DAQ_COUNT * DAQ_UNIT_SIZE + 4)

#define MAX_BEMF_DAQ_COUNT      40
#define MAX_BEMF_DAQ_UNIT       6
#define MAX_BEMF_BUF_SIZE       (MAX_BEMF_DAQ_COUNT * MAX_BEMF_DAQ_UNIT + 4)
#define RESAMPLE_THRESHOLD      20

enum ics_haptic_play_mode
{
	PLAY_MODE_RAM		   = 0x01,
	PLAY_MODE_STREAM		= 0x02,
	PLAY_MODE_TRACK		 = 0x03,
	PLAY_MODE_RAM_LOOP      = 0x01,
};

enum ics_haptic_boost_mode
{
	BOOST_MODE_NORMAL	   = 0x00,
	BOOST_MODE_BYPASS	   = 0x01,
};

enum ics_hatpic_sys_state
{
	SYS_STATE_STOP		  = 0x00,
	SYS_STATE_RAM_PLAY	  = 0X01,
	SYS_STATE_STREAM_PLAY   = 0X02,
	SYS_STATE_TRACK_PLAY	= 0X03,
	SYS_STATE_TRIG		  = 0x04,
	SYS_STATE_DETECT		= 0x05,
	SYS_STATE_BRAKE		 = 0X06,
};

struct ics_haptic_chip_config
{
	uint32_t chip_id;
	uint32_t reg_size;
	uint32_t ram_size;
	uint32_t sys_f0;
	uint32_t f0;
	uint32_t list_base_addr;
	uint32_t wave_base_addr;
	uint32_t fifo_ae;
	uint32_t fifo_af;
	int32_t fifo_size;
	int32_t list_section_size;
	int32_t wave_section_size;
	uint32_t boost_mode;
	uint32_t boost_vol;
	uint32_t gain;
	uint32_t brake_en;
	uint32_t brake_wave_no;
	uint32_t brake_wave_mode;
	uint32_t brake_const;
	uint32_t brake_acq_point;
	int32_t vbat;
	int32_t resistance;
};

struct ics_haptic_data
{
	struct ics_haptic_chip_config chip_config;
	uint32_t waveform_size;
	uint8_t *waveform_data;
	//
	vib_dev_t vib_dev;
	char vib_name[64];
	//
	struct i2c_client *client;
	struct device *dev;
	struct regmap *regmap;
	struct mutex lock;
	struct mutex preset_lock;
	struct hrtimer timer;
	struct delayed_work chip_init_work;
	struct work_struct vibrator_work;
	struct work_struct preset_work;
	struct work_struct brake_guard_work;
	struct ics_haptic_func *func;
	//
	bool chip_initialized;
	bool stream_start;
	struct kfifo stream_fifo;
	uint8_t *gp_buf;
	//
	uint32_t play_mode;
	uint32_t play_status;
	int32_t amplitude;
	uint32_t preset_wave_index;
	uint32_t ram_wave_index;
	uint32_t duration;
	uint32_t activate_state;
	uint32_t sys_state;
	uint32_t irq_state;
	int32_t adc_offset;
	//
	int32_t gpio_en;
	int32_t gpio_irq;
	//
	uint32_t efs_data;
	uint8_t efs_ver;
	//
	uint32_t daq_en;
	struct mutex daq_lock;
	int32_t daq_duration;
	int32_t daq_size;
	uint8_t *daq_data;
	struct hrtimer reset_timer;
	// for input dev
	struct input_dev *input_dev;
	struct workqueue_struct *input_work_queue;
	struct hrtimer input_timer;
	struct mutex input_lock;
	enum ics_haptic_play_mode activate_mode;
	enum ics_haptic_play_mode current_mode;
	int32_t effect_type;
	int32_t effect_id;
	int32_t state;
	struct work_struct input_vibrator_work;
	uint8_t level;
	uint32_t autotrack_f0;
	uint32_t bemf_f0;
	uint32_t f0_en;
	uint32_t bemf_daq_en;
	uint32_t bemf_daq_size;
	uint8_t bemf_daq_buf[MAX_BEMF_BUF_SIZE];
	uint32_t gpp;
	uint8_t *config_data;
	uint32_t config_size;
	uint32_t nt_backup_f0;//Write the value of MMI calibration f0 in the upper layer
	uint32_t nt_cmdline_f0;//record lk stage f0 exceeding threshold
#ifdef AAC_RICHTAP_SUPPORT
	char richtap_misc_name[64];
	void* richtap_data;
	struct work_struct richtap_stream_work;
#endif
};

struct ics_haptic_func
{
	int32_t (*chip_init)(struct ics_haptic_data *, const uint8_t *, int32_t);
	int32_t (*get_chip_id)(struct ics_haptic_data *);
	int32_t (*get_reg)(struct ics_haptic_data *, uint32_t, uint32_t *);
	int32_t (*set_reg)(struct ics_haptic_data *, uint32_t, uint32_t);
	int32_t (*get_f0)(struct ics_haptic_data *);
	int32_t (*set_f0)(struct ics_haptic_data *, uint32_t);
	int32_t (*get_play_mode)(struct ics_haptic_data *);
	int32_t (*set_play_mode)(struct ics_haptic_data *, uint32_t);
	int32_t (*play_go)(struct ics_haptic_data *);
	int32_t (*play_stop)(struct ics_haptic_data *);
	int32_t (*get_play_status)(struct ics_haptic_data *);
	int32_t (*set_brake_en)(struct ics_haptic_data *, uint32_t);
	int32_t (*set_daq_en)(struct ics_haptic_data *, uint32_t);
	int32_t (*set_f0_en)(struct ics_haptic_data *, uint32_t);
	int32_t (*set_gain)(struct ics_haptic_data *, uint32_t);
	int32_t (*set_bst_vol)(struct ics_haptic_data *, uint32_t);
	int32_t (*set_bst_mode)(struct ics_haptic_data *, uint32_t);
	int32_t (*set_play_list)(struct ics_haptic_data *, const uint8_t *, int32_t);
	int32_t (*get_ram_data)(struct ics_haptic_data *, uint8_t *, int32_t *);
	int32_t (*set_waveform_data)(struct ics_haptic_data *, const uint8_t *, int32_t);
	int32_t (*set_sys_data)(struct ics_haptic_data *, const uint8_t *, int32_t);
	int32_t (*get_daq_data)(struct ics_haptic_data *, uint8_t *, int32_t *);
	int32_t (*clear_stream_fifo)(struct ics_haptic_data *);
	int32_t (*set_stream_data)(struct ics_haptic_data *, const uint8_t *, int32_t);
	int32_t (*get_sys_state)(struct ics_haptic_data *);
	int32_t (*get_vbat)(struct ics_haptic_data *);
	int32_t (*get_resistance)(struct ics_haptic_data *);
	int32_t (*get_irq_state)(struct ics_haptic_data *);
	int32_t (*get_adc_offset)(struct ics_haptic_data *);
	int32_t (*resample_ram_waveform)(struct ics_haptic_data *);
	bool (*is_irq_play_done)(struct ics_haptic_data *);
	bool (*is_irq_fifo_ae)(struct ics_haptic_data *);
	bool (*is_irq_fifo_af)(struct ics_haptic_data *);
	bool (*is_irq_protection)(struct ics_haptic_data *);
	int32_t (*clear_protection)(struct ics_haptic_data *);
	int32_t (*acquire_bemf_data)(struct ics_haptic_data *);
	int32_t (*get_gpp)(struct ics_haptic_data *);
};

extern struct ics_haptic_func rt6010_func_list;

#ifdef AAC_RICHTAP_SUPPORT
#define DEFAULT_RICHTAP_NAME	"aac_richtap"
extern int32_t richtap_misc_register(struct ics_haptic_data *haptic_data);
extern int32_t richtap_misc_remove(struct ics_haptic_data *haptic_data);
extern int32_t richtap_irq_handler(void *data);
#endif

#endif // __ICS_HAPTIC_DRV_H__

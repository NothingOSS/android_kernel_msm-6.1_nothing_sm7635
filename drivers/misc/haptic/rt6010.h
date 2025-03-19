#ifndef __ICS_RT6010_H__
#define __ICS_RT6010_H__

/******************************************************************************
 * rt6010 chip id
******************************************************************************/
#define RT6010_CHIP_ID                     0x6B

/******************************************************************************
 * rt6010 register address
******************************************************************************/
#define RT6010_REG_DEV_ID				  0x00
#define RT6010_REG_REV_ID				  0x01
#define RT6010_REG_SOFT_RESET			  0x02
#define RT6010_REG_SYS_STATUS1			 0x03
#define RT6010_REG_SYS_STATUS2			 0x04
#define RT6010_REG_INT_STATUS			  0x05
#define RT6010_REG_INT_MASK				0x06
#define RT6010_REG_RAM_CFG				 0x07
#define RT6010_REG_RAM_ADDR_L			  0x08
#define RT6010_REG_RAM_ADDR_H			  0x09
#define RT6010_REG_RAM_DATA				0x0A
#define RT6010_REG_STREAM_DATA			 0x0B
#define RT6010_REG_FIFO_AE_L			   0x0C
#define RT6010_REG_FIFO_AE_H			   0x0D
#define RT6010_REG_FIFO_AF_L			   0x0E
#define RT6010_REG_FIFO_AF_H			   0x0F
#define RT6010_REG_PLAY_CFG				0x10
#define RT6010_REG_PLAY_MODE			   0x11
#define RT6010_REG_PLAY_CTRL			   0x12
#define RT6010_REG_GPIO1_POS_ENTRY		 0x13
#define RT6010_REG_GPIO1_NEG_ENTRY		 0x14
#define RT6010_REG_GPIO2_POS_ENTRY		 0x15
#define RT6010_REG_GPIO2_NEG_ENTRY		 0x16
#define RT6010_REG_GPIO3_POS_ENTRY		 0x17
#define RT6010_REG_GPIO3_NEG_ENTRY		 0x18
#define RT6010_REG_GPIO_STATUS			 0x19
#define RT6010_REG_PRIORITY_CFG			0x1A
#define RT6010_REG_WAVE_BASE_ADDR_L		0x1C
#define RT6010_REG_WAVE_BASE_ADDR_H		0x1D
#define RT6010_REG_LIST_BASE_ADDR_L		0x1E
#define RT6010_REG_LIST_BASE_ADDR_H		0x1F
#define RT6010_REG_GAIN_CFG				0x20
#define RT6010_REG_SYS_CFG				 0x23
#define RT6010_REG_MODULE_EN			0x24
#define RT6010_REG_STATE_MACHINE_CFG	   0x25
#define RT6010_REG_PROTECTION_STATUS1	  0x26
#define RT6010_REG_PROTECTION_STATUS2	  0x27
#define RT6010_REG_PROTECTION_MASK1		0x28
#define RT6010_REG_PROTECTION_MASK2		0x29
#define RT6010_REG_MANNUAL			0x2B
#define RT6010_REG_ERROR_CODE			  0x2C
#define RT6010_REG_LRA_F0_CFG1			 0x2D
#define RT6010_REG_LRA_F0_CFG2			 0x2E
#define RT6010_REG_DETECT_F0_CFG		0x2F
#define RT6010_REG_BEMF_CZ1_VAL1		   0x30
#define RT6010_REG_BEMF_CZ1_VAL2		   0x31
#define RT6010_REG_BEMF_CZ2_VAL1		   0x32
#define RT6010_REG_BEMF_CZ2_VAL2		   0x33
#define RT6010_REG_BEMF_CZ3_VAL1		   0x34
#define RT6010_REG_BEMF_CZ3_VAL2		   0x35
#define RT6010_REG_BEMF_CZ4_VAL1		   0x36
#define RT6010_REG_BEMF_CZ4_VAL2		   0x37
#define RT6010_REG_BEMF_CZ5_VAL1		   0x38
#define RT6010_REG_BEMF_CZ5_VAL2		   0x39
#define RT6010_REG_BRAKE_CFG1			  0x3A
#define RT6010_REG_BRAKE_CFG2			  0x3B
#define RT6010_REG_BRAKE_CFG3			  0x3c
#define RT6010_REG_BRAKE_CFG4			  0x3D
#define RT6010_REG_BRAKE_CFG5			  0x3E
#define RT6010_REG_BRAKE_CFG6			  0x3F
#define RT6010_REG_BRAKE_CFG7			  0x40
#define RT6010_REG_BRAKE_CFG8			  0x41
#define RT6010_REG_TRACK_CFG1			  0x44
#define RT6010_REG_TRACK_CFG2			  0x45
#define RT6010_REG_TRACK_CFG3			  0x46
#define RT6010_REG_TRACK_F0_VAL1		   0x47
#define RT6010_REG_TRACK_F0_VAL2		   0x48
#define RT6010_REG_TIMING_CFG1			 0x49
#define RT6010_REG_ADC_DATA1			   0x4A
#define RT6010_REG_ADC_DATA2			   0x4B
#define RT6010_REG_BEMF_CFG1			   0x50
#define RT6010_REG_BEMF_CFG2			   0x51
#define RT6010_REG_BEMF_CFG3			   0x52
#define RT6010_REG_BEMF_CFG4			   0x53
#define RT6010_REG_BOOST_CFG1			  0x55
#define RT6010_REG_BOOST_CFG2			  0x56
#define RT6010_REG_BOOST_CFG3			  0x57
#define RT6010_REG_BOOST_CFG4			  0x58
#define RT6010_REG_BOOST_CFG5			  0x59
#define RT6010_REG_PA_CFG1				 0x5A
#define RT6010_REG_PA_CFG2				 0x5B
#define RT6010_REG_PMU_CFG1				0x5C
#define RT6010_REG_PMU_CFG2				0x5D
#define RT6010_REG_PMU_CFG3				0x5E
#define RT6010_REG_PMU_CFG4				0x5F
#define RT6010_REG_OSC_CFG1				0x60
#define RT6010_REG_OSC_CFG2				0x61
#define RT6010_REG_INT_CFG				 0x62
#define RT6010_REG_GPIO_CFG1			   0x63
#define RT6010_REG_GPIO_CFG2			   0x64
#define RT6010_REG_GPIO_CFG3			   0x65
#define RT6010_REG_EFS_WR_DATA			 0x66
#define RT6010_REG_EFS_RD_DATA			 0x67
#define RT6010_REG_EFS_ADDR_INDEX		  0x68
#define RT6010_REG_EFS_MODE_CTRL		   0x69
#define RT6010_REG_EFS_PGM_STROBE_WIDTH	0x6A
#define RT6010_REG_EFS_READ_STROBE_WIDTH   0x6B
#define RT6010_REG_RAM_DATA_READ		   0xFF

/******************************************************************************
 * ics6b1x register bit detail
******************************************************************************/
// RT6010_REG_PLAY_MODE
#define RT6010_BIT_PLAY_MODE_MASK		  (7 << 0)
#define RT6010_BIT_PLAY_MODE_RAM		   (1 << 0)
#define RT6010_BIT_PLAY_MODE_STREAM		(2 << 0)
#define RT6010_BIT_PLAY_MODE_TRACK		 (3 << 0)

// RT6010_REG_PLAY_CTRL
#define RT6010_BIT_GO_MASK				 (1 << 0)
#define RT6010_BIT_GO_START				(1 << 0)
#define RT6010_BIT_GO_STOP				 (0 << 0)

// RT6010_REG_INT_STATUS
#define RT6010_BIT_INTS_PLAYDONE		   (1 << 3)
#define RT6010_BIT_INTS_FIFO_AE			(1 << 2)
#define RT6010_BIT_INTS_FIFO_AF			(1 << 1)
#define RT6010_BIT_INTS_PROTECTION		 (1 << 0)

// RT6010_REG_EFS_MODE_CTRL
#define RT6010_BIT_EFS_READ				(1 << 1)
#define RT6010_BIT_EFS_PGM				 (1 << 0)

// PMU_CFG3
#define RT6010_BIT_OSC_LDO_TRIM_MASK	   (3 << 6)
#define RT6010_BIT_PMU_LDO_TRIM_MASK	   (3 << 4)
#define RT6010_BIT_PMU_VBAT_TRIM_MASK	  (3 << 0)

// PMU_CFG4
#define RT6010_BIT_BIAS_1P2V_TRIM_MASK	 (0x0F << 4)
#define RT6010_BIT_BIAS_I_TRIM_MASK		(0x0F << 0)

// BOOST_CFG3
#define RT6010_BIT_BST_VOUT_TRIM_MASK	  (0x0F << 4)

// OSC_CFG1
#define RT6010_BIT_OSC_TRIM_MASK		   (0xFF << 0)

// BEMF_CFG1
#define RT6010_BIT_BEMF_STAGE_TRIM_MASK	(0X0F << 0)

/******************************************************************************
 * rt6010 efuse layout
******************************************************************************/
#define RT6010_EFS_BIAS_1P2V_TRIM_OFFSET   0x00
#define RT6010_EFS_BIAS_I_TRIM_OFFSET	  0x04
#define RT6010_EFS_PMU_LDO_TRIM_OFFSET	 0x08
#define RT6010_EFS_OSC_LDO_TRIM_OFFSET	 0x09
#define RT6010_EFS_OSC_TRIM_OFFSET		 0x0A
#define RT6010_EFS_VBAT_OFFSET_OFFSET	  0x12
#define RT6010_EFS_RL_OFFSET_OFFSET		0x17
#define RT6010_EFS_VERSION_OFFSET		  0x1E

#define RT6010_EFS_BIAS_1P2V_TRIM_MASK	 0x0F
#define RT6010_EFS_BIAS_I_TRIM_MASK		0xF0
#define RT6010_EFS_PMU_LDO_TRIM_MASK	   0x100
#define RT6010_EFS_OSC_LDO_TRIM_MASK	   0x200
#define RT6010_EFS_OSC_TRIM_MASK		   0x3FC00
#define RT6010_EFS_VBAT_OFFSET_MASK		0x7C0000
#define RT6010_EFS_RL_OFFSET_MASK		  0x3F800000
#define RT6010_EFS_VERSION_MASK			0xC0000000

enum rt6010_boost_vol
{
	BOOST_VOUT_600	= 0,
	BOOST_VOUT_625	= 1,
	BOOST_VOUT_650	= 2,
	BOOST_VOUT_675	= 3,
	BOOST_VOUT_700	= 4,
	BOOST_VOUT_725	= 5,
	BOOST_VOUT_750	= 6,
	BOOST_VOUT_775	= 7,
	BOOST_VOUT_800	= 8,
	BOOST_VOUT_825	= 9,
	BOOST_VOUT_850	= 10,
	BOOST_VOUT_875	= 11,
	BOOST_VOUT_900	= 12,
	BOOST_VOUT_925	= 13,
	BOOST_VOUT_100	= 14,
	BOOST_VOUT_110	= 15,
};

#endif // __ICS_RT6010_H__

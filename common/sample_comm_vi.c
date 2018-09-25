/******************************************************************************
  Some simple Hisilicon Hi35xx video input functions.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-8 Created
******************************************************************************/
#ifdef __cplusplus
#if __cplusplus
extern "C" {
#endif
#endif /* End of #ifdef __cplusplus */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/poll.h>
#include <sys/time.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include <signal.h>
#include "hi_mipi.h"

#include "hi_common.h"
#include "sample_comm.h"

VI_DEV_ATTR_S DEV_ATTR_BT656D1_1MUX =
{
    /* interface mode */
    VI_MODE_BT656,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0x0},
    /* progessive or interleaving */
    VI_SCAN_INTERLACED,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_YVYU,     
    /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,    
    
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            0,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            0,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/ 
     0,            0,            0}
    },    
    /* ISP bypass */
    VI_PATH_BYPASS,
    /* input data type */
    VI_DATA_TYPE_YUV,    
    /* bReverse */
    HI_FALSE,    
     /* DEV CROP */
    {0, 0, 1920, 1080}
};

/* BT1120 1080I input */
VI_DEV_ATTR_S DEV_ATTR_BT1120_1080I_1MUX =
{
    /* interface mode */
    VI_MODE_BT1120_STANDARD,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0xFF0000},
    /* progessive or interleaving */
    VI_SCAN_INTERLACED,
    /*AdChnId*/
    { -1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_UVUV,

    /* synchronization information */
    {
        /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
        VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL, VI_HSYNC_NEG_HIGH, VI_VSYNC_NORM_PULSE, VI_VSYNC_VALID_NEG_HIGH,

        /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            0,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            0,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    /* ISP bypass */
    VI_PATH_BYPASS,
    /* input data type */
    VI_DATA_TYPE_YUV,
    /* bReverse */
    HI_FALSE,
    /* DEV CROP */
    {0, 0, 1920, 1080}
};

/* BT1120 1080p */
VI_DEV_ATTR_S DEV_ATTR_BT1120_1080P_BASE =
{
    /* interface mode */
    VI_MODE_BT1120_STANDARD,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0xFF0000},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_UVUV,

    /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,
   
    /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            0,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            0,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },    
    /* ISP bypass */
    VI_PATH_BYPASS,
     /* input data type */
    VI_DATA_TYPE_YUV,
    /* bReverse */
    HI_FALSE,    
     /* DEV CROP */
    {0, 0, 1920, 1080}
};

 
/* BT1120 720P */
VI_DEV_ATTR_S DEV_ATTR_BT1120_720P_BASE =
/* classical timing 3:7441 BT1120 720P@60fps*/
{
    /* interface mode */
    VI_MODE_BT1120_STANDARD,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFF000000,    0xFF0000},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    //VI_SCAN_INTERLACED,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_UVUV,

     /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_NORM_PULSE,VI_VSYNC_VALID_NEG_HIGH,
    
    /*hsync_hfb    hsync_act    hsync_hhb*/
        {
            0,            1280,        0,
            /*vsync0_vhb vsync0_act vsync0_hhb*/
            0,            720,        0,
            /*vsync1_vhb vsync1_act vsync1_hhb*/
            0,            0,            0
        }
    },
    /* ISP bypass */
    VI_PATH_BYPASS,
    /* input data type */
    VI_DATA_TYPE_YUV,
    /* bReverse */
    HI_FALSE,    
     /* DEV CROP */
    {0, 0, 1280, 720}
};


/*imx222 DC 12bit input*/
VI_DEV_ATTR_S DEV_ATTR_IMX222_DC_1080P_BASE =
{
    /* interface mode */
    VI_MODE_DIGITAL_CAMERA,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFFF0000,    0x0},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_YUYV,

     /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,    
    
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1920,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            1080,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/ 
     0,            0,            0}
    },
    /* use interior ISP */
    VI_PATH_ISP,
    /* input data type */
    VI_DATA_TYPE_RGB,
    /* bRevert */
    HI_FALSE,
    /* stDevRect */
    {200, 20, 1920, 1080} 
};

/*imx222 DC 12bit input*/
VI_DEV_ATTR_S DEV_ATTR_IMX222_DC_720P_BASE =
{
    /* interface mode */
    VI_MODE_DIGITAL_CAMERA,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFFF0000,    0x0},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_YUYV,

     /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,    
    
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/ 
     0,            0,            0}
    },
    /* use interior ISP */
    VI_PATH_ISP,
    /* input data type */
    VI_DATA_TYPE_RGB,
    /* bRevert */
    HI_FALSE,
    /* stDevRect */
    {200, 20, 1280, 720} 
};

/*9M034 DC 12bit input 720P@30fps*/
VI_DEV_ATTR_S DEV_ATTR_9M034_DC_720P_BASE =
{
    /* interface mode */
    VI_MODE_DIGITAL_CAMERA,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask 掩码*/
    {0xFFF0000,    0x0},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_YUYV,

    /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {370,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     6,            720,        6,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /* use interior ISP */
    VI_PATH_ISP,
    /* input data type */
    VI_DATA_TYPE_RGB,
    /* Data Reverse */
    HI_FALSE,
    {0, 0, 1280, 720}
};

/*OV9732 DC 12bit input 720P@30fps*/
VI_DEV_ATTR_S DEV_ATTR_OV9732_DC_720P_BASE =
{
    /* interface mode */
    VI_MODE_DIGITAL_CAMERA,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFFC0000,    0x0},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_YUYV,

    /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_FIELD, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,

    /*hsync_hfb    hsync_act    hsync_hhb*/
    {370,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     6,            720,        6,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /* use interior ISP */
    VI_PATH_ISP,
    /* input data type */
    VI_DATA_TYPE_RGB,
    /* Data Reverse */
    HI_FALSE,
    {0, 0, 1280, 720}
};


/*mt9p006 DC 12bit input*/
VI_DEV_ATTR_S DEV_ATTR_MT9P006_DC_1080P =
{
    /* interface mode */
    VI_MODE_DIGITAL_CAMERA,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFFF00000,    0x0},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
   /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_YUYV,

    /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_HIGH, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,    
   
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1920,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            1080,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/ 
     0,            0,            0}
    },
    /* use interior ISP */
    VI_PATH_ISP,
    /* input data type */
    VI_DATA_TYPE_RGB    
};

VI_DEV_ATTR_S DEV_ATTR_LVDS_BASE =
{
    /* interface mode */
    VI_MODE_LVDS,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFFF00000,    0x0},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_YUYV,

    /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,
   
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /* use interior ISP */
    VI_PATH_ISP,
    /* input data type */
    VI_DATA_TYPE_RGB,    
    /* bRever */
    HI_FALSE,    
    /* DEV CROP */
    {0, 0, 1920, 1080}
};

VI_DEV_ATTR_S DEV_ATTR_HISPI_BASE =
{
    /* interface mode */
    VI_MODE_HISPI,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFFF00000,    0x0},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_YUYV,

    /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,
   
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /* use interior ISP */
    VI_PATH_ISP,
    /* input data type */
    VI_DATA_TYPE_RGB,    
    /* bRever */
    HI_FALSE,    
    /* DEV CROP */
    {0, 0, 1920, 1080}
};


VI_DEV_ATTR_S DEV_ATTR_MIPI_BASE =
{
    /* interface mode */
    VI_MODE_MIPI,
    /* multiplex mode */
    VI_WORK_MODE_1Multiplex,
    /* r_mask    g_mask    b_mask*/
    {0xFFF00000,    0x0},
    /* progessive or interleaving */
    VI_SCAN_PROGRESSIVE,
    /*AdChnId*/
    {-1, -1, -1, -1},
    /*enDataSeq, only support yuv*/
    VI_INPUT_DATA_YUYV,

    /* synchronization information */
    {
    /*port_vsync   port_vsync_neg     port_hsync        port_hsync_neg        */
    VI_VSYNC_PULSE, VI_VSYNC_NEG_LOW, VI_HSYNC_VALID_SINGNAL,VI_HSYNC_NEG_HIGH,VI_VSYNC_VALID_SINGAL,VI_VSYNC_VALID_NEG_HIGH,
   
    /*hsync_hfb    hsync_act    hsync_hhb*/
    {0,            1280,        0,
    /*vsync0_vhb vsync0_act vsync0_hhb*/
     0,            720,        0,
    /*vsync1_vhb vsync1_act vsync1_hhb*/
     0,            0,            0}
    },
    /* use interior ISP */
    VI_PATH_ISP,
    /* input data type */
    VI_DATA_TYPE_RGB,    
    /* bRever */
    HI_FALSE,    
    /* DEV CROP */
    {0, 0, 1920, 1080}
};


combo_dev_attr_t LVDS_4lane_SENSOR_IMX136_12BIT_1080_NOWDR_ATTR =
{
    /* input mode */
    .input_mode = INPUT_MODE_LVDS,
    {
        .lvds_attr = {
            .img_size = {1920, 1080},
            HI_WDR_MODE_NONE,
            LVDS_SYNC_MODE_SAV,
            RAW_DATA_12BIT,
            LVDS_ENDIAN_BIG,
            LVDS_ENDIAN_BIG,
            .lane_id = {0, 1, 2, 3, -1, -1, -1, -1},
            .sync_code = {
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},

                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},

                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                        
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},

                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}}
                     
               }
        }
    }
};

combo_dev_attr_t SUBLVDS_4lane_SENSOR_MN34220_12BIT_1080_NOWDR_ATTR =
{
    /* input mode */
    .input_mode = INPUT_MODE_SUBLVDS,
        
    {
        .lvds_attr = {
            .img_size = {1920, 1080},
            HI_WDR_MODE_NONE,            
            LVDS_SYNC_MODE_SOL,
            RAW_DATA_12BIT,                     
            LVDS_ENDIAN_BIG,
            LVDS_ENDIAN_BIG, 
            .lane_id = {0, 2, -1, -1, 1, 3, -1, -1},
            .sync_code =  {
                  {{0x002, 0x003, 0x000, 0x001}, //PHY0_lane0
                  {0x202, 0x203, 0x200, 0x201},
                  {0x102, 0x103, 0x100, 0x101},
                  {0x302, 0x303, 0x300, 0x301}},

                  {{0x006, 0x007, 0x004, 0x005}, //PHY0_lane1
                  {0x206, 0x207, 0x204, 0x205},
                  {0x106, 0x107, 0x104, 0x105},
                  {0x306, 0x307, 0x304, 0x305}},

                  {{0x00a, 0x00b, 0x008, 0x009}, //PHY0_lane2
                  {0x20a, 0x20b, 0x208, 0x209},
                  {0x10a, 0x10b, 0x108, 0x109},
                  {0x30a, 0x30b, 0x308, 0x309}},

                  {{0x00a, 0x00b, 0x008, 0x009}, //PHY0_lane3  INPUT_MODE_LVDS
                  {0x20a, 0x20b, 0x208, 0x209},
                  {0x10a, 0x10b, 0x108, 0x109},
                  {0x30a, 0x30b, 0x308, 0x309}},

                  {{0x012, 0x013, 0x010, 0x011},//PHY1_lane0
                  {0x212, 0x213, 0x210, 0x211},
                  {0x112, 0x113, 0x110, 0x111},
                  {0x312, 0x313, 0x310, 0x311}},

                  {{0x016, 0x017, 0x014, 0x015}, //PHY1_lane1
                  {0x216, 0x217, 0x214, 0x215},
                  {0x116, 0x117, 0x114, 0x115},
                  {0x316, 0x317, 0x314, 0x315}},

                  {{0x01a, 0x01b, 0x018, 0x019}, //PHY1_lane2
                  {0x21a, 0x21b, 0x218, 0x219},
                  {0x11a, 0x11b, 0x118, 0x119},
                  {0x31a, 0x31b, 0x318, 0x319}},

                  {{0x01a, 0x01b, 0x018, 0x019}, //PHY1_lane3
                  {0x21a, 0x21b, 0x218, 0x219},
                  {0x11a, 0x11b, 0x118, 0x119},
                  {0x31a, 0x31b, 0x318, 0x319}}
               }
        }
    }
};

combo_dev_attr_t SUBLVDS_4lane_SENSOR_MN34220_12BIT_1080_2WDR1_ATTR =
{
    /* input mode */
    .input_mode = INPUT_MODE_SUBLVDS,
        
    {
        .lvds_attr = {
            .img_size = {1920, 1108},
            HI_WDR_MODE_2F,            
            LVDS_SYNC_MODE_SOL,
            RAW_DATA_12BIT,                     
            LVDS_ENDIAN_BIG,
            LVDS_ENDIAN_BIG, 
            .lane_id = {0, 2, -1, -1, 1, 3, -1, -1},
            .sync_code =  {
                  {{0x002, 0x003, 0x000, 0x001}, //PHY0_lane0
                  {0x202, 0x203, 0x200, 0x201},
                  {0x102, 0x103, 0x100, 0x101},
                  {0x302, 0x303, 0x300, 0x301}},

                  {{0x006, 0x007, 0x004, 0x005}, //PHY0_lane1
                  {0x206, 0x207, 0x204, 0x205},
                  {0x106, 0x107, 0x104, 0x105},
                  {0x306, 0x307, 0x304, 0x305}},

                  {{0x00a, 0x00b, 0x008, 0x009}, //PHY0_lane2
                  {0x20a, 0x20b, 0x208, 0x209},
                  {0x10a, 0x10b, 0x108, 0x109},
                  {0x30a, 0x30b, 0x308, 0x309}},

                  {{0x00a, 0x00b, 0x008, 0x009}, //PHY0_lane3  INPUT_MODE_LVDS
                  {0x20a, 0x20b, 0x208, 0x209},
                  {0x10a, 0x10b, 0x108, 0x109},
                  {0x30a, 0x30b, 0x308, 0x309}},

                  {{0x012, 0x013, 0x010, 0x011},//PHY1_lane0
                  {0x212, 0x213, 0x210, 0x211},
                  {0x112, 0x113, 0x110, 0x111},
                  {0x312, 0x313, 0x310, 0x311}},

                  {{0x016, 0x017, 0x014, 0x015}, //PHY1_lane1
                  {0x216, 0x217, 0x214, 0x215},
                  {0x116, 0x117, 0x114, 0x115},
                  {0x316, 0x317, 0x314, 0x315}},

                  {{0x01a, 0x01b, 0x018, 0x019}, //PHY1_lane2
                  {0x21a, 0x21b, 0x218, 0x219},
                  {0x11a, 0x11b, 0x118, 0x119},
                  {0x31a, 0x31b, 0x318, 0x319}},

                  {{0x01a, 0x01b, 0x018, 0x019}, //PHY1_lane3
                  {0x21a, 0x21b, 0x218, 0x219},
                  {0x11a, 0x11b, 0x118, 0x119},
                  {0x31a, 0x31b, 0x318, 0x319}}
               } 
        }
    }
};

combo_dev_attr_t SUBLVDS_4lane_SENSOR_MN34220_12BIT_720_NOWDR_ATTR =
{
    /* input mode */
    .input_mode = INPUT_MODE_SUBLVDS,
        
    {
        .lvds_attr = {
            .img_size = {1280, 720},
            HI_WDR_MODE_NONE,            
            LVDS_SYNC_MODE_SOL,
            RAW_DATA_12BIT,                     
            LVDS_ENDIAN_BIG,
            LVDS_ENDIAN_BIG, 
            .lane_id = {0, 2, -1, -1, 1, 3, -1, -1},
            .sync_code =  {
                  {{0x002, 0x003, 0x000, 0x001}, //PHY0_lane0
                  {0x202, 0x203, 0x200, 0x201},
                  {0x102, 0x103, 0x100, 0x101},
                  {0x302, 0x303, 0x300, 0x301}},

                  {{0x006, 0x007, 0x004, 0x005}, //PHY0_lane1
                  {0x206, 0x207, 0x204, 0x205},
                  {0x106, 0x107, 0x104, 0x105},
                  {0x306, 0x307, 0x304, 0x305}},

                  {{0x00a, 0x00b, 0x008, 0x009}, //PHY0_lane2
                  {0x20a, 0x20b, 0x208, 0x209},
                  {0x10a, 0x10b, 0x108, 0x109},
                  {0x30a, 0x30b, 0x308, 0x309}},

                  {{0x00a, 0x00b, 0x008, 0x009}, //PHY0_lane3  INPUT_MODE_LVDS
                  {0x20a, 0x20b, 0x208, 0x209},
                  {0x10a, 0x10b, 0x108, 0x109},
                  {0x30a, 0x30b, 0x308, 0x309}},

                  {{0x012, 0x013, 0x010, 0x011},//PHY1_lane0
                  {0x212, 0x213, 0x210, 0x211},
                  {0x112, 0x113, 0x110, 0x111},
                  {0x312, 0x313, 0x310, 0x311}},

                  {{0x016, 0x017, 0x014, 0x015}, //PHY1_lane1
                  {0x216, 0x217, 0x214, 0x215},
                  {0x116, 0x117, 0x114, 0x115},
                  {0x316, 0x317, 0x314, 0x315}},

                  {{0x01a, 0x01b, 0x018, 0x019}, //PHY1_lane2
                  {0x21a, 0x21b, 0x218, 0x219},
                  {0x11a, 0x11b, 0x118, 0x119},
                  {0x31a, 0x31b, 0x318, 0x319}},

                  {{0x01a, 0x01b, 0x018, 0x019}, //PHY1_lane3
                  {0x21a, 0x21b, 0x218, 0x219},
                  {0x11a, 0x11b, 0x118, 0x119},
                  {0x31a, 0x31b, 0x318, 0x319}}
               }
        }
    }
};

combo_dev_attr_t SUBLVDS_4lane_SENSOR_MN34220_12BIT_720_2WDR1_ATTR =
{
    /* input mode */
    .input_mode = INPUT_MODE_SUBLVDS,
        
    {
        .lvds_attr = {
            .img_size = {1280, 720},
            HI_WDR_MODE_2F,            
            LVDS_SYNC_MODE_SOL,
            RAW_DATA_12BIT,                     
            LVDS_ENDIAN_BIG,
            LVDS_ENDIAN_BIG, 
            .lane_id = {0, 2, -1, -1, 1, 3, -1, -1},
            .sync_code =  {
                  {{0x002, 0x003, 0x000, 0x001}, //PHY0_lane0
                  {0x202, 0x203, 0x200, 0x201},
                  {0x102, 0x103, 0x100, 0x101},
                  {0x302, 0x303, 0x300, 0x301}},

                  {{0x006, 0x007, 0x004, 0x005}, //PHY0_lane1
                  {0x206, 0x207, 0x204, 0x205},
                  {0x106, 0x107, 0x104, 0x105},
                  {0x306, 0x307, 0x304, 0x305}},

                  {{0x00a, 0x00b, 0x008, 0x009}, //PHY0_lane2
                  {0x20a, 0x20b, 0x208, 0x209},
                  {0x10a, 0x10b, 0x108, 0x109},
                  {0x30a, 0x30b, 0x308, 0x309}},

                  {{0x00a, 0x00b, 0x008, 0x009}, //PHY0_lane3  INPUT_MODE_LVDS
                  {0x20a, 0x20b, 0x208, 0x209},
                  {0x10a, 0x10b, 0x108, 0x109},
                  {0x30a, 0x30b, 0x308, 0x309}},

                  {{0x012, 0x013, 0x010, 0x011},//PHY1_lane0
                  {0x212, 0x213, 0x210, 0x211},
                  {0x112, 0x113, 0x110, 0x111},
                  {0x312, 0x313, 0x310, 0x311}},

                  {{0x016, 0x017, 0x014, 0x015}, //PHY1_lane1
                  {0x216, 0x217, 0x214, 0x215},
                  {0x116, 0x117, 0x114, 0x115},
                  {0x316, 0x317, 0x314, 0x315}},

                  {{0x01a, 0x01b, 0x018, 0x019}, //PHY1_lane2
                  {0x21a, 0x21b, 0x218, 0x219},
                  {0x11a, 0x11b, 0x118, 0x119},
                  {0x31a, 0x31b, 0x318, 0x319}},

                  {{0x01a, 0x01b, 0x018, 0x019}, //PHY1_lane3
                  {0x21a, 0x21b, 0x218, 0x219},
                  {0x11a, 0x11b, 0x118, 0x119},
                  {0x31a, 0x31b, 0x318, 0x319}}
               } 
        }
    }
};


combo_dev_attr_t LVDS_4lane_SENSOR_IMX178_12BIT_5M_NOWDR_ATTR =
{
    /* input mode */
    .input_mode = INPUT_MODE_LVDS,
    {
        .lvds_attr = {
            .img_size = {2592, 1944},
            HI_WDR_MODE_NONE,
            LVDS_SYNC_MODE_SAV,
            RAW_DATA_12BIT,
            LVDS_ENDIAN_BIG,
            LVDS_ENDIAN_BIG,
            .lane_id = {0, 1, 2, 3, -1, -1, -1, -1},
            .sync_code = { 
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},

                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                        
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},

                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}} 
                }
        }
    }
};

combo_dev_attr_t LVDS_4lane_SENSOR_IMX178_12BIT_1080p_ATTR =
{
    /* input mode */
    .input_mode = INPUT_MODE_LVDS,
    {
        .lvds_attr = {
            .img_size = {1920, 1080},
            HI_WDR_MODE_NONE,
            LVDS_SYNC_MODE_SAV,
            RAW_DATA_12BIT,
            LVDS_ENDIAN_BIG,
            LVDS_ENDIAN_BIG,
            .lane_id = {0, 1, 2, 3, -1, -1, -1, -1},
            .sync_code = { 
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},

                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                        
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},

                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}},
                    
                    {{0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}, 
                    {0xab0, 0xb60, 0x800, 0x9d0}} 
                }
        }
    }
};


combo_dev_attr_t MIPI_CMOS3V3_ATTR =
{
    /* input mode */
    .input_mode = INPUT_MODE_CMOS_33V,
    {
        
    }
};

combo_dev_attr_t MIPI_4lane_SENSOR_MN34220_MIPI_12BIT_ATTR = 
{
    .input_mode = INPUT_MODE_MIPI,  
    {
        .mipi_attr =    
        {
            RAW_DATA_12BIT,
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
    
};

combo_dev_attr_t MIPI_2lane_SENSOR_MN34222_12BIT_NOWDR_ATTR =
{
    .input_mode = INPUT_MODE_MIPI,
        
    .mipi_attr =    
    {
        RAW_DATA_12BIT,
        {0, 1, -1, -1, -1, -1, -1, -1}
    }
};

combo_dev_attr_t MIPI_2lane_SENSOR_OV9752_12BIT_NOWDR_ATTR = 
{
    .input_mode = INPUT_MODE_MIPI,
        
    .mipi_attr =    
    {
        RAW_DATA_12BIT,
        {0, 1, -1, -1, -1, -1, -1, -1}
    }
};

combo_dev_attr_t MIPI_4lane_SENSOR_OV2718_12BIT_NOWDR_ATTR = 
{
    .input_mode = INPUT_MODE_MIPI,
        
    .mipi_attr =    
    {
        RAW_DATA_12BIT,
        {0, 1, 2, 3, -1, -1, -1, -1}
    }
};


combo_dev_attr_t MIPI_BT1120_ATTR =
{
    /* input mode */
    .input_mode = INPUT_MODE_BT1120,
    {
        
    }
};


combo_dev_attr_t MIPI_4lane_SENSOR_OV4682_10BIT_ATTR = 
{
    .input_mode = INPUT_MODE_MIPI,  
    {
        .mipi_attr =    
        {
            RAW_DATA_10BIT,
            {0, 1, 2, 3, -1, -1, -1, -1}
        }
    }
    
};

// 这里面的配置要看sensor的数据手册，实际开发中不会看的，因为sensor厂商会事先提供好的
combo_dev_attr_t HISPI_4lane_SENSOR_AR0230_12BIT_ATTR = 
{
    /* input mode */
    .input_mode = INPUT_MODE_HISPI,
    {
        .lvds_attr = 
        {
            .img_size = {1920, 1080},
            HI_WDR_MODE_NONE,
            LVDS_SYNC_MODE_SOL,
            RAW_DATA_12BIT,
            LVDS_ENDIAN_LITTLE,
            LVDS_ENDIAN_LITTLE,
            .lane_id = {0, 1, 2, 3, -1, -1, -1, -1},
            // 下面的要看sensor的数据手册才能知道怎么回事，实际开发中sensor厂商会给你这些参数
            .sync_code = 
            { 
                {{0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}},
                
                {{0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}},

                {{0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}},
                
                {{0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}},
                
                {{0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}},
                    
                {{0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}},

                {{0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}},
                
                {{0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}, 
                {0x003, 0x007, 0x001, 0x005}} 
            }
        }
    }
};




VI_CHN_ATTR_S CHN_ATTR_1920x1080_422 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 1920,   1080},
    /* dest_w  dest_h */
    {1920,   1080 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
    /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};

VI_CHN_ATTR_S CHN_ATTR_860x540_422 =
{
    /*crop_x crop_y crop_w  crop_h*/  
    {0,     0, 860,   540}, 
    /* dest_w  dest_h */
    {860,   540 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
    /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};


VI_CHN_ATTR_S CHN_ATTR_1280x720_422 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 1280,   720 }, 
    /* dest_w  dest_h */
    {1280,   720 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
   /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};

VI_CHN_ATTR_S CHN_ATTR_640x360_422 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 640,   360}, 
    /* dest_w  dest_h */
    {640,   360 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
   /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};


VI_CHN_ATTR_S CHN_ATTR_1280x720_420 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 1280,   720}, 
    /* dest_w  dest_h */
    {1280,   720 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
    /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};


VI_CHN_ATTR_S CHN_ATTR_720x576_422 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 720,   576 }, 
    /* dest_w  dest_h */
    {720,   576 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
    /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};

VI_CHN_ATTR_S CHN_ATTR_720x576_420 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 720,   576}, 
    /* dest_w  dest_h */
    {720,   576},
    /*enCapSel*/
    VI_CAPSEL_BOTH,
    /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};

VI_CHN_ATTR_S CHN_ATTR_360x288_422 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 360,   288 }, 
    /* dest_w  dest_h */
    {360,   288 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
    /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};

VI_CHN_ATTR_S CHN_ATTR_360x288_420 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 360,   288}, 
    /* dest_w  dest_h */
    {360,   288},
    /*enCapSel*/
    VI_CAPSEL_BOTH,
     /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_420,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};

VI_CHN_ATTR_S CHN_ATTR_16x16_422 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 16,   16 }, 
    /* dest_w  dest_h */
    {16,   16 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
     /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};

VI_CHN_ATTR_S CHN_ATTR_960x576_422 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 960,   576 }, 
    /* dest_w  dest_h */
    {960,   576 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
    /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};

VI_CHN_ATTR_S CHN_ATTR_480x288_422 =
{
    /* crop_x crop_y crop_w  crop_h */  
    {0,     0, 480,   288}, 
    /* dest_w  dest_h */
    {480,   288 },
    /*enCapSel*/
    VI_CAPSEL_BOTH,
     /* channel  pixel format */
    PIXEL_FORMAT_YUV_SEMIPLANAR_422,
    /*bMirr  bFlip   bChromaResample*/
    0,      0,      0,
    /*s32SrcFrameRate   s32DstFrameRate*/
	-1, -1
};

VI_DEV g_as32ViDev[VIU_MAX_DEV_NUM];
VI_CHN g_as32MaxChn[VIU_MAX_CHN_NUM];
VI_CHN g_as32SubChn[VIU_MAX_CHN_NUM];
VB_POOL g_Pool = VB_INVALID_POOLID;
HI_S32 SAMPLE_TW2865_CfgV(VIDEO_NORM_E enVideoMode,VI_WORK_MODE_E enWorkMode)
{
#if 0
    int fd, i;
    int video_mode;
    tw2865_video_norm stVideoMode;
    tw2865_work_mode work_mode;

    int chip_cnt = 4;

    fd = open(TW2865_FILE, O_RDWR);
    if (fd < 0)
    {
        SAMPLE_PRT("open 2865 (%s) fail\n", TW2865_FILE);
        return -1;
    }

    video_mode = (VIDEO_ENCODING_MODE_PAL == enVideoMode) ? TW2865_PAL : TW2865_NTSC ;

    for (i=0; i<chip_cnt; i++)
    {
        stVideoMode.chip    = i;
        stVideoMode.mode    = video_mode;
        if (ioctl(fd, TW2865_SET_VIDEO_NORM, &stVideoMode))
        {
            SAMPLE_PRT("set tw2865(%d) video mode fail\n", i);
            close(fd);
            return -1;
        }
    }

    for (i=0; i<chip_cnt; i++)
    {
        work_mode.chip = i;
        if (VI_WORK_MODE_4Multiplex== enWorkMode)
        {
            work_mode.mode = TW2865_4D1_MODE;
        }
        else if (VI_WORK_MODE_2Multiplex== enWorkMode)
        {
            work_mode.mode = TW2865_2D1_MODE;
        }
        else if (VI_WORK_MODE_1Multiplex == enWorkMode)
        {
            work_mode.mode = TW2865_1D1_MODE;
        }
        else
        {
            SAMPLE_PRT("work mode not support\n");
            return -1;
        }
        ioctl(fd, TW2865_SET_WORK_MODE, &work_mode);
    }

    close(fd);
#endif
    return 0;
}

HI_S32 SAMPLE_COMM_VI_Mode2Param(SAMPLE_VI_MODE_E enViMode, SAMPLE_VI_PARAM_S *pstViParam)
{
    switch (enViMode)
    {
        default:
            pstViParam->s32ViDevCnt      = 1;
            pstViParam->s32ViDevInterval = 1;
            pstViParam->s32ViChnCnt      = 1;
            pstViParam->s32ViChnInterval = 1;
            break;
    }
    return HI_SUCCESS;
}

/*****************************************************************************
* function : get vi parameter, according to vi type
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_ADStart(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm)
{
    VI_WORK_MODE_E enWorkMode;
    HI_S32 s32Ret;
    
    switch (enViMode)
    {
        case SAMPLE_VI_MODE_1_D1:
            enWorkMode = VI_WORK_MODE_1Multiplex;
            s32Ret = SAMPLE_TW2865_CfgV(enNorm, enWorkMode);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("SAMPLE_TW2865_CfgV failed with %#x!\n",\
                        s32Ret);
                return HI_FAILURE;
            }
            break;

        /* use 7601 without drv */
        case SAMPLE_VI_MODE_BT1120_720P:
        case SAMPLE_VI_MODE_BT1120_1080P:
            break;

        default:
            SAMPLE_PRT("AD not support!\n");
            return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}



/*****************************************************************************
* function : get vi parameter, according to vi type
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_Mode2Size(SAMPLE_VI_MODE_E enViMode, VIDEO_NORM_E enNorm, SIZE_S *pstSize)
{
    switch (enViMode)
    {
        case SAMPLE_VI_MODE_1_D1:
            pstSize->u32Width = 720;
            pstSize->u32Height = (VIDEO_ENCODING_MODE_PAL==enNorm)?576:480;
            break;

        case SAMPLE_VI_MODE_BT1120_1080P:
            pstSize->u32Width = 1920;
            pstSize->u32Height = 1080;
            break;
            
        case SAMPLE_VI_MODE_BT1120_720P:
            pstSize->u32Width = 1280;
            pstSize->u32Height = 720;
            break;
            
        default:
            SAMPLE_PRT("vi mode invaild!\n");
            return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}

/*****************************************************************************
* function : Get Vi Dev No. according to Vi_Chn No.
*****************************************************************************/
VI_DEV SAMPLE_COMM_VI_GetDev(SAMPLE_VI_MODE_E enViMode, VI_CHN ViChn)
{
    HI_S32 s32Ret, s32ChnPerDev;
    SAMPLE_VI_PARAM_S stViParam;

    s32Ret = SAMPLE_COMM_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("vi get param failed!\n");
        return (VI_DEV)-1;
    }

    s32ChnPerDev = stViParam.s32ViChnCnt / stViParam.s32ViDevCnt;
    return (VI_DEV)(ViChn /stViParam.s32ViChnInterval / s32ChnPerDev * stViParam.s32ViDevInterval);
}

/******************************************************************************
* function : Set vi system memory location
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_MemConfig(SAMPLE_VI_MODE_E enViMode)
{
    HI_CHAR * pcMmzName;
    MPP_CHN_S stMppChnVI;
    SAMPLE_VI_PARAM_S stViParam;
    VI_DEV ViDev;
    VI_CHN ViChn;

    HI_S32 i, s32Ret;

    s32Ret = SAMPLE_COMM_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("vi get param failed!\n");
        return HI_FAILURE;
    }

    for(i=0; i<stViParam.s32ViChnCnt; i++)
    {
        ViChn = i * stViParam.s32ViChnInterval;
        ViDev = SAMPLE_COMM_VI_GetDev(enViMode, ViChn);
        //printf("dev:%d, chn:%d\n", ViDev, ViChn);
        if (ViDev < 0)
        {
            SAMPLE_PRT("get vi dev failed !\n");
            return HI_FAILURE;
        }

        pcMmzName = (0==i%2)?NULL: "ddr1";
        stMppChnVI.enModId = HI_ID_VIU;
        stMppChnVI.s32DevId = 0; //For VIU mode, this item must be set to zero
        stMppChnVI.s32ChnId = ViChn;
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVI,pcMmzName);
        if (s32Ret)
        {
            SAMPLE_PRT("VI HI_MPI_SYS_SetMemConf failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S* pstViConfig);

/*****************************************************************************
* function : init mipi
* 这个函数作用其实就是在应用层去操作sensor的驱动，对sensor属性的必要初始化
启动VI的前传部分
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_StartMIPI(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    int s32Ret;
    
    /* SAMPLE_COMM_VI_SetMipiAttr */
	// SAMPLE_COMM_VI_SetMipiAttr函数是设置sensor的一些属性
	// 主要的传参就是enViMode，函数中匹配enViMode，然后设置相应的属性，最后将属性写到相应的sensor中
    s32Ret = SAMPLE_COMM_VI_SetMipiAttr(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
      SAMPLE_PRT("%s: mipi init failed!\n", __FUNCTION__);
      /* disable videv */
      return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}

/*****************************************************************************
* function : star vi dev (cfg vi_dev_attr; set_dev_cfg; enable dev)
设置VI属性，WDR属性，最后启动VI部分；
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_StartDev(VI_DEV ViDev, SAMPLE_VI_MODE_E enViMode)
{
    HI_S32 s32Ret;
    HI_S32 s32IspDev = 0;
    ISP_WDR_MODE_S stWdrMode;
    VI_DEV_ATTR_S  stViDevAttr;		// 填充设备的属性
    
    memset(&stViDevAttr,0,sizeof(stViDevAttr));

	// switch语句里面是对stViDevAttr结构里面的变量进行填充
    switch (enViMode)
    {
        case SAMPLE_VI_MODE_1_D1:
            memcpy(&stViDevAttr,&DEV_ATTR_BT656D1_1MUX,sizeof(stViDevAttr));
            break;
			
        case SAMPLE_VI_MODE_BT1120_1080P:           
            memcpy(&stViDevAttr,&DEV_ATTR_BT1120_1080P_BASE,sizeof(stViDevAttr));   
            break;
            
        case SAMPLE_VI_MODE_BT1120_720P:
            memcpy(&stViDevAttr,&DEV_ATTR_BT1120_720P_BASE,sizeof(stViDevAttr));        
            break;

			
		case SONY_IMX222_DC_1080P_30FPS:
			memcpy(&stViDevAttr,&DEV_ATTR_IMX222_DC_1080P_BASE,sizeof(stViDevAttr));
			break;
            
        case SONY_IMX222_DC_720P_30FPS:
			memcpy(&stViDevAttr,&DEV_ATTR_IMX222_DC_720P_BASE,sizeof(stViDevAttr));
			break;

        case APTINA_9M034_DC_720P_30FPS:
            memcpy(&stViDevAttr,&DEV_ATTR_9M034_DC_720P_BASE,sizeof(stViDevAttr));
			break;
            
        case APTINA_AR0130_DC_720P_30FPS:
            memcpy(&stViDevAttr,&DEV_ATTR_9M034_DC_720P_BASE,sizeof(stViDevAttr));
			break;
       
        case APTINA_AR0230_HISPI_1080P_30FPS:
            memcpy(&stViDevAttr,&DEV_ATTR_HISPI_BASE,sizeof(stViDevAttr));
            stViDevAttr.stDevRect.s32X = 0;
            stViDevAttr.stDevRect.s32Y = 0;
            stViDevAttr.stDevRect.u32Width  = 1920;
            stViDevAttr.stDevRect.u32Height = 1080;
            break;
        case PANASONIC_MN34222_MIPI_1080P_30FPS:
            memcpy(&stViDevAttr,&DEV_ATTR_LVDS_BASE,sizeof(stViDevAttr));
            break;

        case OMNIVISION_OV9712_DC_720P_30FPS:
        case OMNIVISION_OV9732_DC_720P_30FPS:
            memcpy(&stViDevAttr,&DEV_ATTR_OV9732_DC_720P_BASE,sizeof(stViDevAttr));
            stViDevAttr.stDevRect.s32X = 0;
            stViDevAttr.stDevRect.s32Y = 0;
            stViDevAttr.stDevRect.u32Width  = 1280;
            stViDevAttr.stDevRect.u32Height = 720;
            break;

        case OMNIVISION_OV9750_MIPI_720P_30FPS:
        case OMNIVISION_OV9752_MIPI_720P_30FPS:
            memcpy(&stViDevAttr,&DEV_ATTR_MIPI_BASE,sizeof(stViDevAttr));
            stViDevAttr.stDevRect.s32X = 0;
            stViDevAttr.stDevRect.s32Y = 0;
            stViDevAttr.stDevRect.u32Width  = 1280;
            stViDevAttr.stDevRect.u32Height = 720;
            break;

        case OMNIVISION_OV2718_MIPI_1080P_25FPS:
            memcpy(&stViDevAttr,&DEV_ATTR_MIPI_BASE,sizeof(stViDevAttr));
            stViDevAttr.stDevRect.s32X = 0;
            stViDevAttr.stDevRect.s32Y = 0;
            stViDevAttr.stDevRect.u32Width  = 1920;
            stViDevAttr.stDevRect.u32Height = 1080;
            break;
			
        default:
            memcpy(&stViDevAttr,&DEV_ATTR_LVDS_BASE,sizeof(stViDevAttr));
    }

	/*********************************************************************************************
	函数功能:设备VI设备属性。基本设备属性默认了部分芯片配置，满足绝大部分的sensor对接要求，对于特殊
			 的时序，可使用高级设备属性接口HI_MPI_VI_SetDevAttrEx，配置VI设备的所有属性；
	函数传参:ViDev(VI设备号，取值范围:[0, VIU_MAX_DEV_NUM]，输入型参数)；
			 stViDevAttr(VI设备属性指针、静态属性，输入型参数)
	返回值:  0表示成功；非0表示失败；
	需求:	 头文件(hi_comm_vi.h、mpi_vi.h)、库文件(libmpi.a)
	注意:	 (1)不支持更改设备与通道的绑定关系，只支持一路复合工作模式，不支持BT1120隔行输入；
			 (2)在调用前保证VI设备处于禁用状态。如果VI设备已处于使能状态，可以使用HI_MPI_VI_DisableDev 
			 	来禁用设备
			 (3)参数stViDevAttr主要用来配置指定VI设备的视频接口模式，用于与外围camera、sensor或codec
			 	对接，支持的接口模式包括VT656、BT.601、digital camera、MIPI Rx(MIPI/LVDS/HISPI)、BT.1120
			 	逐行以及BT.1120隔行模式。用户需要配置以下几类信息，具体属性意见参见3.5"数据类型"部分的说明；
			 	接口模式信息: 接口模式为BT.656、BT.601、digital camera、BT.1120、MIPI Rx(MIPI/LVDS/HISPI)等模式；
			 	工作模式:	  1路、2路、4路复合模式；
			 	数据布局信息: 复合模式下多路数据的排布；
			 	数据信息: 	  隔行或逐行输入、YUV数据输入顺序；
			 	同步时序信息: 垂直、水平同步信号的属性；
	*********************************************************************************************/
    s32Ret = HI_MPI_VI_SetDevAttr(ViDev, &stViDevAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_SetDevAttr failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    if ( (SAMPLE_VI_MODE_BT1120_1080P != enViMode)
		&&(SAMPLE_VI_MODE_BT1120_720P != enViMode) )
	{
		/****************************************************************************************
		函数功能:获取ISP宽动态模式
		函数传参:IspDev(ISP设备号，输入型参数)、stWdrMode(获取宽动态模式、输出型参数)
		返回值:0表示成功、非0表示失败
		需求: 头文件(hi_comm_isp.h、mpi_isp.h)、库文件(libisp.a)
		****************************************************************************************/
	    s32Ret = HI_MPI_ISP_GetWDRMode(s32IspDev, &stWdrMode);
	    if (s32Ret != HI_SUCCESS)
	    {
	        SAMPLE_PRT("HI_MPI_ISP_GetWDRMode failed with %#x!\n", s32Ret);
	        return HI_FAILURE;
	    }

        VI_WDR_ATTR_S stWdrAttr;
        stWdrAttr.enWDRMode = stWdrMode.enWDRMode;
        stWdrAttr.bCompress = HI_FALSE;

		/*****************************************************************************************
		函数功能: 设置WDR工作属性；
		函数传参: ViDev(VI设备号、取值范围： [0, VIU_MAX_DEV_NUM]、输入型参数)
				  stWdrAttr(WDR参数属性指针、输入型参数)
		返回值: 0表示成功，非0表示失败；
		芯片差异: Hi3518EV200，支持的WDR模式(WDR_MODE_NONE)
		需求: 头文件(hi_comm_vi.h、mpi_vi.h)、库文件(libmpi.a)
		注意: (1)这个接口必须要在 HI_MPI_VI_SetDevAttr 之后、 HI_MPI_VI_EnableDev 之前设置，因为
				 WDR的相关信息要在从DEV的相关属性中获取；另一方面，如果重新设置了 DEV ATTR 后，
				 需要再调用此接口设置 WDR 的相关属性
			  (2)Hi3516A 上，如果 DevAttr 中配置了 stDevRect. s32Y 为非 0，则不能开启 WDR压缩。
			  (3)如果在相同的 WDR 模式之间切换时，请上层应用程序不要重新设置 MIPI 接口，
  				 否则会导致采集不到图像。在相同的 WDR 模式之间进行切换时，建议上层应用
  				 程序判断当前的 WDR 模式与要切换的 WDR 模式是否相同，如果相同，则可以直
  				 接退出即可，不用进行切换。
		*****************************************************************************************/
        s32Ret = HI_MPI_VI_SetWDRAttr(ViDev, &stWdrAttr);
        if (s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VI_SetWDRAttr failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
	}

	/**********************************************************************************************
	函数功能: 启用VI设备；
	函数传参: ViDev(VI设备号、取值范围： [0, VIU_MAX_DEV_NUM]、输入型参数)；
	返回值: 0表示成功，非0表示失败；
	需求: 头文件(hi_comm_vi.h、mpi_vi.h)、库文件(libmpi.a)
	注意: (1)不支持更改设备与通道的绑定关系；
		  (2)启用前必须已经设置设备属性，否则返回失败；
		  (3)可重复启用，不返回失败；
	**********************************************************************************************/
    s32Ret = HI_MPI_VI_EnableDev(ViDev);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VI_EnableDev failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* function : star vi chn
设置通道属性，图像旋转属性，启动VI通道
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_StartChn(VI_CHN ViChn, RECT_S *pstCapRect, SIZE_S *pstTarSize, SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 s32Ret;
    VI_CHN_ATTR_S stChnAttr;
    ROTATE_E enRotate = ROTATE_NONE;
    SAMPLE_VI_CHN_SET_E enViChnSet = VI_CHN_SET_NORMAL;

    if(pstViConfig)
    {
        enViChnSet = pstViConfig->enViChnSet;
        enRotate = pstViConfig->enRotate;
    }

    /* step  5: config & start vicap dev */
    memcpy(&stChnAttr.stCapRect, pstCapRect, sizeof(RECT_S));
    stChnAttr.enCapSel = VI_CAPSEL_BOTH;
    /* to show scale. this is a sample only, we want to show dist_size = D1 only */
    stChnAttr.stDestSize.u32Width = pstTarSize->u32Width;
    stChnAttr.stDestSize.u32Height = pstTarSize->u32Height;
	// VI通道输出的图像格式就是YUV420SP了
    stChnAttr.enPixFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;   /* sp420 or sp422 */

    stChnAttr.bMirror = HI_FALSE;
    stChnAttr.bFlip = HI_FALSE;

    switch(enViChnSet)
    {
        case VI_CHN_SET_MIRROR:
            stChnAttr.bMirror = HI_TRUE;
            break;

        case VI_CHN_SET_FLIP:
            stChnAttr.bFlip = HI_TRUE;
            break;
            
        case VI_CHN_SET_FLIP_MIRROR:
            stChnAttr.bMirror = HI_TRUE;
            stChnAttr.bFlip = HI_TRUE;
            break;
            
        default:
            break;
    }

    stChnAttr.s32SrcFrameRate = -1;
    stChnAttr.s32DstFrameRate = -1;
	// VI通道中的视频流是非压缩的
    stChnAttr.enCompressMode = COMPRESS_MODE_NONE;

	/*********************************************************************************************
	函数功能: 设置VI通道属性；
	函数传参: ViChn(VI通道号，取值范围： [0, VIU_MAX_PHYCHN_NUM]，输入型参数)、
			  stChnAttr(VI通道属性指针，输入型参数)；
	返回值: 0表示成功；非0表示失败；
	需求: 头文件(hi_comm_vi.h、mpi_vi.h)、库文件(libmpi.a)
	注意: 通道属性的各项配置限制如下:
		(1)采集区域stCapRect:
			- 针对原始图像进行裁剪，stCapRect的宽和高为静态属性，其他项为动态属性；
			- 采集区域起始坐标用于配置需要采集的矩形图像相对于原始图像起始点的位置。
			  起始点位置的横坐标以像素为单位，纵坐标以行为单位。
			- stCapRect中s32X和u32Width必须2对齐；s32Y的u32Height逐行采集时必须2对齐；
			  s32Y和u32Height隔行采集时必须4对齐；s32X和s32Y不能小于0，s32X+u32Width不能大于设备
			  属性中stDevRect的宽，s32Y+u32Height不能大于设备属性中stDevRect的高。
			- Hi3518EV200开启FPN标定或者校正时，stCapRect必须和设备属性中的stevRect配置相同。
		(2)目标图像大小 stDestSize:
			- 必须配置，且大小不应该超出外围ADC输出图像的大小范围，否则可能导致VI硬件工作异常。
			- 当输入图像为逐行时 stDestSize的宽和高必须等于stCapRect的宽与高。
			- 当输入图像为隔行时，根据采单场或两场，将采集高度分别配置为单场或两场的高度，并要求
			  两场采集时高度是4的整数倍。
		(3)抽场选择enCapSel:
			- 抽场选择用于在原始图像为隔行输入时，用户可选择仅捕获其中的一场。
			- 为防止图像上下抖动现象，建议捕获单场时选择捕获底场。
		(4)像素格式enPixFormat: Rotate处理时，输出像素格式只支持sp420。
			PIXEL_FORMAT_RGB_BAYER 表示通道写出的是 16bit raw 数据， LDC 或者Rotate 使能时不支持 
			PIXEL_FORMAT_RGB_BAYER。
		(5)原始帧率 s32SrcFrameRate：如果不进行帧率控制，则该值设置为-1。
		(6)目标帧率 s32DstFrameRate：如果不进行帧率控制，则该值设置为-1，否则必须先设置原始帧率 
			s32SrcFrameRate 的值，且目标帧率必须小于或等于原始帧率。
		(7)抓拍机方案中，建议不要使用帧率控制，否则会造成闪光信号触发的延时，不能及时响应抓拍。
		(8)压缩模式： LDC 处理时，输入输出只支持非压缩。鱼眼处理时，输入只支持非压缩。 DIS 处理时，
			输入只支持非压缩。
		(9)隔行图像不支持垂直翻转功能（因为开启该功能后顶底场采集顺序与原来的相反，后续模块处理可能
			会引起图像质量问题）， MPP 内部不做此检查，由用户保证。
		(10)必须先设置设备属性才能设置通道属性，否则会返回失败。
	*********************************************************************************************/
	// 主要进行图像大小，图像翻转，采样帧率，像素格式，重采样进行配置
	s32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    if(ROTATE_NONE != enRotate)
    {
		/*****************************************************************************************
		函数功能: 设置VI图像旋转属性；
		函数传参: ViChn(VI物理通道号，取值范围： [0, VIU_MAX_PHYCHN_NUM]，输入型参数)、
				  enRotate(旋转属性、输入型参数)
		返回值: 0表示成功；非0表示失败；
		需求: 头文件(hi_comm_vi.h、mpi_vi.h)、库文件(libmpi.a)
		注意: (1)只支持离线模式；
			  (2)Rotate是静态属性，不能动态修改，且必须在设置通道属性后，在Enable通道前设置此属性；
			  (3)仅支持非场输出的sp420图像的旋转；
			  (4)仅支持90/180/270的旋转，不支持任意角度旋转；
			  (5)仅支持物理通道的Rotate；
			  (6)设置旋转后，物理通道输出的宽高可能会发生变化，但获取的通道属性中的通道宽高仍为用户
			  	 设置值。比如720P输入的图像，在旋转90/270时，实际输出的图像大小为(720,1280)，但
			  	 HI_MPI_VI_GetChnAttr 获取的 DestSize 宽高仍为(1280,720)。
			  (7)设置旋转后，用户图片要根据转后图像大小进行设置；
			  (8)设置Rotate时，不支持压缩；
			  (9)Hi3519V100当物理通道的宽度超过4096时，不支持设置旋转。
		*****************************************************************************************/
        s32Ret = HI_MPI_VI_SetRotate(ViChn, enRotate);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VI_SetRotate failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

	/**********************************************************************************************
	函数功能: 启用VI通道；
	函数传参: ViChn(VI物理通道号，取值范围： [0, VIU_MAX_PHYCHN_NUM]，输入型参数)
	返回值: 0表示成功；非0表示失败；
	需求: 头文件(hi_comm_vi.h、mpi_vi.h)、库文件(libmpi.a)
	注意: (1)必须先设置通道属性，且通道所绑定的VI设备必须使能；
		  (2)如果设置了后端模块与VI的绑定，在成功调用该接口后，后端模块就会得到视频数据；
		  (3)可重复启用VI通道，不返回失败。
	**********************************************************************************************/
    s32Ret = HI_MPI_VI_EnableChn(ViChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* function : star vi according to product type
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_StartBT656(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 i, s32Ret = HI_SUCCESS;
    VI_DEV ViDev;
    VI_CHN ViChn;
    HI_U32 u32DevNum = 1;
    HI_U32 u32ChnNum = 1;
    SIZE_S stTargetSize;
    RECT_S stCapRect;
    SAMPLE_VI_MODE_E enViMode;

    if(!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }
    enViMode = pstViConfig->enViMode;

	/******************************************
	 step 1: mipi configure
	******************************************/
	s32Ret = SAMPLE_COMM_VI_StartMIPI_BT1120(enViMode);   
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("%s: MIPI init failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }     
	
    for (i = 0; i < u32DevNum; i++)
    {
        ViDev = i;
        s32Ret = SAMPLE_COMM_VI_StartDev(ViDev, enViMode);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("%s: start vi dev[%d] failed!\n", __FUNCTION__, i);
            return HI_FAILURE;
        }
    }
    
    /******************************************************
    * Step 2: config & start vicap chn (max 1) 
    ******************************************************/
    for (i = 0; i < u32ChnNum; i++)
    {
        ViChn = i;

        stCapRect.s32X = 0;
        stCapRect.s32Y = 0;
        switch (enViMode)
        {
            case SAMPLE_VI_MODE_BT1120_720P:           
                stCapRect.u32Width = 1280;
                stCapRect.u32Height = 720;
                break;
                
           case SAMPLE_VI_MODE_BT1120_1080P:           
                stCapRect.u32Width  = 1920;
                stCapRect.u32Height = 1080;
                break; 
                
            default:
                stCapRect.u32Width  = 1920;
                stCapRect.u32Height = 1080;
                break;
        }

        stTargetSize.u32Width = stCapRect.u32Width;
        stTargetSize.u32Height = stCapRect.u32Height;

        s32Ret = SAMPLE_COMM_VI_StartChn(ViChn, &stCapRect, &stTargetSize, pstViConfig);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_COMM_ISP_Stop();
            return HI_FAILURE;
        }
    }

    return s32Ret;
}

/*****************************************************************************
* function : stop vi accroding to product type
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_StopBT656(SAMPLE_VI_MODE_E enViMode)
{
    VI_DEV ViDev;
    VI_CHN ViChn;
    HI_S32 i;
    HI_S32 s32Ret;
    SAMPLE_VI_PARAM_S stViParam;

    /*** get parameter from Sample_Vi_Mode ***/
    s32Ret = SAMPLE_COMM_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return HI_FAILURE;
    }

    /*** Stop VI Chn ***/
    for(i=0;i<stViParam.s32ViChnCnt;i++)
    {
        /* Stop vi phy-chn */
        ViChn = i * stViParam.s32ViChnInterval;
        s32Ret = HI_MPI_VI_DisableChn(ViChn);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StopChn failed with %#x\n",s32Ret);
            return HI_FAILURE;
        }
    }

    /*** Stop VI Dev ***/
    for(i=0; i<stViParam.s32ViDevCnt; i++)
    {
        ViDev = i * stViParam.s32ViDevInterval;
        s32Ret = HI_MPI_VI_DisableDev(ViDev);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("SAMPLE_COMM_VI_StopDev failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

/*****************************************************************************
* function : Vi chn bind vpss group
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_BindVpss(SAMPLE_VI_MODE_E enViMode)
{
    HI_S32 j, s32Ret;
    VPSS_GRP VpssGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    SAMPLE_VI_PARAM_S stViParam;
    VI_CHN ViChn;

	// 获取VI中的参数，譬如设备号，通道号，方便下面与VPSS对接时使用；
	// 获取VI中有几个chn通道，好为以后和VPSS绑定做准备；
    s32Ret = SAMPLE_COMM_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return HI_FAILURE;
    }
    
    VpssGrp = 0;
    for (j=0; j<stViParam.s32ViChnCnt; j++)
    {
        ViChn = j * stViParam.s32ViChnInterval;		// ViChn=0

		// 源是VI单元的channel0，模块是HI_ID_VIU，设备号是0
        stSrcChn.enModId  = HI_ID_VIU;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = ViChn;

		// 目的是VPASS的channel0，模块是HI_ID_VPSS，设备号是VpssGrp
        stDestChn.enModId  = HI_ID_VPSS;
        stDestChn.s32DevId = VpssGrp;
        stDestChn.s32ChnId = 0;

		/*****************************************************************************************
		函数功能: 数据源到数据接收者绑定接口；在线模式绑定，让VI和VPSS在线绑定
		函数传参: stSrcChn(源通道指针，输出型参数)、stDestChn(目的通道指针，输出型参数)
		返回值: 0表示成功，非0表示失败；
		需求: 头文件(hi_comm_sys.h、mpi_sys.h)、库文件(libmpi.a)
		注意: (1)系统目前支持的绑定关系，请参见表2-1；
			  (2)同一个数据接收者只能绑定一个数据源；
			  (3)绑定是指数据源和数据接收者建立关联关系。绑定后，数据源生成的数据将自动发送给接收者。
			  (4)VI和VENC作为数据源，是以通道为发送者，向其他模块发送数据，用户将设备号置为0，SDK不
			  	 检查输入的设备号。
			  (5)VO作为数据源发送回写(WBC)数据时，是以设备为发送者，向其他模块发送数据，用户将通道
			  	 号置为0，SDK不检查输入的通道号。
			  (6)VPSS作为数据接收者时，是以设备(GROUP)为接收者，接收其他模块发来的数据，用户将通道号
			     置为0，SDK不检查输入的通道号。
			  (7)VENC作为数据接收者时，是以通道号为接收者，接收其他模块发过来的数据，用户将设备号置为
			     0，SDK不检查输入的设备号。
			  (8)其他情况均需指定设备号和通道号。
		*****************************************************************************************/
		// VI和VPSS绑定，说明是在线模式，如果不绑定，则说明是离线模式；
		s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
        
        VpssGrp ++;
    }
    return HI_SUCCESS;
}

/******************************************************
* 函数作用: 判断图像是否是通过sensor得到的
*******************************************************/
HI_BOOL IsSensorInput(SAMPLE_VI_MODE_E enViMode)
{
    HI_BOOL bRet = HI_TRUE;

    switch(enViMode)
    {
        case SAMPLE_VI_MODE_1_D1:
        case SAMPLE_VI_MODE_BT1120_1080P:
        case SAMPLE_VI_MODE_BT1120_720P:		// 传统模拟电信号有关的，无与现在的sensor是不一样的
            bRet = HI_FALSE;
            break;
        default:
            break;
    }

    return bRet;    
}


HI_S32 SAMPLE_COMM_VI_BindVenc(SAMPLE_VI_MODE_E enViMode)
{
    HI_S32 j, s32Ret;
    VENC_GRP  VencGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    SAMPLE_VI_PARAM_S stViParam;
    VI_CHN ViChn;

    s32Ret = SAMPLE_COMM_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return HI_FAILURE;
    }
    
    VencGrp = 0;
    for (j=0; j<stViParam.s32ViChnCnt; j++)
    {
        ViChn = j * stViParam.s32ViChnInterval;
        
        stSrcChn.enModId = HI_ID_VIU;
        stSrcChn.s32DevId = 0;
        stSrcChn.s32ChnId = ViChn;
    
        stDestChn.enModId = HI_ID_GROUP;
        stDestChn.s32DevId = VencGrp;
        stDestChn.s32ChnId = 0;
    
        s32Ret = HI_MPI_SYS_Bind(&stSrcChn, &stDestChn);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
        
        VencGrp ++;
    }
    
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_StartMIPI_BT1120(SAMPLE_VI_MODE_E enViMode)
{
	HI_S32 fd;
	combo_dev_attr_t *pstcomboDevAttr = NULL;
	
	fd = open("/dev/hi_mipi", O_RDWR);
	if (fd < 0)
	{
	   printf("warning: open hi_mipi dev failed\n");
	   return -1;
	}

	if( (enViMode == SAMPLE_VI_MODE_BT1120_720P)
		||(enViMode == SAMPLE_VI_MODE_BT1120_1080P) )
	{
		pstcomboDevAttr = &MIPI_BT1120_ATTR;
	}
	else
	{
	}

    if (NULL == pstcomboDevAttr)
    {
        printf("Func %s() Line[%d], unsupported enViMode: %d\n", __FUNCTION__, __LINE__, enViMode);
        close(fd);
        return HI_FAILURE;   
    }
	
	if (ioctl(fd, HI_MIPI_SET_DEV_ATTR, pstcomboDevAttr))
	{
		printf("set mipi attr failed\n");
		close(fd);
		return HI_FAILURE;
	}
	close(fd);
	return HI_SUCCESS;
}

// 这个函数与sensor的驱动相关联
// 这个函数作用就是: 打开sensor设备文件，对sensor属性进行填充，最后将属性写到sensor设备操作文件中
HI_S32 SAMPLE_COMM_VI_SetMipiAttr(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 fd;
    combo_dev_attr_t *pstcomboDevAttr = NULL;

    /* mipi reset unrest */
	// 打开sensor设备文件
    fd = open("/dev/hi_mipi", O_RDWR);
    if (fd < 0)
    {
        printf("warning: open hi_mipi dev failed\n");
        return -1;
    }
	printf("=============SAMPLE_COMM_VI_SetMipiAttr enWDRMode: %d\n", pstViConfig->enWDRMode);

	// 对sensor进行填充
    if ( pstViConfig->enViMode == APTINA_AR0230_HISPI_1080P_30FPS )
    {
		// 对sensor为AR0230的属性进行设置
        pstcomboDevAttr = &HISPI_4lane_SENSOR_AR0230_12BIT_ATTR;
    }

    if ( pstViConfig->enViMode == PANASONIC_MN34222_MIPI_1080P_30FPS )
    {
        pstcomboDevAttr = &MIPI_2lane_SENSOR_MN34222_12BIT_NOWDR_ATTR;
    }

    if ( (pstViConfig->enViMode == OMNIVISION_OV9752_MIPI_720P_30FPS)
        || (pstViConfig->enViMode == OMNIVISION_OV9750_MIPI_720P_30FPS) )
    {
        pstcomboDevAttr = &MIPI_2lane_SENSOR_OV9752_12BIT_NOWDR_ATTR;
    }

    if ( pstViConfig->enViMode ==  OMNIVISION_OV2718_MIPI_1080P_25FPS )
    {
        pstcomboDevAttr = &MIPI_4lane_SENSOR_OV2718_12BIT_NOWDR_ATTR;
    }

// 我们用的sensor在这里APTINA_AR0130_DC_720P_30FPS、OMNIVISION_OV9712_DC_720P_30FPS
	if ( (pstViConfig->enViMode == APTINA_9M034_DC_720P_30FPS)
		|| (pstViConfig->enViMode == APTINA_AR0130_DC_720P_30FPS)
		|| (pstViConfig->enViMode == SONY_IMX222_DC_1080P_30FPS)
        || (pstViConfig->enViMode == SONY_IMX222_DC_720P_30FPS)
        || (pstViConfig->enViMode == OMNIVISION_OV9712_DC_720P_30FPS)
        || (pstViConfig->enViMode == OMNIVISION_OV9732_DC_720P_30FPS) )
    {
		// sensor使用的是并口连接，所以这里是空的；
		// 如果使用的MIPI连接，那么这里就需要有数据了；
        pstcomboDevAttr = &MIPI_CMOS3V3_ATTR;
    }

    if (NULL == pstcomboDevAttr)
    {
        printf("Func %s() Line[%d], unsupported enViMode: %d\n", __FUNCTION__, __LINE__, pstViConfig->enViMode);
        close(fd);
        return HI_FAILURE;   
    }

	// 填充完了调用ioctl函数(驱动对应用开放出来的接口)
	// 属性的传参标准是pstcomboDevAttr，都是别人事先定义好的结构体
	// pstcomboDevAttr结构体对于不同的sensor是不一样的
	// 将填充好的sensor属性信息(pstcomboDevAttr)写到对应的sensor属性设置(HI_MIPI_SET_DEV_ATTR)中
    if (ioctl(fd, HI_MIPI_SET_DEV_ATTR, pstcomboDevAttr))
    {
        printf("set mipi attr failed\n");
        close(fd);
        return HI_FAILURE;
    }
    close(fd);
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_StartIspAndVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 i, s32Ret = HI_SUCCESS;
    VI_DEV ViDev;
    VI_CHN ViChn;
    HI_U32 u32DevNum = 1;
    HI_U32 u32ChnNum = 1;
    SIZE_S stTargetSize;
    RECT_S stCapRect;
    SAMPLE_VI_MODE_E enViMode;

    if(!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }
    enViMode = pstViConfig->enViMode;

    /******************************************
     step 1: mipi configure
     第一步是对sensor的操作，启动sensor，实际上我们用的接口并不是MIPI的，而是并口的；
     先匹配sensor，然后将属性设置到相应的sensor中；
     实际传参是enViMode
    ******************************************/
    s32Ret = SAMPLE_COMM_VI_StartMIPI(pstViConfig);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("%s: MIPI init failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    /******************************************
     step 2: configure sensor and ISP (include WDR mode).
     note: you can jump over this step, if you do not use Hi3516A interal isp. 
     用来操作ISP的，将ISP进行初始化；
     ISP也是HI3518E内部的一个硬件单元，ISP硬件单元就是用来做ISP运算的，这一块在
     MPP中封装成了一些API；
     ISP主要是处理图像质量的，譬如3A，sensor公共属性，ISP外部寄存器等
     "传参是宽动态的模式"
    ******************************************/
    s32Ret = SAMPLE_COMM_ISP_Init(pstViConfig->enWDRMode);
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("%s: Sensor init failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }

    /******************************************
     step 3: run isp thread 
     note: you can jump over this step, if you do not use Hi3516A interal isp.
     创建一个线程，让事先写好的ISP在线程中运行起来；在这里可以使用PQ Tools进行调优；
    ******************************************/
    s32Ret = SAMPLE_COMM_ISP_Run();
    if (HI_SUCCESS != s32Ret)
    {
        SAMPLE_PRT("%s: ISP init failed!\n", __FUNCTION__);
	    /* disable videv */
        return HI_FAILURE;
    }
    
    /******************************************************
    HI3518E内部的ISP单元是隶属于VI模块的。VI模块就包含三大部分
    第一就是和sensor对接的部分；第二部分就是ISP；第三部分就是
    VI dev(用来采集图像的一个硬件单元)和channel(通道，图像分为好多分支进行裁剪等处理)
     step 4 : config & start vicap dev
    ******************************************************/
    // u32DevNum为1，表示我们只有一个VI输入设备模块
    for (i = 0; i < u32DevNum; i++)
    {
        ViDev = i;
		// 设置VI属性，设置WDR工作属性，最后启动VI设备
        s32Ret = SAMPLE_COMM_VI_StartDev(ViDev, enViMode);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("%s: start vi dev[%d] failed!\n", __FUNCTION__, i);
            return HI_FAILURE;
        }
    }
    
    /******************************************************
    * Step 5: config & start vicap chn (max 1) 
    ******************************************************/
    for (i = 0; i < u32ChnNum; i++)
    {
        ViChn = i;

        stCapRect.s32X = 0;
        stCapRect.s32Y = 0;
        switch (enViMode)
        {
            case APTINA_9M034_DC_720P_30FPS:
            case APTINA_AR0130_DC_720P_30FPS:
            case SONY_IMX222_DC_720P_30FPS:
            case OMNIVISION_OV9712_DC_720P_30FPS:
            case OMNIVISION_OV9732_DC_720P_30FPS:
            case OMNIVISION_OV9750_MIPI_720P_30FPS:
            case OMNIVISION_OV9752_MIPI_720P_30FPS:
                stCapRect.u32Width = 1280;
                stCapRect.u32Height = 720;
                break;        

			case SONY_IMX222_DC_1080P_30FPS:
            case APTINA_AR0230_HISPI_1080P_30FPS:
            case PANASONIC_MN34222_MIPI_1080P_30FPS:
            case OMNIVISION_OV2718_MIPI_1080P_25FPS:
                stCapRect.u32Width  = 1920;
                stCapRect.u32Height = 1080;
                break;

            default:
                stCapRect.u32Width  = 1920;
                stCapRect.u32Height = 1080;
                break;
        }

        stTargetSize.u32Width = stCapRect.u32Width;
        stTargetSize.u32Height = stCapRect.u32Height;

		// 启动VI channel 并且填充一些输出数据
        s32Ret = SAMPLE_COMM_VI_StartChn(ViChn, &stCapRect, &stTargetSize, pstViConfig);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_COMM_ISP_Stop();
            return HI_FAILURE;
        }
    }

    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_StopIsp(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    VI_DEV ViDev;
    VI_CHN ViChn;
    HI_S32 i;
    HI_S32 s32Ret;
    HI_U32 u32DevNum = 1;
    HI_U32 u32ChnNum = 1;

    if(!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }
    
    /*** Stop VI Chn ***/
    for(i=0;i < u32ChnNum; i++)
    {
        /* Stop vi phy-chn */
        ViChn = i;
        s32Ret = HI_MPI_VI_DisableChn(ViChn);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VI_DisableChn failed with %#x\n",s32Ret);
            return HI_FAILURE;
        }
    }

    /*** Stop VI Dev ***/
    for(i=0; i < u32DevNum; i++)
    {
        ViDev = i;
        s32Ret = HI_MPI_VI_DisableDev(ViDev);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("HI_MPI_VI_DisableDev failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }
    }

    SAMPLE_COMM_ISP_Stop();
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_StartVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_VI_MODE_E enViMode;  

    if(!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }
	
    enViMode = pstViConfig->enViMode;
    if(!IsSensorInput(enViMode))		// 相当于if(!IsSensorInput(enViMode) != NULL)
    {
		// 传统的模拟电信号走这条路
        s32Ret = SAMPLE_COMM_VI_StartBT656(pstViConfig);
    }
    else
    {
		// 我们调用的是这个
        s32Ret = SAMPLE_COMM_VI_StartIspAndVi(pstViConfig);
    }

    return s32Ret; 
}

HI_S32 SAMPLE_COMM_VI_StopVi(SAMPLE_VI_CONFIG_S* pstViConfig)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_VI_MODE_E enViMode;

    if(!pstViConfig)
    {
        SAMPLE_PRT("%s: null ptr\n", __FUNCTION__);
        return HI_FAILURE;
    }
    enViMode = pstViConfig->enViMode;
    
    if(!IsSensorInput(enViMode))
    {
        s32Ret = SAMPLE_COMM_VI_StopBT656(enViMode);        
    }
    else
    {
        s32Ret = SAMPLE_COMM_VI_StopIsp(pstViConfig);        
    }
    
    return s32Ret;
}

HI_S32 SAMPLE_COMM_VI_SwitchResParam( SAMPLE_VI_CONFIG_S* pstViConfig,
                                               ISP_PUB_ATTR_S *pstPubAttr, 
                                               RECT_S *pstCapRect )
{
    CHECK_NULL_PTR(pstViConfig);
    CHECK_NULL_PTR(pstPubAttr);
    CHECK_NULL_PTR(pstCapRect);
    if (SONY_IMX222_DC_1080P_30FPS == pstViConfig->enViMode)
    {
        pstViConfig->enViMode = SONY_IMX222_DC_720P_30FPS;
        pstPubAttr->stWndRect.u32Width = 1280;
        pstPubAttr->stWndRect.u32Height = 720;
	    pstPubAttr->f32FrameRate = 30;
        
        pstCapRect->s32X = 0;
    	pstCapRect->s32Y = 0;
    	pstCapRect->u32Width = 1280;
    	pstCapRect->u32Height = 720;
    }
    else if (SONY_IMX222_DC_720P_30FPS == pstViConfig->enViMode)
    {
        pstViConfig->enViMode = SONY_IMX222_DC_1080P_30FPS;
        pstPubAttr->stWndRect.u32Width = 1920;
        pstPubAttr->stWndRect.u32Height = 1080;
	    pstPubAttr->f32FrameRate = 30;
        
        pstCapRect->s32X = 0;
    	pstCapRect->s32Y = 0;
    	pstCapRect->u32Width = 1920;
    	pstCapRect->u32Height = 1080;
    }
    else
    {
        SAMPLE_PRT("This sensor type is not surpport!");
        return HI_FAILURE;
    }
    
    return HI_SUCCESS;
}

/*****************************************************************************
* function : Vi chn unbind vpss group
*****************************************************************************/
HI_S32 SAMPLE_COMM_VI_UnBindVpss(SAMPLE_VI_MODE_E enViMode)
{
    HI_S32 i, j, s32Ret;
    VPSS_GRP VpssGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    SAMPLE_VI_PARAM_S stViParam;
    VI_DEV ViDev;
    VI_CHN ViChn;

    s32Ret = SAMPLE_COMM_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return HI_FAILURE;
    }
    
    VpssGrp = 0;    
    for (i=0; i<stViParam.s32ViDevCnt; i++)
    {
        ViDev = i * stViParam.s32ViDevInterval;

        for (j=0; j<stViParam.s32ViChnCnt; j++)
        {
            ViChn = j * stViParam.s32ViChnInterval;
            
            stSrcChn.enModId = HI_ID_VIU;
            stSrcChn.s32DevId = ViDev;
            stSrcChn.s32ChnId = ViChn;
        
            stDestChn.enModId = HI_ID_VPSS;
            stDestChn.s32DevId = VpssGrp;
            stDestChn.s32ChnId = 0;
        
            s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("failed with %#x!\n", s32Ret);
                return HI_FAILURE;
            }
            
            VpssGrp ++;
        }
    }
    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VI_UnBindVenc(SAMPLE_VI_MODE_E enViMode)
{
    HI_S32 i, j, s32Ret;
    VENC_GRP  VencGrp;
    MPP_CHN_S stSrcChn;
    MPP_CHN_S stDestChn;
    SAMPLE_VI_PARAM_S stViParam;
    VI_DEV ViDev;
    VI_CHN ViChn;

    s32Ret = SAMPLE_COMM_VI_Mode2Param(enViMode, &stViParam);
    if (HI_SUCCESS !=s32Ret)
    {
        SAMPLE_PRT("SAMPLE_COMM_VI_Mode2Param failed!\n");
        return HI_FAILURE;
    }
    
    VencGrp = 0;    
    for (i=0; i<stViParam.s32ViDevCnt; i++)
    {
        ViDev = i * stViParam.s32ViDevInterval;

        for (j=0; j<stViParam.s32ViChnCnt; j++)
        {
            ViChn = j * stViParam.s32ViChnInterval;
            
            stSrcChn.enModId = HI_ID_VIU;
            stSrcChn.s32DevId = ViDev;
            stSrcChn.s32ChnId = ViChn;
        
            stDestChn.enModId = HI_ID_GROUP;
            stDestChn.s32DevId = VencGrp;
            stDestChn.s32ChnId = 0;
        
            s32Ret = HI_MPI_SYS_UnBind(&stSrcChn, &stDestChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("failed with %#x!\n", s32Ret);
                return HI_FAILURE;
            }
            
            VencGrp ++;
        }
    }
    
    return HI_SUCCESS;
}


/******************************************************************************
* function : read frame
******************************************************************************/
HI_VOID SAMPLE_COMM_VI_ReadFrame(FILE * fp, HI_U8 * pY, HI_U8 * pU, HI_U8 * pV, HI_U32 width, HI_U32 height, HI_U32 stride, HI_U32 stride2)
{
    HI_U8 * pDst;

    HI_U32 u32Row;

    pDst = pY;
    for ( u32Row = 0; u32Row < height; u32Row++ )
    {
        fread( pDst, width, 1, fp );
        pDst += stride;
    }

    pDst = pU;
    for ( u32Row = 0; u32Row < height/2; u32Row++ )
    {
        fread( pDst, width/2, 1, fp );
        pDst += stride2;
    }

    pDst = pV;
    for ( u32Row = 0; u32Row < height/2; u32Row++ )
    {
        fread( pDst, width/2, 1, fp );
        pDst += stride2;
    }
}
 
/******************************************************************************
* function : Plan to Semi
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_PlanToSemi(HI_U8 *pY, HI_S32 yStride,
                       HI_U8 *pU, HI_S32 uStride,
                       HI_U8 *pV, HI_S32 vStride,
                       HI_S32 picWidth, HI_S32 picHeight)
{
    HI_S32 i;
    HI_U8* pTmpU, *ptu;
    HI_U8* pTmpV, *ptv;
    HI_S32 s32HafW = uStride >>1 ;
    HI_S32 s32HafH = picHeight >>1 ;
    HI_S32 s32Size = s32HafW*s32HafH;

    pTmpU = malloc( s32Size );
    if (NULL == pTmpU)
    {
        printf("Func: %s() Line[%d], malloc failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;
    }
    pTmpV = malloc( s32Size );
    if (NULL == pTmpV)
    {
        printf("Func: %s() Line[%d], malloc failed\n", __FUNCTION__, __LINE__);
        return HI_FAILURE;  
    }
    ptu = pTmpU;
    ptv = pTmpV;

    memcpy(pTmpU,pU,s32Size);
    memcpy(pTmpV,pV,s32Size);

    for(i = 0;i<s32Size>>1;i++)
    {
        *pU++ = *pTmpV++;
        *pU++ = *pTmpU++;

    }
    for(i = 0;i<s32Size>>1;i++)
    {
        *pV++ = *pTmpV++;
        *pV++ = *pTmpU++;
    }

    free( ptu );
    free( ptv );

    return HI_SUCCESS;
}

int SAMPLE_COMM_VI_ExitMpp( int s32poolId)
{
    if(s32poolId<0)
    {
        if (HI_MPI_SYS_Exit())
        {
            printf("sys exit fail\n");
            return -1;
        }

        if (HI_MPI_VB_Exit())
        {
            printf("vb exit fail\n");
            return -1;
        }
        return -1;
    }
    
    return 0;
}


/******************************************************************************
* function : Get from YUV
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_GetVFrameFromYUV(FILE *pYUVFile, HI_U32 u32Width, HI_U32 u32Height,HI_U32 u32Stride, VIDEO_FRAME_INFO_S *pstVFrameInfo)
{
    HI_U32             u32LStride;
    HI_U32             u32CStride;
    HI_U32             u32LumaSize;
    HI_U32             u32ChrmSize;
    HI_U32             u32Size;
    VB_BLK VbBlk;
    HI_U32 u32PhyAddr;
    HI_U8 *pVirAddr;

    u32LStride  = u32Stride;
    u32CStride  = u32Stride;

    u32LumaSize = (u32LStride * u32Height);
    u32ChrmSize = (u32CStride * u32Height) >> 2;/* YUV 420 */
    u32Size = u32LumaSize + (u32ChrmSize << 1);

    /* alloc video buffer block ---------------------------------------------------------- */
    VbBlk = HI_MPI_VB_GetBlock(VB_INVALID_POOLID, u32Size, NULL);
    if (VB_INVALID_HANDLE == VbBlk)
    {
        SAMPLE_PRT("HI_MPI_VB_GetBlock err! size:%d\n",u32Size);
        return -1;
    }
    u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
    if (0 == u32PhyAddr)
    {
        return -1;
    }

    pVirAddr = (HI_U8 *) HI_MPI_SYS_Mmap(u32PhyAddr, u32Size);
    if (NULL == pVirAddr)
    {
        return -1;
    }

    pstVFrameInfo->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
    if (VB_INVALID_POOLID == pstVFrameInfo->u32PoolId)
    {
        return -1;
    }
    SAMPLE_PRT("pool id :%d, phyAddr:%x,virAddr:%x\n" ,pstVFrameInfo->u32PoolId,u32PhyAddr,(int)pVirAddr);

    pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
    pstVFrameInfo->stVFrame.u32PhyAddr[1] = pstVFrameInfo->stVFrame.u32PhyAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.u32PhyAddr[2] = pstVFrameInfo->stVFrame.u32PhyAddr[1] + u32ChrmSize;

    pstVFrameInfo->stVFrame.pVirAddr[0] = pVirAddr;
    pstVFrameInfo->stVFrame.pVirAddr[1] = pstVFrameInfo->stVFrame.pVirAddr[0] + u32LumaSize;
    pstVFrameInfo->stVFrame.pVirAddr[2] = pstVFrameInfo->stVFrame.pVirAddr[1] + u32ChrmSize;

    pstVFrameInfo->stVFrame.u32Width  = u32Width;
    pstVFrameInfo->stVFrame.u32Height = u32Height;
    pstVFrameInfo->stVFrame.u32Stride[0] = u32LStride;
    pstVFrameInfo->stVFrame.u32Stride[1] = u32CStride;
    pstVFrameInfo->stVFrame.u32Stride[2] = u32CStride;
    pstVFrameInfo->stVFrame.enPixelFormat = PIXEL_FORMAT_YUV_SEMIPLANAR_420;
    pstVFrameInfo->stVFrame.u32Field = VIDEO_FIELD_INTERLACED;/* Intelaced D1,otherwise VIDEO_FIELD_FRAME */

    /* read Y U V data from file to the addr ----------------------------------------------*/
    SAMPLE_COMM_VI_ReadFrame(pYUVFile, pstVFrameInfo->stVFrame.pVirAddr[0],
       pstVFrameInfo->stVFrame.pVirAddr[1], pstVFrameInfo->stVFrame.pVirAddr[2],
       pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height,
       pstVFrameInfo->stVFrame.u32Stride[0], pstVFrameInfo->stVFrame.u32Stride[1] >> 1 );

    /* convert planar YUV420 to sem-planar YUV420 -----------------------------------------*/
    SAMPLE_COMM_VI_PlanToSemi(pstVFrameInfo->stVFrame.pVirAddr[0], pstVFrameInfo->stVFrame.u32Stride[0],
      pstVFrameInfo->stVFrame.pVirAddr[1], pstVFrameInfo->stVFrame.u32Stride[1],
      pstVFrameInfo->stVFrame.pVirAddr[2], pstVFrameInfo->stVFrame.u32Stride[1],
      pstVFrameInfo->stVFrame.u32Width, pstVFrameInfo->stVFrame.u32Height);

    HI_MPI_SYS_Mmap(u32PhyAddr, u32Size);
    return 0;
}

HI_S32 SAMPLE_COMM_VI_ChangeCapSize(VI_CHN ViChn, HI_U32 u32CapWidth, HI_U32 u32CapHeight,HI_U32 u32Width, HI_U32 u32Height)
{
    VI_CHN_ATTR_S stChnAttr;
    HI_S32 S32Ret = HI_SUCCESS;
    S32Ret = HI_MPI_VI_GetChnAttr(ViChn, &stChnAttr);
    if(HI_SUCCESS!= S32Ret)
    {
        SAMPLE_PRT( "HI_MPI_VI_GetChnAttr failed\n");
    }
    stChnAttr.stCapRect.u32Width = u32CapWidth;
    stChnAttr.stCapRect.u32Height = u32CapHeight;
    stChnAttr.stDestSize.u32Width = u32Width;
    stChnAttr.stDestSize.u32Height = u32Height;

    S32Ret = HI_MPI_VI_SetChnAttr(ViChn, &stChnAttr);
    if(HI_SUCCESS!= S32Ret)
    {
        SAMPLE_PRT( "HI_MPI_VI_SetChnAttr failed\n");
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_FPN_GetVBFromPool(HI_U32 u32VbSize, HI_U32 u32Width, HI_U32 u32Height, HI_U32 u32Stride, SAMPLE_VI_FRAME_INFO_S *pstVMstFrameInfo)
{
    VB_BLK VbBlk;
    HI_U32 u32PhyAddr;
    VIDEO_FRAME_INFO_S *pstVFrameInfo = &pstVMstFrameInfo->stVideoFrame;

    printf("===============FPN VB Size: %d\n", u32VbSize);
    /* alloc video buffer block ---------------------------------------------------------- */
    VbBlk = HI_MPI_VB_GetBlock(g_Pool, u32VbSize, HI_NULL);
    if (VB_INVALID_HANDLE == VbBlk)        
    {
        printf("HI_MPI_VB_GetBlock err! size:%d\n",u32VbSize);
        return HI_FAILURE;
    }
    pstVMstFrameInfo->u32FrmSize = u32VbSize;
    pstVMstFrameInfo->VbBlk = VbBlk;

    printf("VbBlk: 0x%x\n", VbBlk);
    
    u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
    if (0 == u32PhyAddr)
    {
        printf("HI_MPI_VB_Handle2PhysAddr err!\n");
        return HI_FAILURE;
    }

    pstVFrameInfo->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
    if (VB_INVALID_POOLID == pstVFrameInfo->u32PoolId)
    {
        SAMPLE_COMM_VI_ExitMpp(pstVFrameInfo->u32PoolId);
        return -1;
    }

    pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
    pstVFrameInfo->stVFrame.u32PhyAddr[1] = 0;
    pstVFrameInfo->stVFrame.u32PhyAddr[2] = 0;

    pstVFrameInfo->stVFrame.pVirAddr[0] = NULL;
    pstVFrameInfo->stVFrame.pVirAddr[1] = NULL;
    pstVFrameInfo->stVFrame.pVirAddr[2] = NULL;

    pstVFrameInfo->stVFrame.u32Width  = u32Width;
    pstVFrameInfo->stVFrame.u32Height = u32Height;
    pstVFrameInfo->stVFrame.u32Stride[0] = u32Stride;
    pstVFrameInfo->stVFrame.u32Stride[1] = 0;
    pstVFrameInfo->stVFrame.u32Stride[2] = 0;

    return HI_SUCCESS;
}



void SAMPLE_COMM_VI_SaveFpnData(ISP_FPN_FRAME_INFO_S* pVBuf, HI_U32 u32FpnMode, HI_U32 u32Nbit, FILE* pfd)
{
    HI_U8* pU8VBufVirt_Y;
    HI_U32 phy_addr, u32Size;
    HI_U8* pUserPageAddr[2];
    HI_BOOL bCompress;

    u32Size = pVBuf->u32FrmSize;
    printf("FPN data size: %d\n", u32Size);
    
    phy_addr =  pVBuf->stFpnFrame.stVFrame.u32PhyAddr[0];
    pUserPageAddr[0] = (HI_U8*) HI_MPI_SYS_Mmap(phy_addr, u32Size);
    if (NULL == pUserPageAddr[0])
    {
        printf("HI_MPI_SYS_Mmap null\n");
        return;
    }

    pU8VBufVirt_Y = (HI_U8*)pUserPageAddr[0];

    /* save Y ----------------------------------------------------------------*/
    fprintf(stderr, "saving......Raw data......u32Stride[0]: %d, width: %d, height: %d\n"
                                , pVBuf->stFpnFrame.stVFrame.u32Stride[0]
                                , pVBuf->stFpnFrame.stVFrame.u32Width
                                , pVBuf->stFpnFrame.stVFrame.u32Height);
    //fprintf(stderr, "phy Addr: 0x%x\n", pVBuf->stFpnFrame.stVFrame.u32PhyAddr[0]);
    fflush(stderr);

    fwrite(pU8VBufVirt_Y, pVBuf->u32FrmSize, 1, pfd);
    
    /* save offset */
    fwrite(&pVBuf->u32Offset, 4, 1, pfd);

    /* save compress flag */
    bCompress = (COMPRESS_MODE_LINE == pVBuf->stFpnFrame.stVFrame.enCompressMode);
    fwrite(&bCompress, 4, 1, pfd);
    
    /* save fpn frame size */
    fwrite(&pVBuf->u32FrmSize, 4, 1, pfd);

    /* save ISO */
    fwrite(&pVBuf->u32Iso, 4, 1, pfd);
    fflush(pfd);        
    
    //fprintf(stderr, "done u32TimeRef: %d!\n", pVBuf->stFpnFrame.stVFrame.u32TimeRef);
    fflush(stderr);

    HI_MPI_SYS_Munmap(pUserPageAddr[0], u32Size);
}


HI_S32 SAMPLE_COMM_VI_ReleaseVBToPool(SAMPLE_VI_FRAME_INFO_S *pstVMstFrameInfo)
{
    HI_S32 s32Ret = HI_SUCCESS;
    
    s32Ret = HI_MPI_VB_ReleaseBlock(pstVMstFrameInfo->VbBlk);
    return s32Ret;
}


HI_S32 SAMPLE_COMM_VI_FPN_CALIBRATE_CONFIG(const char* fpn_file,    /* fpn file name */
                            ISP_FPN_TYPE_E enFpnType, /* line/frame */
                            PIXEL_FORMAT_E enPixelFormat,
                            COMPRESS_MODE_E	enCompressMode,
                            HI_U32 u32FrmNum,
                            HI_U32 u32Threshold)
{
    VI_CHN ViChn = 0;
    VI_CHN_ATTR_S stTempChnAttr;
    ISP_FPN_CALIBRATE_ATTR_S stFpnCalAttr;   
    HI_U32 u32Stride, u32VbSize;
    SAMPLE_VI_FRAME_INFO_S stVMstFrame = {0};
    HI_U32 u32Width, u32Height;
    HI_S32 s32Ret = HI_SUCCESS;
     
    ISP_DEV IspDev = 0;
    FILE* pFile;
    char fileName[256] = {0};
    
    s32Ret =  HI_MPI_VI_GetChnAttr(ViChn, &stTempChnAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("get vi chn attr failed!");
        return s32Ret;
    }
    u32Width = stTempChnAttr.stCapRect.u32Width; 
    u32Height = stTempChnAttr.stCapRect.u32Height;

    /* alloc 16bit/pixel memory */
    u32Stride = ALIGN_BACK(stTempChnAttr.stCapRect.u32Width * 2, 16);
    printf("u32Stride: %d\n", u32Stride);
    stVMstFrame.stVideoFrame.stVFrame.enPixelFormat = enPixelFormat;
    stVMstFrame.stVideoFrame.stVFrame.enCompressMode = enCompressMode;
    stVMstFrame.stVideoFrame.stVFrame.u32Field = VIDEO_FIELD_FRAME;

    if (ISP_FPN_TYPE_LINE == enFpnType)
    {
        u32VbSize = u32Stride * 1;
    }
    else
    {
        u32VbSize = (u32Stride) * u32Height;
    }
    s32Ret = SAMPLE_COMM_VI_FPN_GetVBFromPool(u32VbSize, u32Width, u32Height, u32Stride, &stVMstFrame);
    if (HI_SUCCESS != s32Ret)
    {
        printf("alloc mem failed!");
        return s32Ret;
    }

    printf("==========let isp fly ====================\n");
    printf("\n please turn off camera aperture to start calibrate!\n");
    VI_PAUSE();

    s32Ret =  HI_MPI_VI_DisableChn(ViChn);
    if (HI_SUCCESS != s32Ret)
    {
        printf("disable vi chn failed!");
        return s32Ret;
    }

    stFpnCalAttr.u32FrameNum  = u32FrmNum;
    stFpnCalAttr.u32Threshold = u32Threshold;
    stFpnCalAttr.enFpnType    = enFpnType;
    memcpy(&stFpnCalAttr.stFpnCaliFrame.stFpnFrame, &stVMstFrame.stVideoFrame, sizeof(VIDEO_FRAME_INFO_S));
    stFpnCalAttr.stFpnCaliFrame.u32Offset = 0;

    printf("compress mode: %d\n", stFpnCalAttr.stFpnCaliFrame.stFpnFrame.stVFrame.enCompressMode);
    s32Ret = HI_MPI_ISP_FPNCalibrate(IspDev, &stFpnCalAttr);
    if (HI_SUCCESS != s32Ret)
    {
        printf("HI_MPI_ISP_FPNCalibrate err: 0x%x\n", s32Ret);
        return s32Ret;
    }
    //printf("out stVMstFrame.stVideoFrame.stVFrame.u32Stride[0]: %d\n", stVMstFrame.stVideoFrame.stVFrame.u32Stride[0]);   
    /* save RAW data */ 
    snprintf(fileName, sizeof(fileName), "./%s_%d_%d_%dbit.raw", fpn_file, u32Width, u32Height, 16);
    printf("\nafter calibrate, offset =0x%x,ISO = %d\n",stFpnCalAttr.stFpnCaliFrame.u32Offset,
       stFpnCalAttr.stFpnCaliFrame.u32Iso);  

    s32Ret =  HI_MPI_VI_EnableChn(ViChn);
    if(HI_SUCCESS != s32Ret)
    {
        printf("enable vi chn failed!");
        return HI_FAILURE;
    }    
    
    printf("\nhit the Enter key,save dark frame file: %s!\n", fileName);
    VI_PAUSE();
    
    pFile = fopen(fileName, "wb");
    if (NULL == pFile)
    {
        printf("open file %s err!\n",fileName);
        return -1;
    }
    else
    {
        HI_U32 u32FpnBitWidth = 10;
        
        SAMPLE_COMM_VI_SaveFpnData(&stFpnCalAttr.stFpnCaliFrame, enFpnType, u32FpnBitWidth, pFile);        
        fclose(pFile);
    }
    printf("\nsaved dark frame!\n");
    
    s32Ret = SAMPLE_COMM_VI_ReleaseVBToPool(&stVMstFrame);
    if (HI_SUCCESS != s32Ret)
    {
        printf("SAMPLE_COMM_VI_ReleaseVBToPool err: 0x%x\n", s32Ret);        
    }
    
    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VI_ReadOneFpnFrame(FILE * fp, HI_U8 *pY, HI_U32 *pu32Offset,
                         HI_U32 *pu32FrmSize, COMPRESS_MODE_E *penCompressMode, HI_U32 *pu32Iso)
{
    HI_U8* pDst;
    HI_U32 u32FrmSize;
    HI_BOOL bCompress;
    HI_S32 s32Ret;

    /*****************************************************************
    raw file storage layout:
    ///////////////////////////////////////////////////
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------------------------------------|
    |-------------------|------|------|------|--------|
                        offset  comp   size    u32iso
    ****************************************************************/

    /* seek end of file */
    s32Ret = fseek(fp, -4, SEEK_END);
    if (0 != s32Ret)
    {
        printf("Func: %s(), line: [%d], get iso failed:  %s\n", __FUNCTION__, __LINE__, strerror(errno));
        return HI_FAILURE;
    }
    /* get calibrate ISO */
    fread(pu32Iso, 4, 1, fp);

    /* get fpn frame size */
    s32Ret = fseek(fp, -8, SEEK_END);
    if (0 != s32Ret)
    {
        printf("Func: %s(), line: [%d], get frame size failed:  %s\n", __FUNCTION__, __LINE__, strerror(errno));
        return HI_FAILURE;
    }
    fread(&u32FrmSize, 4, 1, fp);
    *pu32FrmSize = u32FrmSize;

    /* get fpn frame compress flag */
    s32Ret = fseek(fp, -12, SEEK_END);
    if (0 != s32Ret)
    {
        printf("Func: %s(), line: [%d], get compress flag failed:  %s\n", __FUNCTION__, __LINE__, strerror(errno));
        return HI_FAILURE;
    }
    fread(&bCompress, 4, 1, fp);

    if (bCompress)
    {
        *penCompressMode = COMPRESS_MODE_LINE;
    }
    else
    {
        *penCompressMode = COMPRESS_MODE_NONE;
    }

    /* get fpn offset */
    s32Ret = fseek(fp, -16, SEEK_END);
    if (0 != s32Ret)
    {
        printf("Func: %s(), line: [%d], get fpn offset failed:  %s\n", __FUNCTION__, __LINE__, strerror(errno));
        return HI_FAILURE;
    }
    fread(pu32Offset, 4, 1, fp);

    /* back to begin of file */
    fseek(fp, 0L, SEEK_SET);
    pDst = pY;
    fread(pDst, 1, u32FrmSize, fp);

    return HI_SUCCESS;
}


static inline HI_VOID SAMPLE_COMM_VI_GetBitWidthByPixFmt(PIXEL_FORMAT_E enPixelFormat, HI_U32 *pu32BitWidth)
{
    if( PIXEL_FORMAT_RGB_BAYER_8BPP == enPixelFormat)
    {
        *pu32BitWidth = 8;
    }
    else if( PIXEL_FORMAT_RGB_BAYER_10BPP == enPixelFormat)
    {
        *pu32BitWidth = 10;
    }
    else if( PIXEL_FORMAT_RGB_BAYER_12BPP == enPixelFormat)
    {
        *pu32BitWidth = 12;
    }
    else if( PIXEL_FORMAT_RGB_BAYER_14BPP == enPixelFormat)
    {
        *pu32BitWidth = 14;
    }
    else                                   
    {
        *pu32BitWidth = 16;
    }
}

HI_S32 SAMPLE_COMM_VI_GetFPNFrame_FromRaw(FILE *pRawFile,
                                 HI_U32 u32Width, HI_U32 u32Height, 
                                 SAMPLE_VI_FRAME_INFO_S *pstVMstFrameInfo,
                                 HI_U32 *pu32Offset,
                                 HI_U32 *pu32Iso)
{
    HI_U32 u32Size;
    VB_BLK VbBlk;
    HI_U32 u32PhyAddr;
    HI_U8 *pVirAddr;
    HI_U32 u32Stride;
    HI_U32 u32BitWidth = 16;
    HI_S32 s32Ret;
    VIDEO_FRAME_INFO_S *pstVFrameInfo = &pstVMstFrameInfo->stVideoFrame;
    COMPRESS_MODE_E enCompressMode = COMPRESS_MODE_NONE;

    ////////////////////////////////////////////////////////
    /* seek end of file */
    s32Ret = fseek(pRawFile, -8, SEEK_END);
    if (0 != s32Ret)
    {
        printf("Func: %s(), line: [%d], get frame size failed:  %s\n", __FUNCTION__, __LINE__, strerror(errno));
        return HI_FAILURE;
    }
    /* get fpn frame size */
    fread(&u32Size, 1, 4, pRawFile);
    /* back to begin of file */
    fseek(pRawFile, 0L, SEEK_SET);
    ////////////////////////////////////////////////////////

    /* alloc video buffer block ---------------------------------------------------------- */
    VbBlk = HI_MPI_VB_GetBlock(g_Pool, u32Size, HI_NULL);
    if (VB_INVALID_HANDLE == VbBlk)
    {
        printf("HI_MPI_VB_GetBlock err! size:%d\n",u32Size);       
        SAMPLE_COMM_VI_ExitMpp(VbBlk);       
        return HI_FAILURE;
    }
    pstVMstFrameInfo->VbBlk = VbBlk;

    u32PhyAddr = HI_MPI_VB_Handle2PhysAddr(VbBlk);
    if (0 == u32PhyAddr)
    {
        printf("HI_MPI_VB_Handle2PhysAddr err!\n");
        SAMPLE_COMM_VI_ExitMpp(u32PhyAddr);
        return HI_FAILURE;
    }
    pVirAddr = (HI_U8*) HI_MPI_SYS_Mmap(u32PhyAddr, u32Size);
    if (NULL == pVirAddr)
    {
        printf("HI_MPI_SYS_Mmap err!\n");
        SAMPLE_COMM_VI_ExitMpp((HI_S32)pVirAddr);
        return HI_FAILURE;
    }

    pstVFrameInfo->u32PoolId = HI_MPI_VB_Handle2PoolId(VbBlk);
    if (VB_INVALID_POOLID == pstVFrameInfo->u32PoolId)
    {
        HI_MPI_SYS_Munmap(pVirAddr, u32Size);
        SAMPLE_COMM_VI_ExitMpp(pstVFrameInfo->u32PoolId);
        return HI_FAILURE;
    }

    pstVFrameInfo->stVFrame.u32PhyAddr[0] = u32PhyAddr;
    pstVFrameInfo->stVFrame.u32PhyAddr[1] = 0;
    pstVFrameInfo->stVFrame.u32PhyAddr[2] = 0;

    pstVFrameInfo->stVFrame.pVirAddr[0] = pVirAddr;
    pstVFrameInfo->stVFrame.pVirAddr[1] = NULL;
    pstVFrameInfo->stVFrame.pVirAddr[2] = NULL;

    pstVFrameInfo->stVFrame.u32Width  = u32Width;
    pstVFrameInfo->stVFrame.u32Height = u32Height;
    pstVFrameInfo->stVFrame.u32Stride[0] = 0;
    pstVFrameInfo->stVFrame.u32Stride[1] = 0;
    pstVFrameInfo->stVFrame.u32Stride[2] = 0;
    pstVFrameInfo->stVFrame.u32Field = VIDEO_FIELD_FRAME;

    SAMPLE_COMM_VI_ReadOneFpnFrame(pRawFile, pstVFrameInfo->stVFrame.pVirAddr[0], pu32Offset,
                                   &pstVMstFrameInfo->u32FrmSize, &enCompressMode, pu32Iso);

    pstVFrameInfo->stVFrame.enCompressMode = enCompressMode;

    /* calculate the stride */
    u32Stride = u32Width * u32BitWidth;
    if (COMPRESS_MODE_LINE == enCompressMode)
    {
        SAMPLE_COMM_VI_GetBitWidthByPixFmt(pstVFrameInfo->stVFrame.enPixelFormat, &u32BitWidth);
        u32Stride += (16 * 8 + ((u32Width + 15) / 16 * 4));
    }
    u32Stride = ((u32Stride + 127) / 128 * 128) / 8;
    pstVFrameInfo->stVFrame.u32Stride[0] = u32Stride;
    
    HI_MPI_SYS_Munmap(pVirAddr, u32Size);
    return 0;
}



HI_S32 SAMPLE_COMM_VI_ReadOneRawFile(const char* file_name, 
                               HI_U32 u32Width, HI_U32 u32Height, 
                               SAMPLE_VI_FRAME_INFO_S *pstVMstFrameInfo, 
                               HI_U32 *u32Offset,
                               HI_U32 *pu32Iso)
{
    FILE *pfd;
    HI_S32 s32Ret = HI_SUCCESS;

    /* open YUV file */
    printf("open dark frame file: %s. \n", file_name);
    pfd = fopen(file_name, "rb");
    if (!pfd)
    {
        printf("open file -> %s fail \n", file_name);
        return HI_FAILURE;
    }

    /* read frame information from YUV file */
    s32Ret = SAMPLE_COMM_VI_GetFPNFrame_FromRaw(pfd, u32Width, u32Height, pstVMstFrameInfo, u32Offset, pu32Iso);

    fclose(pfd);
    return s32Ret;
}


HI_S32 SAMPLE_COMM_VI_CORRECTION_CONFIG(const char* fpn_file,    /* fpn file name */
                                        ISP_FPN_TYPE_E enFpnType, /* line/frame */
                                        ISP_OP_TYPE_E  enOpType,  /* auto/manual */
                                        HI_U32 u32Strength,       /* strength */                                       
                                        PIXEL_FORMAT_E enPixelFormat)
{
    VI_CHN ViChn = 0;    
    VI_CHN_ATTR_S stTempChnAttr;    
    ISP_FPN_ATTR_S stFPNAttr;
    SAMPLE_VI_FRAME_INFO_S stVMstFrame = {0};
    ISP_DEV IspDev = 0;
    ISP_FPN_FRAME_INFO_S *pstFpnFrmInfo;
    HI_S32 s32Ret = HI_SUCCESS;
    HI_U32 u32Iso;

    s32Ret = HI_MPI_VI_GetChnAttr(ViChn, &stTempChnAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("get vi chn attr failed!");
        return HI_FAILURE;
    }

    stVMstFrame.stVideoFrame.stVFrame.enPixelFormat = enPixelFormat;
    stVMstFrame.stVideoFrame.stVFrame.enCompressMode = COMPRESS_MODE_NONE;
	pstFpnFrmInfo = &stFPNAttr.stFpnFrmInfo;

    printf("\nhit the Enter key,start read dark frame\n");
    VI_PAUSE();
    s32Ret = SAMPLE_COMM_VI_ReadOneRawFile(fpn_file, 
                           stTempChnAttr.stDestSize.u32Width,
                           stTempChnAttr.stDestSize.u32Height,
                           &stVMstFrame, 
                           &pstFpnFrmInfo->u32Offset,
                           &u32Iso);

    if(HI_SUCCESS != s32Ret)
    {
        printf("read raw file failed!");
        return HI_FAILURE;
    }

    pstFpnFrmInfo->u32FrmSize = stVMstFrame.u32FrmSize;
    memcpy(&pstFpnFrmInfo->stFpnFrame, &stVMstFrame.stVideoFrame, sizeof(VIDEO_FRAME_INFO_S));
    stFPNAttr.bEnable  = HI_TRUE;
    stFPNAttr.enOpType = enOpType;
    stFPNAttr.enFpnType   = enFpnType;
	stFPNAttr.stFpnFrmInfo.u32Iso = u32Iso;
	stFPNAttr.stManual.u32Strength = u32Strength;
	
    printf("\nread u32Offset  = 0x%x, u32iso = %d\n", stFPNAttr.stFpnFrmInfo.u32Offset , stFPNAttr.stFpnFrmInfo.u32Iso);
    printf("FrmSize: %d, stride: %d, compress mode: %d\n", stFPNAttr.stFpnFrmInfo.u32FrmSize, 
                                        stFPNAttr.stFpnFrmInfo.stFpnFrame.stVFrame.u32Stride[0],
                                        stFPNAttr.stFpnFrmInfo.stFpnFrame.stVFrame.enCompressMode);

    printf("hit the Enter key,start correction\n");
    VI_PAUSE();   

    s32Ret =  HI_MPI_ISP_SetFPNAttr(IspDev, &stFPNAttr);
    if(HI_SUCCESS != s32Ret)
    {
        printf("fpn correction fail 0x%x\n", s32Ret);
        return HI_FAILURE;
    }
    
    VI_PAUSE();
    
    s32Ret = SAMPLE_COMM_VI_ReleaseVBToPool(&stVMstFrame);
    if(HI_SUCCESS != s32Ret)
    {
        printf("SAMPLE_COMM_VI_ReleaseVBToPool fail 0x%x\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : Get enSize by diffrent sensor
函数前面的SAMPLE_COMM_VI告诉我们这个函数位于: sample/common/sample_comm_vi.c中
函数作用: 通过sensor来算你get到的图像有多大
函数传参: 输出型参数，输出的是图片的分辨率大小
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_GetSizeBySensor(PIC_SIZE_E *penSize)
{
    HI_S32 s32Ret = HI_SUCCESS;
	// SENSOR_TYPE是在sample目录下的Makefile.param中定义的
    SAMPLE_VI_MODE_E enMode = SENSOR_TYPE;

// 如果SAMPLE_COMM_VI_GetSizeBySensor函数的传参等于NULL，则返回HI_FAILURE
// 这个if用来这传参参数检查
    if (!penSize)
    {
        return HI_FAILURE;
    }

	// 如果自己添加的sensor这里面没有，则需要自己在这里添加上，同时，
	// 需要在sample/Makefile.param文件中进行添加；
    switch (enMode)
    {
		// APTINA指的是公司；AR0130指的是sensor系列；DC指的是sensor的接口；
		// 720P指的是采集图像的分辨率是多大；30FPS指的是帧率大小
        case APTINA_AR0130_DC_720P_30FPS:
        case APTINA_9M034_DC_720P_30FPS:
        case SONY_IMX222_DC_720P_30FPS:
        case OMNIVISION_OV9712_DC_720P_30FPS:
        case OMNIVISION_OV9732_DC_720P_30FPS:
        case OMNIVISION_OV9750_MIPI_720P_30FPS:
        case OMNIVISION_OV9752_MIPI_720P_30FPS:
			// 解引用，指针指向的地址中存放的内容赋值为PIC_HD720
            *penSize = PIC_HD720;	// PIC_HD720值作为输出型参数传递出去
            break;
        case APTINA_AR0230_HISPI_1080P_30FPS:
        case SONY_IMX222_DC_1080P_30FPS:
        case PANASONIC_MN34222_MIPI_1080P_30FPS:
        case OMNIVISION_OV2718_MIPI_1080P_25FPS:
            *penSize = PIC_HD1080;
            break;

        default:
            printf("not support this sensor\n");
            break;
    }

    return s32Ret;
}

/******************************************************************************
* funciton : Get rgbir attr by diffrent sensor
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_GetRgbirAttrBySensor(ISP_RGBIR_ATTR_S *pstRgbirAttr)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_VI_MODE_E enMode = SENSOR_TYPE;

    if (!pstRgbirAttr)
    {
        return HI_FAILURE;
    }
    
    switch (enMode)
    {
        case OMNIVISION_OV9752_MIPI_720P_30FPS:
            pstRgbirAttr->bEnable = HI_TRUE;
            pstRgbirAttr->enIrPosType = ISP_IRPOS_TYPE_GR;
            pstRgbirAttr->u16OverExpThresh = 4031;
            break;
        case PANASONIC_MN34222_MIPI_1080P_30FPS:
            pstRgbirAttr->bEnable = HI_TRUE;
            pstRgbirAttr->enIrPosType = ISP_IRPOS_TYPE_GR;
            pstRgbirAttr->u16OverExpThresh = 3839;
            break;
        case APTINA_AR0130_DC_720P_30FPS:
        case APTINA_9M034_DC_720P_30FPS:
        case SONY_IMX222_DC_720P_30FPS:
        case APTINA_AR0230_HISPI_1080P_30FPS:
        case SONY_IMX222_DC_1080P_30FPS:
            pstRgbirAttr->bEnable = HI_FALSE;
            pstRgbirAttr->enIrPosType = ISP_IRPOS_TYPE_GR;
            pstRgbirAttr->u16OverExpThresh = 4095;
            break;

        default:
            printf("not support this sensor\n");
            break;
    }

    return s32Ret;
}

/******************************************************************************
* funciton : Get rgbir ctrl by diffrent sensor
******************************************************************************/
HI_S32 SAMPLE_COMM_VI_GetRgbirCtrlBySensor(ISP_RGBIR_CTRL_S *pstRgbirCtrl)
{
    HI_S32 s32Ret = HI_SUCCESS;
    SAMPLE_VI_MODE_E enMode = SENSOR_TYPE;

    if (!pstRgbirCtrl)
    {
        return HI_FAILURE;
    }
    
    switch (enMode)
    {
        case OMNIVISION_OV9752_MIPI_720P_30FPS:
            pstRgbirCtrl->bIrOutEn = HI_FALSE;
            pstRgbirCtrl->bIrFilterEn = HI_TRUE;
            pstRgbirCtrl->bRemovelEn = HI_TRUE;
            pstRgbirCtrl->enCompType = OP_TYPE_AUTO;
            pstRgbirCtrl->u16ManuGain = 256;
            pstRgbirCtrl->as16ScaleCoef[0] = 266;
            pstRgbirCtrl->as16ScaleCoef[1] = 3;
            pstRgbirCtrl->as16ScaleCoef[2] = 36;
            pstRgbirCtrl->as16ScaleCoef[3] = 8;
            pstRgbirCtrl->as16ScaleCoef[4] = 272;
            pstRgbirCtrl->as16ScaleCoef[5] = 30;
            pstRgbirCtrl->as16ScaleCoef[6] = 15;
            pstRgbirCtrl->as16ScaleCoef[7] = 5;
            pstRgbirCtrl->as16ScaleCoef[8] = 283;
            pstRgbirCtrl->as16ScaleCoef[9] = -300;
            pstRgbirCtrl->as16ScaleCoef[10] = -305;
            pstRgbirCtrl->as16ScaleCoef[11] = -313;
            pstRgbirCtrl->as16ScaleCoef[12] = -3;
            pstRgbirCtrl->as16ScaleCoef[13] = -12;
            pstRgbirCtrl->as16ScaleCoef[14] = -4;
            break;
        case PANASONIC_MN34222_MIPI_1080P_30FPS:
            pstRgbirCtrl->bIrOutEn = HI_FALSE;
            pstRgbirCtrl->bIrFilterEn = HI_TRUE;
            pstRgbirCtrl->bRemovelEn = HI_TRUE;
            pstRgbirCtrl->enCompType = OP_TYPE_AUTO;
            pstRgbirCtrl->u16ManuGain = 256;
            pstRgbirCtrl->as16ScaleCoef[0] = 298;
            pstRgbirCtrl->as16ScaleCoef[1] = 0;
            pstRgbirCtrl->as16ScaleCoef[2] = 21;
            pstRgbirCtrl->as16ScaleCoef[3] = 24;
            pstRgbirCtrl->as16ScaleCoef[4] = 280;
            pstRgbirCtrl->as16ScaleCoef[5] = 18;
            pstRgbirCtrl->as16ScaleCoef[6] = 3;
            pstRgbirCtrl->as16ScaleCoef[7] = 12;
            pstRgbirCtrl->as16ScaleCoef[8] = 259;
            pstRgbirCtrl->as16ScaleCoef[9] = -260;
            pstRgbirCtrl->as16ScaleCoef[10] = -265;
            pstRgbirCtrl->as16ScaleCoef[11] = -235;
            pstRgbirCtrl->as16ScaleCoef[12] = -20;
            pstRgbirCtrl->as16ScaleCoef[13] = -30;
            pstRgbirCtrl->as16ScaleCoef[14] = -5;
            break;
        case APTINA_AR0130_DC_720P_30FPS:
        case APTINA_9M034_DC_720P_30FPS:
        case SONY_IMX222_DC_720P_30FPS:
        case APTINA_AR0230_HISPI_1080P_30FPS:
        case SONY_IMX222_DC_1080P_30FPS:
            pstRgbirCtrl->bIrOutEn = HI_FALSE;
            pstRgbirCtrl->bIrFilterEn = HI_TRUE;
            pstRgbirCtrl->bRemovelEn = HI_TRUE;
            pstRgbirCtrl->enCompType = OP_TYPE_AUTO;
            pstRgbirCtrl->u16ManuGain = 256;
            pstRgbirCtrl->as16ScaleCoef[0] = 256;
            pstRgbirCtrl->as16ScaleCoef[1] = 0;
            pstRgbirCtrl->as16ScaleCoef[2] = 0;
            pstRgbirCtrl->as16ScaleCoef[3] = 0;
            pstRgbirCtrl->as16ScaleCoef[4] = 256;
            pstRgbirCtrl->as16ScaleCoef[5] = 0;
            pstRgbirCtrl->as16ScaleCoef[6] = 0;
            pstRgbirCtrl->as16ScaleCoef[7] = 0;
            pstRgbirCtrl->as16ScaleCoef[8] = 256;
            pstRgbirCtrl->as16ScaleCoef[9] = 0;
            pstRgbirCtrl->as16ScaleCoef[10] = 0;
            pstRgbirCtrl->as16ScaleCoef[11] = 0;
            pstRgbirCtrl->as16ScaleCoef[12] = 0;
            pstRgbirCtrl->as16ScaleCoef[13] = 0;
            pstRgbirCtrl->as16ScaleCoef[14] = 0;
            break;

        default:
            printf("not support this sensor\n");
            break;
    }

    return s32Ret;
}




#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

/******************************************************************************
  Some simple Hisilicon Hi35xx isp functions.

  Copyright (C), 2010-2011, Hisilicon Tech. Co., Ltd.
 ******************************************************************************
    Modification:  2011-2 Created
******************************************************************************/

#ifdef __cplusplus
#if __cplusplus
extern "C"{
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
#include "sample_comm.h"

static pthread_t gs_IspPid = 0;
static HI_BOOL gbIspInited = HI_FALSE;


/******************************************************************************
* funciton : ISP initISP的Firmware包含三部分，一部分是ISP控制单元和基础算法单元，
* 即ISP库；一部分是AE/AWB/AF算法库；一部分是sensor库。
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Init(WDR_MODE_E  enWDRMode)
{
	// 这里面处理的都是ISP设备0的属性
	ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ISP_PUB_ATTR_S stPubAttr;
    ALG_LIB_S stLib;
    
#if 0   
    /* 0. set cmos iniparser file path */
    s32Ret = sensor_set_inifile_path("configs/");
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: set cmos iniparser file path failed with %#x!\n", \
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
#endif

    /* 1. sensor register callback ；注册sensor库*/
	// sensor_register_callback函数在sensor的驱动中
	// 数据手册<ISP_3A开发指南.pdf>、<HiISP 开发参考.pdf>
    s32Ret = sensor_register_callback();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_register_callback failed with %#x!\n", \
               __FUNCTION__, s32Ret);
        return s32Ret;
    }

    /* 2. register hisi ae lib ；注册海思AE算法库*/
	// AE(auto exposure)，AE的好坏直接给人的感觉是这颗摄像头ISP调节是否正常；
	// 自动曝光是为了使感光器件获得合适的曝光量；
	// 通过获取图像的亮度调节相应的曝光参数，得到合适的曝光量，曝光参数包括光圈大小，快门速度和
	// 摄像头传感器的亮度增益；
	// 算法的主要思想是根据H3A统计值，调整曝光至目标亮度，若达到目标亮度后，除非统计值超过
	// 阈值后才会重新调整曝光。
    stLib.s32Id = 0;
	// 将HI_AE_LIB_NAME海思的AE库复制到stLib.acLibName里面；
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
	/********************************************************************************************
	函数功能:向ISP注册AE库；
	函数传参:IspDev(ISP设备号，输入型参数)；stLib(AE算法库结构体指针，输出型参数)；
	返回值:	 0表示成功；非0表示失败；
	需求:	 头文件(hi_comm_isp.h、mpi_ae.h)；库文件(libisp.a、lib_hiae.a)
	注意:	 (1)该接口调用了ISP库提供的AE注册回调接口HI_MPI_ISP_AELibRegCallBack，以实现海思
			 AE库向ISP库注册的功能；(2)AE库可以注册多个实例；
	*********************************************************************************************/
    s32Ret = HI_MPI_AE_Register(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AE_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 3. register hisi awb lib ；注册海思AWB算法库*/
	// AWB(automatic white balance)
	// 白平衡的本质是使白色物体在任何光源下都显示白色；
	// 通过白平衡增益，使拍摄画面的颜色接近物体真实的颜色，增益调节的根据是环境光源的色温；
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	/*********************************************************************************************
	函数功能:向ISP注册AWB库
	函数传参:IspDev(ISP设备号，输入型参数)；stLib(AWB算法库结构体指针，输出型参数)；
	返回值:	 0表示成功；非0表示失败；
	需求:	 头文件(hi_comm_isp.h、mpi_awb..h)、库文件(libisp.a)
	注意:	 (1)该接口调用了ISP库提供的AWB注册回调接口HI_MPI_ISP_AWBLibRegCallBack，以实现向ISP
				库注册的功能；(2)用户调用此接口完成海思AWB库向ISP库注册；(3)AWB库可以注册多个实例；
	**********************************************************************************************/
    s32Ret = HI_MPI_AWB_Register(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AWB_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 4. register hisi af lib ；注册海思AF算法库*/
	/**********************************************************************************************
	AF(auto focus)自动对焦
	自动对焦即调节摄像头焦距自动得到清晰的图像的过程；
	AF算法的基本步骤是先判断图像的模糊程度，通过合适的模糊度评价函数求得采集的每一副图像的评价值，
	然后通过搜索算法得到一系列评价值的峰值，最后通过电机驱动采集设备调节到峰值所在的位置，得到最
	清晰的图像，算法的关键在于达到准确度和速度的平衡，同时算法的精度受到软件算法和硬件精度的双重影响。
	**********************************************************************************************/
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AF_LIB_NAME);
    s32Ret = HI_MPI_AF_Register(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AF_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 5. isp mem init ；初始化ISP外部寄存器*/
	/*********************************************************************************************
	函数功能:初始化ISP外部寄存器
	函数传参:IspDev(ISP设备号，输入型参数)；
	返回值:	0表示成功；非0表示失败；
	返回值失败错误码: HI_ERR_ISP_MEM_NOT_INIT(外部寄存器没有初始化)
	 					 HI_ERR_ISP_SNS_UNREGISTER(sensor未注册)
	 					 HI_ERR_ISP_ILLEGAL_PARAM(参数无效)
	需求: 	头文件(hi_comm_isp.h、mpi_isp.h)；库文件(libisp.a)
	注意:	(1)外部寄存器初始化前需要确保ko已加载，sensor向ISP注册了回调函数；
			(2)调用本接口后，才能调用 HI_MPI_ISP_SetWDRMode 和 HI_MPI_ISP_SetPubAttr
			 	分别配置WDR模式和图像公共属性；
			(3)不支持多线程，必须要与 sensor_register_callback、 HI_MPI_AE_Register、
				HI_MPI_AWB_Register、 HI_MPI_ISP_Init、 HI_MPI_ISP_Run、 HI_MPI_ISP_Exit
				接口在同一个进程调用。
			(4)不支持重复调用本接口
			(5)推荐调用HI_MPI_ISP_Exit 后，再调用本接口重新初始化。
			(6)Huawei LiteOS 没有内核模块加载概念， Linux load ko 过程对应 Huawei LiteOS
				release/ko 下 sdk_init.c 中执行的相关过程。
	*********************************************************************************************/
    s32Ret = HI_MPI_ISP_MemInit(IspDev);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_Init failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 6. isp set WDR mode ；配置ISP宽动态模式*/
	/********************************************************************************************
	WDR(wide dynamic range)，就是场景中特别亮的部位和特别暗的部分同时都能看得特别清除。
	宽动态范围是图像分辨最高的亮度信号值与能分辨的最暗的亮度信号值的比值。
	函数功能:设置ISP宽动态模式；
	函数传参:IspDev(ISP设备号，输入型参数)；stWdrMode(宽动态模式，输入型参数)；
	返回值:	0表示成功；非0表示失败；
	需求:	头文件(hi_comm_isp.h、mpi_isp.h)、库文件(libisp.a)
	注意:	(1)ISP启动时，需要确保已调用 HI_MPI_ISP_MemInit 初始化 ISP 外部寄存器；
			(2)支持在ISP运行之后，调用本接口实现宽动态切花；
	 		(3)如果在相同的WDR模式之间切换时，请上层应用程序不要重新设置MIPI接口，
				否则会导致采集不到图像。在相同的WDR模式之间进行切换时，建议上层应用
				程序判断当前的WDR模式与要切换的WDR模式是否相同，如果相同，则可以直接
				推出即可，不用进行切换。
	*********************************************************************************************/
    ISP_WDR_MODE_S stWdrMode;
    stWdrMode.enWDRMode  = enWDRMode;
    s32Ret = HI_MPI_ISP_SetWDRMode(0, &stWdrMode);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("start ISP WDR failed!\n");
        return s32Ret;
    }

    /* 7. isp set pub attributes ；配置图像公共属性*/
    /* note : different sensor, different ISP_PUB_ATTR_S define.
              if the sensor you used is different, you can change
              ISP_PUB_ATTR_S definition */

    switch(SENSOR_TYPE)
    {
        case APTINA_9M034_DC_720P_30FPS:
        case APTINA_AR0130_DC_720P_30FPS:
            stPubAttr.enBayer               = BAYER_GRBG;	// rawRGB 原始图像数据格式
            stPubAttr.f32FrameRate          = 30;			// 帧率
            stPubAttr.stWndRect.s32X        = 0;			// 图像区域起始点
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1280;			// 图像宽
            stPubAttr.stWndRect.u32Height   = 720;			// 图像高
            break;

		case SONY_IMX222_DC_1080P_30FPS:
			stPubAttr.enBayer               = BAYER_RGGB;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 200;
            stPubAttr.stWndRect.s32Y        = 20;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;

        case SONY_IMX222_DC_720P_30FPS:
			stPubAttr.enBayer               = BAYER_RGGB;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 200;
            stPubAttr.stWndRect.s32Y        = 20;
            stPubAttr.stWndRect.u32Width    = 1280;
            stPubAttr.stWndRect.u32Height   = 720;
            break;

        case APTINA_AR0230_HISPI_1080P_30FPS:
            stPubAttr.enBayer               = BAYER_GRBG;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;

        case PANASONIC_MN34222_MIPI_1080P_30FPS:
            stPubAttr.enBayer               = BAYER_GRBG;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;

        case OMNIVISION_OV9712_DC_720P_30FPS:
            stPubAttr.enBayer               = BAYER_BGGR;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1280;
            stPubAttr.stWndRect.u32Height   = 720;
            break;
            
        case OMNIVISION_OV9732_DC_720P_30FPS:
            stPubAttr.enBayer               = BAYER_BGGR;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1280;
            stPubAttr.stWndRect.u32Height   = 720;
            break;

        case OMNIVISION_OV9750_MIPI_720P_30FPS:
        case OMNIVISION_OV9752_MIPI_720P_30FPS:
            stPubAttr.enBayer               = BAYER_BGGR;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1280;
            stPubAttr.stWndRect.u32Height   = 720;
            break;

        case OMNIVISION_OV2718_MIPI_1080P_25FPS:
            stPubAttr.enBayer               = BAYER_BGGR;
            stPubAttr.f32FrameRate          = 25;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;
            
        default:
            stPubAttr.enBayer      = BAYER_GRBG;
            stPubAttr.f32FrameRate          = 30;
            stPubAttr.stWndRect.s32X        = 0;
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1920;
            stPubAttr.stWndRect.u32Height   = 1080;
            break;
    }

	/*********************************************************************************************
	调用HI_MPI_ISP_SetPubAttr函数将图像参数写到ISP单元中
	函数功能:设置ISP功能属性；
	函数传参:IspDev(ISP设备号，输入型参数)；stPubAttr(ISP公共属性，输入型参数)；
	返回值:	0表示成功；非0表示失败
	失败错误码: HI_ERR_ISP_NULL_PTR(空指针错误)、HI_ERR_ISP_ILLEGAL_PARAM(参数无效)
	需求:	 头文件(hi_comm_isp.h、mpi_isp.h)、库文件(libisp.a)
	注意:	(1)图像属性即对应的sensor的采集属性；
			(2)ISP启动时，需要确保已调用HI_MPI_ISP_MemInit 初始化 ISP 外部寄存器；
	 		(3)支持在 ISP 运行之后，调用本接口实现动态分辨率和帧率切换。
	 		(4)调用本接口后ISP内的处理流程:a)ISP firmware判断图像分辨率和帧率是否变化，
	 			若都不变则直接返回；否则，ISP firmware会调用sensor cmos.c里面的cmos_set_image_mode
				函数改变sensor模式；b)若sensor模式改变(返回值为0)，则ISP firmware会调用sensor_init
				函数重新配置sensor；c)ISP firmware将帧率信息传给海思AE库，并决定是否更改帧率。
			(5)若调用本接口实现动态分辨率和帧率切换时sensor模式发生了改变，请参照sample提供的
				切换流程操作(先停掉vi设备，再切换，最后启动vi设备)。另外，动态分辨率和帧率切换时，
	 			切换的分辨率和帧率必须有一项要不同(即不能切换到自己本身)，否则，sensor可能不会
				重新初始化而导致异常。
	 		(6)使用Vi Dev和ISP提供的裁剪功能时，需要注意: 若裁剪后的分辨率和帧率，小于另一组
				sensor模式的分辨率和帧率，则调用本接口会先切换到对应的sensor模式。
			(7)用户可以更改sensor cmos.c里面的cmos_set_image_mode函数调整sensor模式切换的顺序，
				如只提供了5M30fps和1080P60fps初始化序列的sensor，若要运行1080P30fps，可以从5M30fps
				裁剪得到，也可以从1080P60fps降帧得到，修改cmos_set_image_mode函数实现即可。
	**********************************************************************************************/
    s32Ret = HI_MPI_ISP_SetPubAttr(IspDev, &stPubAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetPubAttr failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    /* 8. isp init ；前7步将ISP的参数设置好了，这一步是ISP自己本身的初始化*/
	/**********************************************************************************************
	函数功能:初始化ISP firmware
	函数传参:IspDev(ISP设备号，输入型参数)
	返回值:	 0表示成功；非0表示失败
	失败错误码: HI_ERR_ISP_MEM_NOT_INIT(外部寄存器没有初始化)、HI_ERR_ISP_NOT_INIT(没有初始化)
				HI_ERR_ISP_SNS_UNREGISTER(sensor未注册)
	需求:	 头文件(hi_comm_isp.h、mpi_isp.h)、库文件(libisp.a)
	注意:	 (1)初始化前需要确保ko已加载，sensor向ISP注册了回调函数；
			 (2)初始化前需要确保已调用HI_MPI_ISP_MemInit 初始化 ISP 外部寄存器；
			 (3)初始化前需要确保已调用 HI_MPI_ISP_SetWDRMode 和 HI_MPI_ISP_SetPubAttr分别配置WDR
			 	模式和图像公共属性；
			 (4)不支持多线程，必须要与 sensor_register_callback、 HI_MPI_AE_Register、
				HI_MPI_AWB_Register、 HI_MPI_ISP_MemInit、 HI_MPI_ISP_Run、
				HI_MPI_ISP_Exit 接口在同一个进程调用；
			 (5)不支持重复调用本接口；
			 (6)推荐调用HI_MPI_ISP_Exit 后，再调用本接口重新初始化；
			 (7)Huawei LiteOS 没内核模块加载概念， Linux load ko 过程对应 Huawei LiteOS
				release/ko 下 sdk_init.c 中执行的相关过程；
	***********************************************************************************************/
    s32Ret = HI_MPI_ISP_Init(IspDev);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_Init failed!\n", __FUNCTION__);
        return s32Ret;
    }

    gbIspInited = HI_TRUE;

    return HI_SUCCESS;
}

HI_VOID* Test_ISP_Run(HI_VOID *param)
{
    ISP_DEV IspDev = 0;
	// 最后调用这个
	/*********************************************************************************************
	函数作用:运行ISP firmware
	函数传参:IspDev(ISP设备号，输入型参数)
	返回值:0表示成功；非0表示失败
	失败错误码:	HI_ERR_ISP_SNS_UNREGISTER(sensor未注册)、
				HI_ERR_ISP_MEM_NOT_INIT(外部寄存器没有初始化)、HI_ERR_ISP_NOT_INIT(没有初始化)
	需求:	 头文件(hi_comm_isp.h、mpi_isp.h)、库文件(libisp.a)
	注意:	 (1)运行前需要确保sensor已经初始化，并且向ISP注册了回调函数；
			 (2)运行前需要确保已调用HI_MPI_ISP_Init 初始化 ISP；
			 (3)不支持多进程，必须要与sensor_register_callback、 HI_MPI_AE_Register、
				HI_MPI_AWB_Register、 HI_MPI_ISP_MemInit、 HI_MPI_ISP_Init、
				HI_MPI_ISP_Exit 接口在同一个进程调用；
			 (4)该接口是阻塞接口，建议用户采用实时线程处理；
	*********************************************************************************************/
    HI_MPI_ISP_Run(IspDev);

    return HI_NULL;
}

/******************************************************************************
* funciton : ISP Run
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Run()
{
#if 1

#if (CHIP_ID == CHIP_HI3516C_V200)
// 创建一个线程pthread_create，让ISP在线程中运行
    if (0 != pthread_create(&gs_IspPid, 0, (void* (*)(void*))Test_ISP_Run, NULL))
    {
        printf("%s: create isp running thread failed!\n", __FUNCTION__);
        return HI_FAILURE;
    }
    usleep(1000);
#else
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 4096 * 1024);
	// 单独启动线程运行
    if (0 != pthread_create(&gs_IspPid, &attr, (void* (*)(void*))Test_ISP_Run, NULL))
    {
        printf("%s: create isp running thread failed!\n", __FUNCTION__);
        pthread_attr_destroy(&attr);
        return HI_FAILURE;
    }
    usleep(1000);
    pthread_attr_destroy(&attr);
#endif

#else
	/* configure thread priority */
	if (1)
	{
		#include <sched.h>

		pthread_attr_t attr;
		struct sched_param param;
		int newprio = 50;

		pthread_attr_init(&attr);

		if (1)
		{
			int policy = 0;
			int min, max;

			pthread_attr_getschedpolicy(&attr, &policy);
			printf("-->default thread use policy is %d --<\n", policy);

			pthread_attr_setschedpolicy(&attr, SCHED_RR);
			pthread_attr_getschedpolicy(&attr, &policy);
			printf("-->current thread use policy is %d --<\n", policy);

			switch (policy)
			{
				case SCHED_FIFO:
					printf("-->current thread use policy is SCHED_FIFO --<\n");
					break;

				case SCHED_RR:
					printf("-->current thread use policy is SCHED_RR --<\n");
					break;

				case SCHED_OTHER:
					printf("-->current thread use policy is SCHED_OTHER --<\n");
					break;

				default:
					printf("-->current thread use policy is UNKNOW --<\n");
					break;
			}

			min = sched_get_priority_min(policy);
			max = sched_get_priority_max(policy);

			printf("-->current thread policy priority range (%d ~ %d) --<\n", min, max);
		}

		pthread_attr_getschedparam(&attr, &param);

		printf("-->default isp thread priority is %d , next be %d --<\n", param.sched_priority, newprio);
		param.sched_priority = newprio;
		pthread_attr_setschedparam(&attr, &param);

		if (0 != pthread_create(&gs_IspPid, &attr, (void* (*)(void*))HI_MPI_ISP_Run, NULL))
		{
			printf("%s: create isp running thread failed!\n", __FUNCTION__);
			return HI_FAILURE;
		}

		pthread_attr_destroy(&attr);
	}
#endif

    return HI_SUCCESS;
}

/******************************************************************************
* funciton : change between linear and wdr mode
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_ChangeSensorMode(HI_U8 u8Mode)
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    
    ISP_WDR_MODE_S stWDRMode;
    stWDRMode.enWDRMode = u8Mode;
    s32Ret = HI_MPI_ISP_SetWDRMode(IspDev, &stWDRMode);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetWDRMode failed with %#x!\n", 
               __FUNCTION__, s32Ret);
        return s32Ret;
    }
    
    return HI_SUCCESS;
}


/******************************************************************************
* funciton : stop ISP, and stop isp thread
******************************************************************************/
HI_VOID SAMPLE_COMM_ISP_Stop()
{
    ISP_DEV IspDev = 0;
    HI_S32 s32Ret;
    ALG_LIB_S stLib;

    if (!gbIspInited)
    {
        return;
    }

	/**********************************************************************************************
	函数功能: 推出ISP firmware
	函数传参: IspDev(ISP设备号，输入型参数)
	返回值: 0表示成功，非0表示失败；
	需求: 头文件(hi_comm_isp.h、mpi_isp.h)、库文件(libisp.a)
	注意: (1)调用 HI_MPI_ISP_Init 和 HI_MPI_ISP_Run 之后，再调用本接口退出 ISPfirmware；
		  (2)不支持多进程，必须要与 sensor_register_callback、 HI_MPI_AE_Register、HI_MPI_AWB_Register、 
		  	 HI_MPI_ISP_MemInit、 HI_MPI_ISP_Init、HI_MPI_ISP_Run 接口在同一个进程调用。
		  (3)支持重复调用本接口；
	**********************************************************************************************/
    HI_MPI_ISP_Exit(IspDev);
    if (gs_IspPid)
    {
        pthread_join(gs_IspPid, 0);
        gs_IspPid = 0;
    }
    gbIspInited = HI_FALSE;
    
    /* unregister hisi af lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AF_LIB_NAME);
	// 向ISP反注册AE库
    s32Ret = HI_MPI_AF_UnRegister(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AF_UnRegister failed!\n", __FUNCTION__);
        return;
    }

    /* unregister hisi awb lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	/**********************************************************************************************
	函数功能: 向ISP注销AWB库；
	函数传参: IspDev(设备编号、输入型参数)、stLib(AWB算法库结构体指针，输出型参数)
	返回值: 0表示成功，非0表示失败
	需求: 头文件(hi_comm_isp.h、mpi_awb.h)、库文件(libisp.a)
	注意: (1)该接口调用了ISP库提供的AWB反注册回调接口HI_MPI_ISP_AWBLibRegCallBack，以实现AWB向ISP 
			 库反注册的功能。
		  (2)用户调用此接口完成海思AWB库向ISP库反注册；
	**********************************************************************************************/
    s32Ret = HI_MPI_AWB_UnRegister(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AWB_UnRegister failed!\n", __FUNCTION__);
        return;
    }

    /* unregister hisi ae lib */
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
	/*********************************************************************************************
	函数功能: 向ISP反注册AE库；
	函数传参: IspDev(设备编号、输入型参数)、stLib(AE算法库结构体指针，输出型参数)
	返回值: 0表示成功，非0表示失败；
	需求: 头文件(hi_comm_isp.h、mpi_ae.h)、库文件(libisp.a、lib_hiae.a)
	注意: 该接口调用了ISP库提供的AE反注册回调接口 HI_MPI_ISP_AELibUnRegCallBack，以实现AE向ISP库
		  反注册的功能。
	*********************************************************************************************/
    s32Ret = HI_MPI_AE_UnRegister(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AE_UnRegister failed!\n", __FUNCTION__);
        return;
    }

    /* sensor unregister callback */
    s32Ret = sensor_unregister_callback();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_unregister_callback failed with %#x!\n", \
               __FUNCTION__, s32Ret);
        return;
    }
    
    return;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */


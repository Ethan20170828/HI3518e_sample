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
* funciton : ISP initISP��Firmware���������֣�һ������ISP���Ƶ�Ԫ�ͻ����㷨��Ԫ��
* ��ISP�⣻һ������AE/AWB/AF�㷨�⣻һ������sensor�⡣
******************************************************************************/
HI_S32 SAMPLE_COMM_ISP_Init(WDR_MODE_E  enWDRMode)
{
	// �����洦��Ķ���ISP�豸0������
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

    /* 1. sensor register callback ��ע��sensor��*/
	// sensor_register_callback������sensor��������
	// �����ֲ�<ISP_3A����ָ��.pdf>��<HiISP �����ο�.pdf>
    s32Ret = sensor_register_callback();
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: sensor_register_callback failed with %#x!\n", \
               __FUNCTION__, s32Ret);
        return s32Ret;
    }

    /* 2. register hisi ae lib ��ע�ả˼AE�㷨��*/
	// AE(auto exposure)��AE�ĺû�ֱ�Ӹ��˵ĸо����������ͷISP�����Ƿ�������
	// �Զ��ع���Ϊ��ʹ�й�������ú��ʵ��ع�����
	// ͨ����ȡͼ������ȵ�����Ӧ���ع�������õ����ʵ��ع������ع����������Ȧ��С�������ٶȺ�
	// ����ͷ���������������棻
	// �㷨����Ҫ˼���Ǹ���H3Aͳ��ֵ�������ع���Ŀ�����ȣ����ﵽĿ�����Ⱥ󣬳���ͳ��ֵ����
	// ��ֵ��Ż����µ����ع⡣
    stLib.s32Id = 0;
	// ��HI_AE_LIB_NAME��˼��AE�⸴�Ƶ�stLib.acLibName���棻
    strcpy(stLib.acLibName, HI_AE_LIB_NAME);
	/********************************************************************************************
	��������:��ISPע��AE�⣻
	��������:IspDev(ISP�豸�ţ������Ͳ���)��stLib(AE�㷨��ṹ��ָ�룬����Ͳ���)��
	����ֵ:	 0��ʾ�ɹ�����0��ʾʧ�ܣ�
	����:	 ͷ�ļ�(hi_comm_isp.h��mpi_ae.h)�����ļ�(libisp.a��lib_hiae.a)
	ע��:	 (1)�ýӿڵ�����ISP���ṩ��AEע��ص��ӿ�HI_MPI_ISP_AELibRegCallBack����ʵ�ֺ�˼
			 AE����ISP��ע��Ĺ��ܣ�(2)AE�����ע����ʵ����
	*********************************************************************************************/
    s32Ret = HI_MPI_AE_Register(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AE_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 3. register hisi awb lib ��ע�ả˼AWB�㷨��*/
	// AWB(automatic white balance)
	// ��ƽ��ı�����ʹ��ɫ�������κι�Դ�¶���ʾ��ɫ��
	// ͨ����ƽ�����棬ʹ���㻭�����ɫ�ӽ�������ʵ����ɫ��������ڵĸ����ǻ�����Դ��ɫ�£�
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AWB_LIB_NAME);
	/*********************************************************************************************
	��������:��ISPע��AWB��
	��������:IspDev(ISP�豸�ţ������Ͳ���)��stLib(AWB�㷨��ṹ��ָ�룬����Ͳ���)��
	����ֵ:	 0��ʾ�ɹ�����0��ʾʧ�ܣ�
	����:	 ͷ�ļ�(hi_comm_isp.h��mpi_awb..h)�����ļ�(libisp.a)
	ע��:	 (1)�ýӿڵ�����ISP���ṩ��AWBע��ص��ӿ�HI_MPI_ISP_AWBLibRegCallBack����ʵ����ISP
				��ע��Ĺ��ܣ�(2)�û����ô˽ӿ���ɺ�˼AWB����ISP��ע�᣻(3)AWB�����ע����ʵ����
	**********************************************************************************************/
    s32Ret = HI_MPI_AWB_Register(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AWB_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 4. register hisi af lib ��ע�ả˼AF�㷨��*/
	/**********************************************************************************************
	AF(auto focus)�Զ��Խ�
	�Զ��Խ�����������ͷ�����Զ��õ�������ͼ��Ĺ��̣�
	AF�㷨�Ļ������������ж�ͼ���ģ���̶ȣ�ͨ�����ʵ�ģ�������ۺ�����òɼ���ÿһ��ͼ�������ֵ��
	Ȼ��ͨ�������㷨�õ�һϵ������ֵ�ķ�ֵ�����ͨ����������ɼ��豸���ڵ���ֵ���ڵ�λ�ã��õ���
	������ͼ���㷨�Ĺؼ����ڴﵽ׼ȷ�Ⱥ��ٶȵ�ƽ�⣬ͬʱ�㷨�ľ����ܵ�����㷨��Ӳ�����ȵ�˫��Ӱ�졣
	**********************************************************************************************/
    stLib.s32Id = 0;
    strcpy(stLib.acLibName, HI_AF_LIB_NAME);
    s32Ret = HI_MPI_AF_Register(IspDev, &stLib);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_AF_Register failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 5. isp mem init ����ʼ��ISP�ⲿ�Ĵ���*/
	/*********************************************************************************************
	��������:��ʼ��ISP�ⲿ�Ĵ���
	��������:IspDev(ISP�豸�ţ������Ͳ���)��
	����ֵ:	0��ʾ�ɹ�����0��ʾʧ�ܣ�
	����ֵʧ�ܴ�����: HI_ERR_ISP_MEM_NOT_INIT(�ⲿ�Ĵ���û�г�ʼ��)
	 					 HI_ERR_ISP_SNS_UNREGISTER(sensorδע��)
	 					 HI_ERR_ISP_ILLEGAL_PARAM(������Ч)
	����: 	ͷ�ļ�(hi_comm_isp.h��mpi_isp.h)�����ļ�(libisp.a)
	ע��:	(1)�ⲿ�Ĵ�����ʼ��ǰ��Ҫȷ��ko�Ѽ��أ�sensor��ISPע���˻ص�������
			(2)���ñ��ӿں󣬲��ܵ��� HI_MPI_ISP_SetWDRMode �� HI_MPI_ISP_SetPubAttr
			 	�ֱ�����WDRģʽ��ͼ�񹫹����ԣ�
			(3)��֧�ֶ��̣߳�����Ҫ�� sensor_register_callback�� HI_MPI_AE_Register��
				HI_MPI_AWB_Register�� HI_MPI_ISP_Init�� HI_MPI_ISP_Run�� HI_MPI_ISP_Exit
				�ӿ���ͬһ�����̵��á�
			(4)��֧���ظ����ñ��ӿ�
			(5)�Ƽ�����HI_MPI_ISP_Exit ���ٵ��ñ��ӿ����³�ʼ����
			(6)Huawei LiteOS û���ں�ģ����ظ�� Linux load ko ���̶�Ӧ Huawei LiteOS
				release/ko �� sdk_init.c ��ִ�е���ع��̡�
	*********************************************************************************************/
    s32Ret = HI_MPI_ISP_MemInit(IspDev);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_Init failed!\n", __FUNCTION__);
        return s32Ret;
    }

    /* 6. isp set WDR mode ������ISP��̬ģʽ*/
	/********************************************************************************************
	WDR(wide dynamic range)�����ǳ������ر����Ĳ�λ���ر𰵵Ĳ���ͬʱ���ܿ����ر������
	��̬��Χ��ͼ��ֱ���ߵ������ź�ֵ���ֱܷ����������ź�ֵ�ı�ֵ��
	��������:����ISP��̬ģʽ��
	��������:IspDev(ISP�豸�ţ������Ͳ���)��stWdrMode(��̬ģʽ�������Ͳ���)��
	����ֵ:	0��ʾ�ɹ�����0��ʾʧ�ܣ�
	����:	ͷ�ļ�(hi_comm_isp.h��mpi_isp.h)�����ļ�(libisp.a)
	ע��:	(1)ISP����ʱ����Ҫȷ���ѵ��� HI_MPI_ISP_MemInit ��ʼ�� ISP �ⲿ�Ĵ�����
			(2)֧����ISP����֮�󣬵��ñ��ӿ�ʵ�ֿ�̬�л���
	 		(3)�������ͬ��WDRģʽ֮���л�ʱ�����ϲ�Ӧ�ó���Ҫ��������MIPI�ӿڣ�
				����ᵼ�²ɼ�����ͼ������ͬ��WDRģʽ֮������л�ʱ�������ϲ�Ӧ��
				�����жϵ�ǰ��WDRģʽ��Ҫ�л���WDRģʽ�Ƿ���ͬ�������ͬ�������ֱ��
				�Ƴ����ɣ����ý����л���
	*********************************************************************************************/
    ISP_WDR_MODE_S stWdrMode;
    stWdrMode.enWDRMode  = enWDRMode;
    s32Ret = HI_MPI_ISP_SetWDRMode(0, &stWdrMode);    
    if (HI_SUCCESS != s32Ret)
    {
        printf("start ISP WDR failed!\n");
        return s32Ret;
    }

    /* 7. isp set pub attributes ������ͼ�񹫹�����*/
    /* note : different sensor, different ISP_PUB_ATTR_S define.
              if the sensor you used is different, you can change
              ISP_PUB_ATTR_S definition */

    switch(SENSOR_TYPE)
    {
        case APTINA_9M034_DC_720P_30FPS:
        case APTINA_AR0130_DC_720P_30FPS:
            stPubAttr.enBayer               = BAYER_GRBG;	// rawRGB ԭʼͼ�����ݸ�ʽ
            stPubAttr.f32FrameRate          = 30;			// ֡��
            stPubAttr.stWndRect.s32X        = 0;			// ͼ��������ʼ��
            stPubAttr.stWndRect.s32Y        = 0;
            stPubAttr.stWndRect.u32Width    = 1280;			// ͼ���
            stPubAttr.stWndRect.u32Height   = 720;			// ͼ���
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
	����HI_MPI_ISP_SetPubAttr������ͼ�����д��ISP��Ԫ��
	��������:����ISP�������ԣ�
	��������:IspDev(ISP�豸�ţ������Ͳ���)��stPubAttr(ISP�������ԣ������Ͳ���)��
	����ֵ:	0��ʾ�ɹ�����0��ʾʧ��
	ʧ�ܴ�����: HI_ERR_ISP_NULL_PTR(��ָ�����)��HI_ERR_ISP_ILLEGAL_PARAM(������Ч)
	����:	 ͷ�ļ�(hi_comm_isp.h��mpi_isp.h)�����ļ�(libisp.a)
	ע��:	(1)ͼ�����Լ���Ӧ��sensor�Ĳɼ����ԣ�
			(2)ISP����ʱ����Ҫȷ���ѵ���HI_MPI_ISP_MemInit ��ʼ�� ISP �ⲿ�Ĵ�����
	 		(3)֧���� ISP ����֮�󣬵��ñ��ӿ�ʵ�ֶ�̬�ֱ��ʺ�֡���л���
	 		(4)���ñ��ӿں�ISP�ڵĴ�������:a)ISP firmware�ж�ͼ��ֱ��ʺ�֡���Ƿ�仯��
	 			����������ֱ�ӷ��أ�����ISP firmware�����sensor cmos.c�����cmos_set_image_mode
				�����ı�sensorģʽ��b)��sensorģʽ�ı�(����ֵΪ0)����ISP firmware�����sensor_init
				������������sensor��c)ISP firmware��֡����Ϣ������˼AE�⣬�������Ƿ����֡�ʡ�
			(5)�����ñ��ӿ�ʵ�ֶ�̬�ֱ��ʺ�֡���л�ʱsensorģʽ�����˸ı䣬�����sample�ṩ��
				�л����̲���(��ͣ��vi�豸�����л����������vi�豸)�����⣬��̬�ֱ��ʺ�֡���л�ʱ��
	 			�л��ķֱ��ʺ�֡�ʱ�����һ��Ҫ��ͬ(�������л����Լ�����)������sensor���ܲ���
				���³�ʼ���������쳣��
	 		(6)ʹ��Vi Dev��ISP�ṩ�Ĳü�����ʱ����Ҫע��: ���ü���ķֱ��ʺ�֡�ʣ�С����һ��
				sensorģʽ�ķֱ��ʺ�֡�ʣ�����ñ��ӿڻ����л�����Ӧ��sensorģʽ��
			(7)�û����Ը���sensor cmos.c�����cmos_set_image_mode��������sensorģʽ�л���˳��
				��ֻ�ṩ��5M30fps��1080P60fps��ʼ�����е�sensor����Ҫ����1080P30fps�����Դ�5M30fps
				�ü��õ���Ҳ���Դ�1080P60fps��֡�õ����޸�cmos_set_image_mode����ʵ�ּ��ɡ�
	**********************************************************************************************/
    s32Ret = HI_MPI_ISP_SetPubAttr(IspDev, &stPubAttr);
    if (s32Ret != HI_SUCCESS)
    {
        printf("%s: HI_MPI_ISP_SetPubAttr failed with %#x!\n", __FUNCTION__, s32Ret);
        return s32Ret;
    }

    /* 8. isp init ��ǰ7����ISP�Ĳ������ú��ˣ���һ����ISP�Լ�����ĳ�ʼ��*/
	/**********************************************************************************************
	��������:��ʼ��ISP firmware
	��������:IspDev(ISP�豸�ţ������Ͳ���)
	����ֵ:	 0��ʾ�ɹ�����0��ʾʧ��
	ʧ�ܴ�����: HI_ERR_ISP_MEM_NOT_INIT(�ⲿ�Ĵ���û�г�ʼ��)��HI_ERR_ISP_NOT_INIT(û�г�ʼ��)
				HI_ERR_ISP_SNS_UNREGISTER(sensorδע��)
	����:	 ͷ�ļ�(hi_comm_isp.h��mpi_isp.h)�����ļ�(libisp.a)
	ע��:	 (1)��ʼ��ǰ��Ҫȷ��ko�Ѽ��أ�sensor��ISPע���˻ص�������
			 (2)��ʼ��ǰ��Ҫȷ���ѵ���HI_MPI_ISP_MemInit ��ʼ�� ISP �ⲿ�Ĵ�����
			 (3)��ʼ��ǰ��Ҫȷ���ѵ��� HI_MPI_ISP_SetWDRMode �� HI_MPI_ISP_SetPubAttr�ֱ�����WDR
			 	ģʽ��ͼ�񹫹����ԣ�
			 (4)��֧�ֶ��̣߳�����Ҫ�� sensor_register_callback�� HI_MPI_AE_Register��
				HI_MPI_AWB_Register�� HI_MPI_ISP_MemInit�� HI_MPI_ISP_Run��
				HI_MPI_ISP_Exit �ӿ���ͬһ�����̵��ã�
			 (5)��֧���ظ����ñ��ӿڣ�
			 (6)�Ƽ�����HI_MPI_ISP_Exit ���ٵ��ñ��ӿ����³�ʼ����
			 (7)Huawei LiteOS û�ں�ģ����ظ�� Linux load ko ���̶�Ӧ Huawei LiteOS
				release/ko �� sdk_init.c ��ִ�е���ع��̣�
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
	// ���������
	/*********************************************************************************************
	��������:����ISP firmware
	��������:IspDev(ISP�豸�ţ������Ͳ���)
	����ֵ:0��ʾ�ɹ�����0��ʾʧ��
	ʧ�ܴ�����:	HI_ERR_ISP_SNS_UNREGISTER(sensorδע��)��
				HI_ERR_ISP_MEM_NOT_INIT(�ⲿ�Ĵ���û�г�ʼ��)��HI_ERR_ISP_NOT_INIT(û�г�ʼ��)
	����:	 ͷ�ļ�(hi_comm_isp.h��mpi_isp.h)�����ļ�(libisp.a)
	ע��:	 (1)����ǰ��Ҫȷ��sensor�Ѿ���ʼ����������ISPע���˻ص�������
			 (2)����ǰ��Ҫȷ���ѵ���HI_MPI_ISP_Init ��ʼ�� ISP��
			 (3)��֧�ֶ���̣�����Ҫ��sensor_register_callback�� HI_MPI_AE_Register��
				HI_MPI_AWB_Register�� HI_MPI_ISP_MemInit�� HI_MPI_ISP_Init��
				HI_MPI_ISP_Exit �ӿ���ͬһ�����̵��ã�
			 (4)�ýӿ��������ӿڣ������û�����ʵʱ�̴߳���
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
// ����һ���߳�pthread_create����ISP���߳�������
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
	// ���������߳�����
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
	��������: �Ƴ�ISP firmware
	��������: IspDev(ISP�豸�ţ������Ͳ���)
	����ֵ: 0��ʾ�ɹ�����0��ʾʧ�ܣ�
	����: ͷ�ļ�(hi_comm_isp.h��mpi_isp.h)�����ļ�(libisp.a)
	ע��: (1)���� HI_MPI_ISP_Init �� HI_MPI_ISP_Run ֮���ٵ��ñ��ӿ��˳� ISPfirmware��
		  (2)��֧�ֶ���̣�����Ҫ�� sensor_register_callback�� HI_MPI_AE_Register��HI_MPI_AWB_Register�� 
		  	 HI_MPI_ISP_MemInit�� HI_MPI_ISP_Init��HI_MPI_ISP_Run �ӿ���ͬһ�����̵��á�
		  (3)֧���ظ����ñ��ӿڣ�
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
	// ��ISP��ע��AE��
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
	��������: ��ISPע��AWB�⣻
	��������: IspDev(�豸��š������Ͳ���)��stLib(AWB�㷨��ṹ��ָ�룬����Ͳ���)
	����ֵ: 0��ʾ�ɹ�����0��ʾʧ��
	����: ͷ�ļ�(hi_comm_isp.h��mpi_awb.h)�����ļ�(libisp.a)
	ע��: (1)�ýӿڵ�����ISP���ṩ��AWB��ע��ص��ӿ�HI_MPI_ISP_AWBLibRegCallBack����ʵ��AWB��ISP 
			 �ⷴע��Ĺ��ܡ�
		  (2)�û����ô˽ӿ���ɺ�˼AWB����ISP�ⷴע�᣻
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
	��������: ��ISP��ע��AE�⣻
	��������: IspDev(�豸��š������Ͳ���)��stLib(AE�㷨��ṹ��ָ�룬����Ͳ���)
	����ֵ: 0��ʾ�ɹ�����0��ʾʧ�ܣ�
	����: ͷ�ļ�(hi_comm_isp.h��mpi_ae.h)�����ļ�(libisp.a��lib_hiae.a)
	ע��: �ýӿڵ�����ISP���ṩ��AE��ע��ص��ӿ� HI_MPI_ISP_AELibUnRegCallBack����ʵ��AE��ISP��
		  ��ע��Ĺ��ܡ�
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


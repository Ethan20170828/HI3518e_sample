/******************************************************************************
  Some simple Hisilicon Hi35xx video input functions.

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

/******************************************************************************
* function : Set vpss system memory location
******************************************************************************/
HI_S32 SAMPLE_COMM_VPSS_MemConfig()
{
    HI_CHAR * pcMmzName;
    MPP_CHN_S stMppChnVpss;
    HI_S32 s32Ret, i;

    /*vpss group max is 64, not need config vpss chn.*/
    for(i=0;i<64;i++)
    {
        stMppChnVpss.enModId  = HI_ID_VPSS;
        stMppChnVpss.s32DevId = i;
        stMppChnVpss.s32ChnId = 0;

        if(0 == (i%2))
        {
            pcMmzName = NULL;  
        }
        else
        {
            pcMmzName = "ddr1";
        }

        /*vpss*/
        s32Ret = HI_MPI_SYS_SetMemConf(&stMppChnVpss, pcMmzName);
        if (HI_SUCCESS != s32Ret)
        {
            SAMPLE_PRT("Vpss HI_MPI_SYS_SetMemConf ERR !\n");
            return HI_FAILURE;
        }
    }
    return HI_SUCCESS;
}


HI_S32 SAMPLE_COMM_VPSS_StartGroup(VPSS_GRP VpssGrp, VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    HI_S32 s32Ret;
    VPSS_NR_PARAM_U unNrParam = {{0}};

	// 对SAMPLE_COMM_VPSS_StartGroup函数第一个传参进行校验
    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        printf("VpssGrp%d is out of rang. \n", VpssGrp);
        return HI_FAILURE;
    }

	// 对SAMPLE_COMM_VPSS_StartGroup函数第二个传参进行校验
    if (HI_NULL == pstVpssGrpAttr)
    {
        printf("null ptr,line%d. \n", __LINE__);
        return HI_FAILURE;
    }

	/*********************************************************************************************
	函数功能: 创建一个VPSS GROUP
	函数传参: VpssGrp(VPSS GROUP号、离线模式的取值范围：[0, VPSS_MAX_GRP_NUM]、在线模式的取值范围：0)
			  pstVpssGrpAttr(VPSS GROUP属性指针，输出型参数)；
	返回值: 0表示成功；非0表示失败；
	需求: 头文件(hi_comm_vpss.h、mpi_vpss.h)、库文件(libmpi.a)
	*********************************************************************************************/
    s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, pstVpssGrpAttr);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

    /*** get vpss 3DNR param ***/
	/*********************************************************************************************
	函数功能: 获取VPSS 3DNR参数，降噪
	函数传参: VpssGrp(VPSS GROUP号，离线模式的取值范围:[0,VPSS_MAX_GRP_NUM]、在线模式的取值范围：0)
			  unNrParam(VPSS 3DNR 参数联合体体指针、输出型参数)
    返回值: 0表示成功，非0表示失败；
    需求: 头文件(hi_comm_vpss.h、mpi_vpss.h)、库文件(libmpi.a)；
    注意: GROUP必须已创建；
	*********************************************************************************************/
    s32Ret = HI_MPI_VPSS_GetNRParam(VpssGrp, &unNrParam);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }
    
    /*********************************************************************************************
    函数功能: 设置VPSS 3DNR参数；
    函数传参: VpssGrp(VPSS GROUP号、离线模式的取值范围：[0,VPSS_MAX_GRP_NUM]、在线模式的取值范围：0)
    		  unNrParam(VPSS 3DNR参数联合体指针、输出型参数)
    返回值: 0表示成功，非0表示失败；
    需求: 头文件(hi_comm_vpss.h、mpi_vpss.h)、库文件(libmpi.a)；
    注意: GROUP必须已创建；
    *********************************************************************************************/
    s32Ret = HI_MPI_VPSS_SetNRParam(VpssGrp, &unNrParam);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("failed with %#x!\n", s32Ret);
        return HI_FAILURE;
    }

	/*********************************************************************************************
	函数功能: 启用VPSS GROUP
	函数传参: VpssGrp(VPSS GROUP号、离线模式的取值范围：[0,VPSS_MAX_GRP_NUM]、在线模式的取值范围：0)
	返回值: 0表示成功，非0表示失败；
    需求: 头文件(hi_comm_vpss.h、mpi_vpss.h)、库文件(libmpi.a)；
    注意: (1)GROUP必须已创建；
    	  (2)重复调用该函数设置同一个组返回成功。
	*********************************************************************************************/
    s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}
                                   

HI_S32 SAMPLE_COMM_VPSS_EnableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn, 
                                                  VPSS_CHN_ATTR_S *pstVpssChnAttr,
                                                  VPSS_CHN_MODE_S *pstVpssChnMode,
                                                  VPSS_EXT_CHN_ATTR_S *pstVpssExtChnAttr)
{
    HI_S32 s32Ret;

    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        printf("VpssGrp%d is out of rang[0,%d]. \n", VpssGrp, VPSS_MAX_GRP_NUM);
        return HI_FAILURE;
    }

    if (VpssChn < 0 || VpssChn > VPSS_MAX_CHN_NUM)
    {
        printf("VpssChn%d is out of rang[0,%d]. \n", VpssChn, VPSS_MAX_CHN_NUM);
        return HI_FAILURE;
    }

    if (HI_NULL == pstVpssChnAttr && HI_NULL == pstVpssExtChnAttr)
    {
        printf("null ptr,line%d. \n", __LINE__);
        return HI_FAILURE;
    }

    if (VpssChn < VPSS_MAX_PHY_CHN_NUM)
    {
		/*****************************************************************************************
		函数功能: 设置VPSS通道属性；
		函数传参: VpssGrp(VPSS GROUP号，离线模式的取值范围： [0, VPSS_MAX_GRP_NUM)、在线模式的取值范围： 0)
				  VpssChn(VPSS Chn号，取值范围： [0, VPSS_MAX_PHY_CHN_NUM))；
				  pstVpssChnAttr(VPSS通道属性，输出型参数)
		返回值: 0表示成功；非0表示失败；
		需求: 头文件(hi_comm_vpss.h、mpi_vpss.h)、库文件(libmpi.a)
		注意: GROUP必须已创建；扩展通道不支持此接口；
		*****************************************************************************************/
        s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, pstVpssChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }
    }
    else
    {
		/*****************************************************************************************
		函数功能: 设置VPSS扩展通道属性，扩展通道的主要应用是进行二次缩放和帧率控制；
		函数传参: VpssGrp(VPSS GROUP号，离线模式的取值范围： [0, VPSS_MAX_GRP_NUM)、在线模式的取值范围： 0)
				  VpssChn(VPSS扩展通道号，取值范围:[VPSS_MAX_PHY_CHN_NUM,VPSS_MAX_CHN_NUM))
				  pstVpssExtChnAttr(VPSS扩展通道属性，输出型参数)
		返回值: 0表示成功；非0表示失败；
		需求: 头文件(hi_comm_vpss.h、mpi_vpss.h)、库文件(libmpi.a)
		注意: (1)扩展通道的输入绑定源 必须为物理通道，宽、高需要满足2对齐，SrcFrameRate必须大于等于
				 DstFrameRate，若都设置成-1，表示不进行帧率控制。
			  (2)扩展通道以绑定的方式连接物理通道，每个扩展通道最多绑定一个物理通道；
			  (3)多个扩展通道可以绑定到同一个物理通道，若源物理通道绑定的扩展通大道达到最大数目，
			     设置将会失败；
			  (4)GROUP必须已创建；
			  (5)Hi3519V100 宽度超过 4096 时，不支持压缩；
		*****************************************************************************************/
        s32Ret = HI_MPI_VPSS_SetExtChnAttr(VpssGrp, VpssChn, pstVpssExtChnAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("%s failed with %#x\n", __FUNCTION__, s32Ret);
            return HI_FAILURE;
        }
    }
    
    if (VpssChn < VPSS_MAX_PHY_CHN_NUM && HI_NULL != pstVpssChnMode)
    {
		/*****************************************************************************************
		函数功能: 设置VPSS通道工作模式。USER模式主要用于通道一绑多，即一个通道绑定多个输出源；在USER
				  模式下，VPSS后端可以不用绑定输出源，而通过获取图像接口获取图像。AUTO模式主要用于一般
				  场景，对后端自适应。
		函数传参: VpssGrp(VPSS GROUP号,离线模式的取值范围： [0, VPSS_MAX_GRP_NUM)。在线模式的取值范围：0)
				  VpssChn(VPSS 通道号。取值范围： [0, VPSS_MAX_PHY_CHN_NUM)。)
				  pstVpssChnMode(通道工作模式信息，具体限制请参见VPSS_CHN_MODE_S 结构。输出型参数)
		返回值: 0表示成功；非0表示失败；
		需求: 头文件(hi_comm_vpss.h、mpi_vpss.h)、库文件(libmpi.a)
		注意: (1)GROUP必须已建立；
			  (2)CHN号必须在取值范围内；
			  (3)默认为AUTO模式，工作时，可在两种模式间动态切换；
			  (4)扩展通道不支持工作模式设置；
			  (5)Hi3519V100宽度超过4096时，不支持压缩；
		*****************************************************************************************/
        s32Ret = HI_MPI_VPSS_SetChnMode(VpssGrp, VpssChn, pstVpssChnMode);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("%s failed with %#x\n", __FUNCTION__, s32Ret);
            return HI_FAILURE;
        }     
    }

	/*********************************************************************************************
	函数功能: 启用VPSS通道；
	函数传参: VpssGrp(VPSS GROUP号。离线模式的取值范围： [0, VPSS_MAX_GRP_NUM)。在线模式的取值范围：0。)
			  VpssChn(VPSS Chn 号。取值范围： [0, VPSS_MAX_CHN_NUM)。)
	返回值: 0表示成功；非0表示失败；
	需求: 头文件(hi_comm_vpss.h、mpi_vpss.h)、库文件(libmpi.a)；
	注意: (1)多次使能返回成功；
		  (2)GROUP必须已建立；
		  (3)若支持扩展通道，扩展通道必须保证此通道绑定的源物理已经使能，否则返回失败错误码；
		  (4)物理通道时，必须先调用 HI_MPI_VPSS_SetChnMode，设置通道工作模式，否则返回失败错误码；
	*********************************************************************************************/
    s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("HI_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPSS_StopGroup(VPSS_GRP VpssGrp)
{
    HI_S32 s32Ret;

    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        printf("VpssGrp%d is out of rang[0,%d]. \n", VpssGrp, VPSS_MAX_GRP_NUM);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_StopGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("%s failed with %#x\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }

    s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("%s failed with %#x\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}

HI_S32 SAMPLE_COMM_VPSS_DisableChn(VPSS_GRP VpssGrp, VPSS_CHN VpssChn)
{
    HI_S32 s32Ret;

    if (VpssGrp < 0 || VpssGrp > VPSS_MAX_GRP_NUM)
    {
        printf("VpssGrp%d is out of rang[0,%d]. \n", VpssGrp, VPSS_MAX_GRP_NUM);
        return HI_FAILURE;
    }

    if (VpssChn < 0 || VpssChn > VPSS_MAX_CHN_NUM)
    {
        printf("VpssChn%d is out of rang[0,%d]. \n", VpssChn, VPSS_MAX_CHN_NUM);
        return HI_FAILURE;
    }
    
    s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
    if (s32Ret != HI_SUCCESS)
    {
        SAMPLE_PRT("%s failed with %#x\n", __FUNCTION__, s32Ret);
        return HI_FAILURE;
    }

    return HI_SUCCESS;
}



/*****************************************************************************
* function : start vpss. VPSS chn with frame
*****************************************************************************/
HI_S32 SAMPLE_COMM_VPSS_Start(HI_S32 s32GrpCnt, SIZE_S *pstSize, HI_S32 s32ChnCnt,VPSS_GRP_ATTR_S *pstVpssGrpAttr)
{
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;
    VPSS_GRP_ATTR_S stGrpAttr = {0};
    VPSS_CHN_ATTR_S stChnAttr = {0};
    VPSS_NR_PARAM_U unNrParam = {{0}};
    HI_S32 s32Ret;
    HI_S32 i, j;

    /*** Set Vpss Grp Attr ***/

    if(NULL == pstVpssGrpAttr)
    {
        stGrpAttr.u32MaxW = pstSize->u32Width;
        stGrpAttr.u32MaxH = pstSize->u32Height;
        stGrpAttr.bIeEn = HI_FALSE;
        stGrpAttr.bNrEn = HI_TRUE;
        stGrpAttr.bHistEn = HI_FALSE;
        stGrpAttr.enDieMode = VPSS_DIE_MODE_NODIE;
        stGrpAttr.enPixFmt = SAMPLE_PIXEL_FORMAT;
    }
    else
    {
        memcpy(&stGrpAttr,pstVpssGrpAttr,sizeof(VPSS_GRP_ATTR_S));
    }
    

    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
        /*** create vpss group ***/
        s32Ret = HI_MPI_VPSS_CreateGrp(VpssGrp, &stGrpAttr);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VPSS_CreateGrp failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        /*** set vpss param ***/
        s32Ret = HI_MPI_VPSS_GetNRParam(VpssGrp, &unNrParam);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }        
        
        s32Ret = HI_MPI_VPSS_SetNRParam(VpssGrp, &unNrParam);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }

        /*** enable vpss chn, with frame ***/
        for(j=0; j<s32ChnCnt; j++)
        {
            VpssChn = j;
            /* Set Vpss Chn attr */
            stChnAttr.bSpEn = HI_FALSE;
            stChnAttr.bBorderEn = HI_TRUE;
            stChnAttr.stBorder.u32Color = 0xff00;
            stChnAttr.stBorder.u32LeftWidth = 2;
            stChnAttr.stBorder.u32RightWidth = 2;
            stChnAttr.stBorder.u32TopWidth = 2;
            stChnAttr.stBorder.u32BottomWidth = 2;
            
            s32Ret = HI_MPI_VPSS_SetChnAttr(VpssGrp, VpssChn, &stChnAttr);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("HI_MPI_VPSS_SetChnAttr failed with %#x\n", s32Ret);
                return HI_FAILURE;
            }
    
            s32Ret = HI_MPI_VPSS_EnableChn(VpssGrp, VpssChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("HI_MPI_VPSS_EnableChn failed with %#x\n", s32Ret);
                return HI_FAILURE;
            }
        }

        /*** start vpss group ***/
        s32Ret = HI_MPI_VPSS_StartGrp(VpssGrp);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("HI_MPI_VPSS_StartGrp failed with %#x\n", s32Ret);
            return HI_FAILURE;
        }

    }
    return HI_SUCCESS;
}

/*****************************************************************************
* function : disable vi dev
*****************************************************************************/
HI_S32 SAMPLE_COMM_VPSS_Stop(HI_S32 s32GrpCnt, HI_S32 s32ChnCnt)
{
    HI_S32 i, j;
    HI_S32 s32Ret = HI_SUCCESS;
    VPSS_GRP VpssGrp;
    VPSS_CHN VpssChn;

    for(i=0; i<s32GrpCnt; i++)
    {
        VpssGrp = i;
        s32Ret = HI_MPI_VPSS_StopGrp(VpssGrp);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
        for(j=0; j<s32ChnCnt; j++)
        {
            VpssChn = j;
            s32Ret = HI_MPI_VPSS_DisableChn(VpssGrp, VpssChn);
            if (s32Ret != HI_SUCCESS)
            {
                SAMPLE_PRT("failed with %#x!\n", s32Ret);
                return HI_FAILURE;
            }
        }
    
        s32Ret = HI_MPI_VPSS_DestroyGrp(VpssGrp);
        if (s32Ret != HI_SUCCESS)
        {
            SAMPLE_PRT("failed with %#x!\n", s32Ret);
            return HI_FAILURE;
        }
    }

    return HI_SUCCESS;
}

#ifdef __cplusplus
#if __cplusplus
}
#endif
#endif /* End of #ifdef __cplusplus */

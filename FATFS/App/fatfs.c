/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */

const char *Fs_ErrorCode[] = {
	"FR_OK",				/* (0) Succeeded */
	"FR_DISK_ERR",			/* (1) A hard error occurred in the low level disk I/O layer */
	"FR_INT_ERR",				/* (2) Assertion failed */
	"FR_NOT_READY",			/* (3) The physical drive cannot work */
	"FR_NO_FILE",				/* (4) Could not find the file */
	"FR_NO_PATH",				/* (5) Could not find the path */
	"FR_INVALID_NAME",		/* (6) The path name format is invalid */
	"FR_DENIED",				/* (7) Access denied due to prohibited access or directory full */
	"FR_EXIST",				/* (8) Access denied due to prohibited access */
	"FR_INVALID_OBJECT",		/* (9) The file/directory object is invalid */
	"FR_WRITE_PROTECTED",		/* (10) The physical drive is write protected */
	"FR_INVALID_DRIVE",		/* (11) The logical drive number is invalid */
	"FR_NOT_ENABLED",			/* (12) The volume has no work area */
	"FR_NO_FILESYSTEM",		/* (13) There is no valid FAT volume */
	"FR_MKFS_ABORTED",		/* (14) The f_mkfs() aborted due to any problem */
	"FR_TIMEOUT",				/* (15) Could not get a grant to access the volume within defined period */
	"FR_LOCKED",				/* (16) The operation is rejected according to the file sharing policy */
	"FR_NOT_ENOUGH_CORE",		/* (17) LFN working buffer could not be allocated */
	"FR_TOO_MANY_OPEN_FILES",	/* (18) Number of open files > _FS_LOCK */
	"FR_INVALID_PARAMETER"	/* (19) Given parameter is invalid */
};
/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */
void FS_Ready(void)
{
    uint8_t ret = 0;
    BYTE work[_MAX_SS];
    
    ret = f_mount(&SDFatFS,SDPath,1);   //mount disk
    if(ret == FR_NO_FILESYSTEM) //if no filesystem, format it and creat a filesystem
    {    
        ret = f_mkfs(SDPath,NULL,0, work,sizeof(work));
        if(ret == FR_OK)
        {
            U_LOG("fatfs mkfs");
            ret = f_mount(NULL, SDPath, 1);       //cancel mount after formating
            ret = f_mount(&SDFatFS, SDPath, 1);     // mounnt again
        }
        else // format fail
        {
            U_LOG("Description Failed to format the SD card...%s",Fs_ErrorCode[ret]);
            return;
        }
    }
    else if(ret != FR_OK)// mount fail
    {
        U_LOG("Mount failure=%s",Fs_ErrorCode[ret]);
        return;
    }
    else
    {
        U_LOG("FATFS Mount Success");
    }
}

void FS_ListPath(TCHAR* path)
{
    FRESULT res;
    FILINFO fno;             /* 文件信息结构体对象 */
    DIR dir;                 /* 目录read操作对象 */

    res= f_opendir(&dir, path);
    if (res != FR_OK)
    {
        U_LOG("Open %s fail : %s", path, Fs_ErrorCode[res]);
    }
    for (;;)
    {
        res = f_readdir(&dir, &fno); /* 读取目录项 */
        if (res != FR_OK || fno.fname[0] == 0) break; /* 结束条件 */
        if (fno.fname[0] == '.') continue; /* 忽略当前目录和上级目录 */
        // 输出文件名
        U_LOG("%s", fno.fname);
    }
}
/* USER CODE END Application */

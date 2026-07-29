#ifndef __USBLIB_H__
#define __USBLIB_H__

#if (defined(_WIN32) || defined(_WIN64))
    #ifdef USBLIB_EXPORTS
        #define USBSDK_API __declspec(dllexport)
    #else
        #define USBSDK_API __declspec(dllimport)
    #endif
    #define CALLMETHOD __stdcall
    #define CALLBACK   __stdcall
#else
    #define CALLMETHOD
    #define CALLBACK
    #define USBSDK_API //extern "C"
#endif

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#else
#ifdef __OBJC__
#include "objc/objc.h"
#else
#define BOOL        int
#endif
#define BYTE		unsigned char
#define PBYTE		BYTE*
#define LPBYTE		BYTE*
#define PUCHAR		BYTE*
#ifndef LONG
#define LONG		int
#endif
#ifndef DWORD
#define DWORD		unsigned long
#endif
#define WORD		unsigned short
#define COLORREF	DWORD
#define HDC			void*
#define HWND		void*
#define LPSTR		char*
#define UINT		unsigned int
#define TRUE		1
#define FALSE		0
#define ULONGLONG	unsigned long long
#define LONGLONG	long long

typedef struct _SYSTEMTIME
{
    WORD wYear;
    WORD wMonth;
    WORD wDayOfWeek;
    WORD wDay;
    WORD wHour;
    WORD wMinute;
    WORD wSecond;
    WORD wMilliseconds;
} SYSTEMTIME, *PSYSTEMTIME, *LPSYSTEMTIME;
#endif

#ifdef	__cplusplus
extern "C" {
#endif

/***********************************************************************************************/
//> 结构体及枚举定义                                                                               
/***********************************************************************************************/

//设备信息
typedef struct
{
	void*			usbDev;				//设备指针
	unsigned int	nDeviceAddress;		//设备地址
	unsigned char	deviceName[256];	//设备名称
	unsigned char	deviceSerialNumber;	//设备序列号
	unsigned char	res[15];
}USBSDK_DeviceInfo;

///测温项类型
typedef enum USBSDK_RADIOMETRY_METERTYPE 
{
	USBSDK_RADIOMETERY_SPOTMETER,                 //点测温
	USBSDK_RADIOMETERY_LINEMETER,				  //线测温
	USBSDK_RADIOMETERY_AREAMETER				  //区域测温
}USBSDK_RADIOMETRY_METERTYPE;

/* 测温局部参数 32字节 */
typedef struct USBSDK_LocalParam
{
	unsigned int  enable;   /* 使能 */
	float		  objectDistance;
	float		  refalectedTemp;
	float		  objectEmissivity;
	unsigned int  res[4];
}USBSDK_LocalParam;

/// 点，8字节
typedef struct USBSDK_Point
{
	int x;
	int y;
}USBSDK_Point;

/* 测温区域 68字节 */
typedef struct USBSDK_Area
{
	unsigned int  pointNumber;
	USBSDK_Point   points[8];
}USBSDK_Area;

/// 直线，16字节
typedef struct USBSDK_Line
{
	USBSDK_Point start;
	USBSDK_Point end;
}USBSDK_Line;

/* 点、线、区域坐标 68字节 */
typedef union
{
	USBSDK_Point           spotPoint;	    /* 点测温坐标 */
	USBSDK_Area			   areaRegion;      /* 区域测温范围 */
	USBSDK_Line			   linePostion;     /* 线测温位置 */
}USBSDK_RegionConfig;

///区域测温子类型
typedef enum USBSDK_RADIOMETRY_AREASUBTYPE 
{
	USBSDK_RADIOMETERY_AREAPOLYGON,            //多边形
	USBSDK_RADIOMETERY_AREARECT,               //矩形区域
	USBSDK_RADIOMETERY_AREAELLIPSE             //椭圆区域
}USBSDK_RADIOMETRY_AREASUBTYPE;

/* 测温单项规则配置 144字节 */
typedef struct USBSDK_RuleInfo
{
	unsigned int                        ruleId;			/* 测温项编号 编号从1开始 */
	unsigned int                        enable;			/* 使能项 */
	USBSDK_RADIOMETRY_METERTYPE			meterType;		/* 测温项类型 */
	USBSDK_LocalParam					localParam;		/* 局部参数 */
	USBSDK_RegionConfig					regionConfig;	/* 点、线、区域坐标 [0,8192) */
	USBSDK_RADIOMETRY_AREASUBTYPE		subType;		/* 区域测温子类型，只有区域测温类型时有效 */
	char								ruleName[16];	/* 规则名称 */
	unsigned int                        res[3];
}USBSDK_RuleInfo;

/* 测温规则配置 1156字节 */
typedef struct USBSDK_Rules
{
	unsigned int				count;			/* 测温单项规则个数 */
	USBSDK_RuleInfo				Rules[12];		/* 具体的规则 */
}USBSDK_Rules;

/* 某测温项对应温度 64字节 */
typedef struct USBSDK_RuleTemper
{
	USBSDK_RADIOMETRY_METERTYPE meterType;	/* 测温项类型 */
	unsigned int  ruleId;                   /* 配置项编号 */
	unsigned int  resultTypeMask;
	float temperatureAve;					/* 平均温度 */
	float temperatureStd;					/* 标准方差温度 */
	float temperatureMax;					/* 最高温度 */
	float temperatureMin;					/* 最低温度 */
	float temperatureMid;					/* 中间温度 */
	USBSDK_Point   hotPoint;				/* 热点坐标[0,8192) */
	USBSDK_Point   coldPoint;			    /* 冷点坐标[0,8192)	*/
	unsigned int  res[4];
} USBSDK_RuleTemper;

/* 获取冷热点 */
typedef struct USBSDK_HotAndColdInfo
{
	float			hotTemper;              /* 热点温度 */
	USBSDK_Point	hotPoint;				/* 热点坐标[0,8192) */
	float			coldTemper;             /* 冷点温度 */
	USBSDK_Point	coldPoint;			    /* 冷点坐标[0,8192)	*/
	unsigned int	res[4];
} USBSDK_HotAndColdInfo;

typedef struct USBSDK_RadCommonTemparam
{
	unsigned int relativeHumidity;			//相对湿度    百分比表示0%~100%
	float atmosphericTemperature;           //大气温度，精度到°
	float objectEmissivity;					//目标辐射系数
	float objectDistance;					//距离，精度0.1米
	float reflectedTemperature;             //反射温度
}USBSDK_RadCommonTemparam;

typedef struct
{
	unsigned int      enable;      /*等温线使能开关*/
	int       minLimitTemp;        /*等温线下限温度值*/
	int       mediumTemp;          /*等温线中位温度值*/
	int       maxLimitTemp;        /*等温线上限温度值*/
	int       saturationTemp;      /*等温线饱和温度值*/
} USBSDK_IsothermValue;

/****第二版等温线结构体*****/
typedef struct
{
	unsigned int	Enable;              /*等温线使能开关*/
	float           PseudoColorY16[2];   /* 用于vps伪彩显示的Y16值，[0]是下限值,[1]是上限值,Y16[1]需要大于Y16[0]，XS51x0不支持*/
	unsigned int    PseudoMode;          /* 伪彩工作模式 0:上限,1:下限,2:区间,3:两端,4:无 */
}USBSDK_IsothermParams;

//二次标定时间和结果
typedef struct
{
	//低温档标定结果
	int lowTemperCaliResult;						//标定结果 1:通过 0:不通过	其他:未知		
	char lowTemperCaliResultTime[32];				//标定完成时间
	//高温档标定结果
	int highTemperCaliResult;						//标定结果 1:通过 0:不通过	其他:未知		
	char highTemperCaliResultTime[32];				//标定完成时间
	//低温二档标定结果
	int lowTemperSecondGearCaliResult;				//标定结果 1:通过 0:不通过	其他:未知		
	char lowTemperSecondGearCaliResultTime[32];		//标定完成时间
	unsigned int res[12];
}USBSDK_TemperatureCalibraResult;

/* 二次标定黑体温度 */
typedef struct
{
	int lowBlackbodytemp;	//二次标定低温黑体温度,单位摄氏度
	int highBlackbodytemp;	//二次标定高温黑体温度,单位摄氏度
	int res[3];
} USBSDK_TempCaliBlackBodyTemp;

//二次标定系数
typedef struct 
{
	float fHighTempCoff;  //二次标定高温档温度补偿系数
	float fLowTempCoff;   //二次标定低温档温度补偿系数
}USBSDK_TempCaliCofficient;

/* 机芯分辨率类型 */
typedef enum
{
	RESOLUTION_640,
	RESOLUTION_400,
	RESOLUTION_256,
} USBSDK_Resolution;

/*全局像素点温度*/
typedef struct USBSDK_GlobalHeatmapInfo
{
	USBSDK_Resolution   emtype;             ///<机芯分辨率类型
	unsigned int		unWidth;			///< 图像宽度
	unsigned int		unHeight;           ///< 图像高度
	unsigned int		unSizeMap;			///< 热图数据大小，raw数据一般为width*height*2
	int					nReserved[3];       ///< 保留
}USBSDK_GlobalHeatmapInfo;

/*防灼伤开关和挡片时间*/
typedef struct USBSDK_BurnParams
{
	unsigned int  closeShutterTime;	//关闭挡片时间，UTC时间
	unsigned char burnSwitch;		//防灼烧开关 1-开 0-关
	unsigned char isBurnStatus;		//当前是否防灼伤
	unsigned char res[2];
}USBSDK_BurnParams;

//DeltaB 保存时间和次数
typedef struct USBSDK_DeltaBSaveCountAndTime
{
	unsigned int lowTemperCount;	//次数
	unsigned int lowTemperTime;		//时间
	unsigned int res[6];			//保留位
}USBSDK_DeltaBSaveCountAndTime;

/***********************************************************************************************/
//> 常量定义                                                                                     
/***********************************************************************************************/

/* 数据长度 */
#define DATA_MAX_BIT				4000
#define BADPIX_MAX					256
#define TCAM_DATA_BUF_LEN_MAX		8*1024
#define TCAM_IMAGE_BUF_LEN_MAX		32*1024
#define TCAM_CMD_BUF_LEN_MAX		64

//时间宏定义
#define TIMEOUT_500MS	500
#define TIMEOUT_5S		5000
#define TIMEOUT_10S		10000
#define TIMEOUT_15S		15000
#define TIMEOUT_20S		20000
#define TIMEOUT_2S		2000
#define TIMEOUT_1S		1000
#define TIMEOUT_30S		30000
#define TIMEOUT_50S		50000
#define TIMEOUT_70S		70000
#define TIMEOUT_1MIN	60000
#define TIMEOUT_2MIN	120000
#define TIMEOUT_5MIN	TIMEOUT_1MIN*5

/* 错误码 */
#define  USB_SUCCESS								0	//成功
#define	 USB_ERROR_IO								1	//输入输出口错误
#define  USB_ERROR_INVALID_PARAM					2   //无效参数
#define  USB_ERROR_ACCESS							3   //访问被拒绝（权限不足）
#define  USB_ERROR_NO_DEVICE						4	//没有该设备
#define  USB_ERROR_NOT_FOUND						5	//找不到实体
#define  USB_ERROR_BUSY								6	//资源繁忙
#define  USB_ERROR_TIMEOUT							7	//操作超时
#define  USB_ERROR_OVERFLOW							8	//内存溢出
#define  USB_ERROR_PIPE								9	//管道错误
#define  USB_ERROR_INTERRUPTED						10	//系统调用中断(可能是由于信号原因)
#define  USB_ERROR_NO_MEM							11	//buf内存不足
#define  USB_ERROR_NOT_SUPPORTED					12	//此平台不支持或未实现操作
#define	 USB_ERROR_NOT_READY						13  //机芯未就绪
#define  USB_ERROR_CHECKSUM							14  //校验失败
#define  USB_ERROR_UNDEFINED_PROCESS				15  //未知处理码
#define  USB_ERROR_UNDEFINED_FLIR					16  //未知命令码
#define  USB_ERROR_BYTE_COUNT						17	//命令字节数错误
#define  USB_ERROR_FEATURE_NOENABLED				18  //机芯属性未使能
#define  USB_ERROR_UNDEFINED_COMMAND				19	//未知机芯命令码
#define  USB_ERROR_CFG								20  //机芯配置操作失败
#define  USB_ERROR_FLASH							21  //机芯flash操作失败
#define  USB_ERROR_UART								22  //机芯串口操作失败
#define  USB_ERROR_ALGORITHM						23  //机芯算法操作失败
#define  USB_ERROR_MEMORY							24  //机芯内存操作失败
#define  USB_ERROR_UPDATE							25  //升级失败
#define  USB_ERROR_NO_CONTROLPOINT					26	//无控制端点，无法发送和接收命令
#define  USB_ERROR_DEVICE_OPEN						27	//设备已开启，如要连接前先断开设备
#define  USB_ERROR_DEVICE_NOOPEN					28  //设备未连接
#define  USB_ERROR_SEND								29	//USB命令发送失败
#define  USB_ERROR_RECEIVE							30  //USB命令接收失败
#define  USB_ERROR_MATCH							31  //USB发送和接收命令不匹配
#define  USB_ERROR_COLOR_RANGE						32  //伪彩范围超限
#define  USB_ERROR_PART_NUMBER						33  //机芯型号错误
#define  USB_ERROR_SOFTVERSION_LEN					34  //获取软件版本长度错误
#define  USB_ERROR_FILEPATH_NULL					35  //文件路径为空
#define  USB_ERROR_OPENFILE							36  //打开文件失败
#define  USB_ERROR_FILESIZE							37  //文件大小错误
#define  USB_ERROR_THERMAL_INIT						38  //测温初始化失败
#define  USB_ERROR_THERMAL_GAIN						39  //设置测温增益模式失败
#define  USB_ERROR_THERMAL_SETRULE					40  //设置测温规则失败
#define  USB_ERROR_THERMAL_GETTEMPERINFO			41  //获取测温数据失败
#define  USB_ERROR_THERMAL_GETHOTCLODINFO			42  //获取冷热点失败
#define  USB_ERROR_THERMAL_SETGGREYDIFF				43  //设置灰度差值表失败
#define  USB_ERROR_THERMAL_SETCOMPENSATION			44  //设置测温补偿值失败
#define  USB_ERROR_THERMAL_SETDIFFTABLECOMPEN		45  //设置灰度差值补偿表失败
#define  USB_ERROR_THERMAL_METRYOPTIONS				46  //设置测温全局参数失败
#define  USB_ERROR_THERMAL_TECHIGHCOMPENSATION		47  //设置高温段参数失败
#define  USB_ERROR_THERMAL_AUTOCALIBRATIONVER		48  //设置测温自动化版本失败
#define  USB_ERROR_THERMAL_TECCOEF					49  //设置环境补偿系数失败
#define  USB_ERROR_THERMAL_DEVINFO					50  //设置机芯参数失败
#define  USB_ERROR_THERMAL_FOCAL					51  //设置镜头焦距失败
#define  USB_ERROR_THERMAL_DISTANCECOMB				52  //设置距离补偿参数B失败
#define  USB_ERROR_THERMAL_TEMPPARAM				53  //设置温度传感器温度信息失败
#define  USB_ERROR_THERMAL_BASEGRAY					54	//设置测温基准灰度失败
#define  USB_ERROR_DEVICE_BUSY						55  //设备繁忙，上一条发送协议还未结束，请稍后再试
#define  USB_ERROR_THERMAL_SETCORRECTTHRESHOLD		56  //设置测温温度校正阈值失败
#define  USB_ERROR_THERMAL_GETCORRECTRESULT			57  //获取测温温度校正结果失败
#define  USB_ERROR_THERMAL_GETGGREYDIFF				58  //获取灰度差值表失败
#define  USB_ERROR_THERMAL_GETCOMPENSATION			59  //获取测温补偿值失败
#define  USB_ERROR_THERMAL_GETDIFFTABLECOMPEN		60  //获取灰度差值补偿表失败
#define  USB_ERROR_THERMAL_GETMETRYOPTIONS			61  //获取测温全局参数失败
#define  USB_ERROR_THERMAL_GETTECHIGHCOMPENSATION	62  //获取高温段参数失败
#define  USB_ERROR_THERMAL_GETAUTOCALIBRATIONVER	63  //获取测温自动化版本失败
#define  USB_ERROR_THERMAL_GETTECCOEF				64  //获取环境补偿系数失败
#define  USB_ERROR_THERMAL_GETDEVINFO				65  //获取机芯参数失败
#define  USB_ERROR_THERMAL_GETFOCAL					66  //获取镜头焦距失败
#define  USB_ERROR_THERMAL_GETDISTANCECOMB			67  //获取距离补偿参数B失败
#define  USB_ERROR_THERMAL_GETTEMPPARAM				68  //获取温度传感器温度信息失败
#define  USB_ERROR_THERMAL_GETBASEGRAY				69	//获取测温基准灰度失败
#define  USB_ERROR_THERMAL_GETGAIN					70  //获取测温档位失败
#define  USB_ERROR_NO_PART_NUMBER					71  //无该机芯型号信息，获取分辨率失败
#define  USB_ERROR_DEVICE_NOSUPPORT					72  //设备不支持
#define  USB_ERROR_UPGRADEFILE_CHECK				73  //升级文件校验失败
#define  USB_ERROR_THERMAL_FUNCTION_UNOPENED		74  //测温功能未开启
#define  USB_ERROR_THERMAL_GETNEWDISTPARAMS			75	//获取新距离补偿参数失败
#define  USB_ERROR_THERMAL_SETNEWDISTPARAMS			76	//设置新距离补偿参数失败
#define  USB_ERROR_THERMAL_GETTHERMCALIDISTPARAMS	77	//获取测温标定距离参数失败
#define  USB_ERROR_THERMAL_SETTHERMCALIDISTPARAMS	78	//设置测温标定距离参数失败
#define  USB_ERROR_OTHER							99  //其他错误					

/****************************************************************************************
*
*
*                                       通用功能
*
*
****************************************************************************************/

/************************************************************************/
//> 基础接口
/************************************************************************/

/**
* @brief  获取USB SDK主版本号，次版本号和SVN版本号。
* @return 最高1位表示当前的主版本号；第2~3位表示次版本号；其余的表示SVN版本号。
*         如：返回值34033722表示：主版本号是3，次版本号是40，SVN版本号是33722。
*/
USBSDK_API DWORD CALLMETHOD USBSDK_GetSdkVersion();

/**
* @brief  初始化USB环境
* @return 返回说明
*		-<em>FALSE</em> 失败,可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  初始化正常
*/
USBSDK_API BOOL CALLMETHOD USBSDK_Init();

/**
* @brief  释放USB环境
* @return 返回说明
*		-<em>FALSE</em> 失败,可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  去初始化正常
*/
USBSDK_API BOOL CALLMETHOD USBSDK_Uninit();

/**
* @brief  获取错误码
* @return 错误码
*/
USBSDK_API int CALLMETHOD USBSDK_GetLastError();

/************************************************************************/
//> 设备控制
/************************************************************************/

/**
* @brief 是否采用V4L2拉流方式,在获取设备前设置
* @param[in] bV4L2	true:V4L2
* @return 返回说明
*		-<em>FALSE</em> 失败,可通过USBSDK_GetLastError()获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetStreamMode(bool bV4L2);

/**
* @brief 获取当前设备列表，只包含摄像头设备
* @param[out] devInfoList 返回数据存储的列表
* @return 返回个数,返回<=0表示接口调用失败，可通过USBSDK_GetLastError获取错误码
*/
USBSDK_API int CALLMETHOD USBSDK_GetDevInfoList(USBSDK_DeviceInfo* devInfoList);

/**
* @brief 打开指定设备
* @param[in] devInfo 设备信息
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_Open(USBSDK_DeviceInfo* devInfo);

/**
* @brief 关闭指定设备
* @return 返回说明
*		-<em>FALSE</em> 关闭失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  关闭成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_Close();

/**
* @brief 设备是否连接
* @return 返回说明
*		-<em>FALSE</em> 未连接
*		-<em>TRUE</em>  已连接
*/
USBSDK_API BOOL CALLMETHOD USBSDK_Connect();

/************************************************************************/
//> 设备码流
/************************************************************************/

/**
* @brief YUV码流和RAW数据回调
* @param pYUVBuffer		YUV数据
* @param nYUVBufLen		YUV数据大小
* @param pRAWBuffer		RAW数据
* @param nRawBufLen		RAW数据大小
* @param nWidth			分辨率-宽
* @param nHeight		分辨率-高
* @param dwUser			用户数据
*/
typedef void(CALLBACK *fUSBSDKDataCallBack)(unsigned char* pYUVBuffer, int nYUVBufLen, unsigned char* pRAWBuffer, int nRawBufLen, int nWidth, int nHeight, DWORD dwUser);

/**
* @brief 开启实时码流
* @param[in] cbTIUSBDataCallBack		回调函数
* @param[in] dwUser						用户数据
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_StartRealPlay(fUSBSDKDataCallBack cbTIUSBDataCallBack = 0, DWORD dwUser = 0);

/**
* @brief 关闭码流
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_CloseRealPlay();

/**
* @brief 获取码流数据, cbTIUSBDataCallBack=0时,通过此接口获取单帧YUV
* @param[out] pBuffer 数据
* @param[in]  nBufLen 数据buf长度
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetYuvData(unsigned char* pBuffer, int nBufLen);

/**
* @brief 获取帧率
* @return 返回说明
*		-<em>x</em> 帧率
*/
USBSDK_API int CALLMETHOD USBSDK_GetFps();

/************************************************************************/
//> 设备升级
/************************************************************************/

/**
* @brief 主程序升级
* @param[in] pFilePath		文件完整路径和名称
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_UpdateMainProgram(char* pFilePath);

/**
* @brief 获取升级进度
* @param[out] maxFileLen	总大小
* @param[out] nOffset		已升级大小
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetProgress(int *maxFileLen, int *nOffset);

/************************************************************************/
//> 设备功能
/************************************************************************/

/**
* @brief 获取设备类型
* @return 返回说明
*		-<em>0</em>  观测机芯
*		-<em>1</em>  测温机芯
*/
USBSDK_API int CALLMETHOD USBSDK_GetDeviceType();

/**
* @brief FFC快门校正，俗称打挡片
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_ShutterCorrect();

/**
* @brief 获取挡片状态
* @param[none]
* @return 返回说明
*		-<em>FALSE</em> 挡片静止
*		-<em>TRUE</em>  挡片活动
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetShutterStatus();

/**
* @brief 获取校正模式
* @param[out]  nValue	校正模式，0表示手动模式，1表示自动模式
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetFFCMode(int* nValue);

/**
* @brief 设置校正模式
* @param[in]  nValue	校正模式，0表示手动模式，1表示自动模式
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetFFCMode(int nValue);

/**
* @brief 获取校正周期
* @param[out]  nValue	校正周期，单位是秒，范围10~1200
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetFFCTime(int* nValue);

/**
* @brief 设置校正周期
* @param[in]  nValue	校正周期，单位是秒，范围10~1200
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetFFCTime(int nValue);

/**
* @brief 恢复出厂设置
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_FactoryDefault();

/**
* @brief 重启
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_Reboot();

/**
* @brief 获取分辨率
* @param[out]	nWidth  宽
* @param[out]	nHeight 高
* @return
*		-<em>FALSE</em> 失败,可通过USBSDK_GetLastError()获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetDevResolution(int* nWidth, int* nHeight);

/**
* @brief 获取机芯型号
* @param[out] buf 机芯型号buf
* @param[in]  len buf长度
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetPartNumber(char *buf,int len);

/**
* @brief 获取序列号
* @param[out] buf 序列号buf
* @param[in]  len buf长度
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetSerialNumber(char *buf, int len);

/**
* @brief 获取软件版本
* @param[out] buf 软件版本buf
* @param[in]  len buf长度
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetSoftwareVersion(char *buf, int len);

/**
* @brief 设置十字准心
* @param[in]  nEnable	开关十字准心	0:关闭 1:开启
* @param[in]  nColor	准心颜色		0:绿 1:蓝 2:红，只能设置0、1、2,否则会配置失败
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetCrossCursorParams(int nEnable, int nColor);

/**
* @brief 获取十字准心
* @param[out]  nEnable	 开关十字准心	0:关闭 1:开启
* @param[out]  nColor    准心颜色		0:绿 1:蓝 2:红
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetCrossCursorParams(int& nEnable, int& nColor);

/************************************************************************/
//> 图像参数
/************************************************************************/

/**
* @brief 获取当前伪彩
* @param[out] index 伪彩序号
*			   0-白热，1-黑热，2-聚变，3-彩虹，4-金秋，5-午日，6-铁红
*			   7-琥珀，8-玉石，9-夕阳，10-冰火，11-油画，12-石榴，13-翡翠
*			   14-春，15-夏，16-秋，17-冬，18-热检测，19-极光
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetColor(int* index);

/**
* @brief 伪彩切换
* @param[in] index 伪彩序号
*			   0-白热，1-黑热，2-聚变，3-彩虹，4-金秋，5-午日，6-铁红
*			   7-琥珀，8-玉石，9-夕阳，10-冰火，11-油画，12-石榴，13-翡翠
*			   14-春，15-夏，16-秋，17-冬，18-热检测，19-极光
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetColor(int index);

/**
* @brief 图像参数保存，否则断电或重启设置图像参数无效
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SaveDevImageParam();

/**
* @brief 获取亮度信息
* @param[out]  nValue	亮度值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetBrightness(int* nValue);

/**
* @brief 设置亮度信息
* @param[in]  nValue	亮度值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetBrightness(int nValue);

/**
* @brief 获取对比度
* @param[out]  nValue	对比度值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetContrast(int* nValue);

/**
* @brief 设置对比度
* @param[in]  nValue	对比度值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetContrast(int nValue);

/**
* @brief 获取锐度
* @param[out]  nValue	锐度值,范围0-120
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetSharpness(int* nValue);

/**
* @brief 设置锐度
* @param[in]  nValue	锐度值,范围0-120
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetSharpness(int nValue);

/**
* @brief 获取细节增强值
* @param[out]  nValue	细节增强值,范围0-128
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetLCEValue(int* nValue);

/**
* @brief 设置细节增强
* @param[in]  nValue	细节增强值,范围0-128
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetLCEValue(int nValue);

/**
* @brief 获取时域降噪
* @param[out]  nValue	时域降噪值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetNoise2D(int* nValue);

/**
* @brief 设置时域降噪
* @param[in]  nValue	时域降噪值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetNoise2D(int nValue);

/**
* @brief 获取空域降噪
* @param[out]  nValue	空域降噪值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetNoise3D(int* nValue);

/**
* @brief 设置空域降噪
* @param[in]  nValue	空域降噪值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetNoise3D(int nValue);

/**
* @brief 获取自动增益
* @param[out]  nValue	自动增益值,范围0-255
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetGainAuto(int* nValue);

/**
* @brief 设置自动增益
* @param[in]  nValue	自动增益值,范围0-255
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetGainAuto(int nValue);

/**
* @brief 设置宽动态
* @param[in] nValue     宽动态开关 0:关闭 1:开启
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetWideDynamic(int nValue);

/**
* @brief 获取宽动态
* @param[out] nValue    宽动态开关 0:关闭 1:开启
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetWideDynamic(int* nValue);

/**
* @brief 获取局部对比度 (启用宽动态后生效)
* @param[out]  nValue	局部对比度值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetLocalContrast(int* nValue);

/**
* @brief 设置局部对比度 (启用宽动态后生效)
* @param[in]  nValue	局部对比度值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetLocalContrast(int nValue);

/**
* @brief 获取全局对比度 (启用宽动态后生效)
* @param[out]  nValue	全局对比度值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetGlobalContrast(int* nValue);

/**
* @brief 设置全局对比度 (启用宽动态后生效)
* @param[in]  nValue	全局对比度值,范围0-100
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetGlobalContrast(int nValue);

/**
* @brief 设置电子放大倍数 1:正常 2:2倍 4:4倍 8:8倍
* @param[in]	nMagnification 放大倍数
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetDigtalZoom(int nMagnification);

/**
* @brief 获取电子放大倍数 1:正常 2:2倍 4:4倍 8:8倍
* @param[out]	nMagnification 放大倍数
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetDigtalZoom(int& nMagnification);

/**
* @brief 设置图像翻转
* @param[in]  nFlipMode	图像翻转模式 0：正常(默认) 1：垂直翻转 2：水平翻转 3：对角翻转
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetImageFlipMode(int nFlipMode);

/**
* @brief 获取图像翻转
* @param[out]  nFlipMode 图像翻转模式 0：正常(默认) 1：垂直翻转 2：水平翻转 3：对角翻转
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetImageFlipMode(int& nFlipMode);

/****************************************************************************************
*
*
*                                       测温功能
*
*
****************************************************************************************/

/************************************************************************/
//> 基础接口
/************************************************************************/

/**
* @brief 开启测温功能
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_OpenThermalFunction();

/**
* @brief 关闭测温功能
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_CloseThermalFunction();

/**
* @brief 设置打印测温日志
* @param[in]  nLog 0:关闭 1:开启
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetThermalLog(int nLog);

/************************************************************************/
//> 增益模式
/************************************************************************/

/**
* @brief 获取增益模式
* @param[out]  nValue	增益模式，0表示低温模式，1表示高温模式，2表示极致模式(极致模式不测温,通过USBSDK_GetDeviceSupportExtremeMode接口判断是否支持)
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetGainMode(int* nValue);

/**
* @brief 设置增益模式
* @param[in]  nValue	增益模式，0表示低温模式，1表示高温模式，2表示极致模式(极致模式不测温,通过USBSDK_GetDeviceSupportExtremeMode接口判断是否支持)
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetGainMode(int nValue);

/**
* @brief 是否支持极致模式
* @param[]
* @return 返回说明
*		-<em>FALSE</em> 不支持
*		-<em>TRUE</em>  支持
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetDeviceSupportExtremeMode();

/**
* @brief 是否支持低温二档
* @return 返回说明
*		-<em>FALSE</em> 不支持
*		-<em>TRUE</em>  支持
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetDeviceSupportLowTempTwoGear();

/**
* @brief 设置低温二档增益档位
* @param[in]	nAuto	自动档	0-自动档		1-手动档
* @param[in]	nGear	类型		0-低温1档	1-低温2档		(自动档时, nGear不生效)
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetLowTemperatureTwoGear(int nAuto, int nGear);

/**
* @brief 获取低温二档增益档位
* @param[out]	nAuto	自动档	0-自动档		1-手动档
* @param[out]	nGear	类型		0-低温1档	1-低温2档		(自动档时, nGear不生效)
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetLowTemperatureTwoGear(int& nAuto, int& nGear);

/************************************************************************/
//> 测温规则
/************************************************************************/

/**
* @brief 设置规则参数
* @param[in] pRules	规则信息
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetThermalRules(USBSDK_Rules pRules);

/**
* @brief 获取规则参数
* @param[in]  ruleId			规则ID
* @param[out] pRulesTemperInfo	获取到对应规则ID的温度信息
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetThermalRules(unsigned int ruleId, USBSDK_RuleTemper& pRulesTemperInfo);

/**
* @brief 获取全局冷热点信息
* @param[out] pHotAndColdInfo	冷热点信息
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetThermalHotColdPoint(USBSDK_HotAndColdInfo* pHotAndColdInfo);

/**
* @brief 获取测温全局配置参数
* @param[out] tmpTemperParam	测温全局配置参数
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetCommonTemperParam(USBSDK_RadCommonTemparam *tmpTemperParam);

/**
* @brief 设置测温全局配置参数
* @param[in] tmpTemperParam	测温全局配置参数
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetCommonTemperParam(USBSDK_RadCommonTemparam *tmpTemperParam);

/**
* @brief 获取全局像素点温度
* @param[in]  pInfo		热图相关信息
* @param[in]  pVirtAddr	raw数据+冗余数据, 内存由调用者管理, 大小为 param.width * (param.height + 4) * 2
* @param[out] pTempData	温度矩阵, 大小为原始数据 param.width * param.height * sizeof(float)
* @return 返回说明
*		-<em>FALSE</em> 失败，可通过USBSDK_GetLastError获取错误码
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetGlobalHeatMapTemperature(const USBSDK_GlobalHeatmapInfo* pInfo, void* pVirtAddr, float* pTempData);

/************************************************************************/
//> 二次标定
/************************************************************************/

/**
* @brief 二次标定初始化
* @param[]		none
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_TemperatureCalibrationInit();

/**
* @brief 二次标定温度校准
* @param[in]	nState  1:开始标定   0:停止标定
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetAutoTemperatureCalibration(int nState);

/**
* @brief 获取二次标定温度校准进度
* @param[out]  nProgValue	100-标定完成
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetAutoTemperatureCalibrationProgress(int& nProgValue);

/**
* @brief 获取二次标定平衡状态
* @param[out]	nBalanced  0:不平衡  1:平衡
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetTemperatureCalibrationBalancedState(int& nBalanced);

/**
* @brief 设置二次标定校准框
* @param[out]	stRuleInfo  生成的校准框规则
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetTemperatureCalibrationRECT(USBSDK_Rules& stRuleInfo);

/**
* @brief 保存二次标定校准框 (需要先调用设置校准框接口)
* @param[in]	stRules 校准框参数
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetSaveTemperatureCalibrationRECT(USBSDK_Rules stRules);

/**
* @brief 设置二次标定完成时间
* @param[]	none
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetTemperatureCalibrationTime();

/**
* @brief 获取二次标定结果
* @param[out]	stResult 二次标定结果
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetTemperatureCalibrationResult(USBSDK_TemperatureCalibraResult& stResult);

/**
* @brief 二次标定恢复默认
* @param[]	none
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetTemperatureCalibrationDefault();

/**
* @brief 二次标定设置高低温黑体温度值
* @param[in]	stBlackBodyTemp  对应的黑体高低温摄氏度温度
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetTemperatureCalibrationBlackBodyTemp(USBSDK_TempCaliBlackBodyTemp stBlackBodyTemp);

/**
* @brief 二次标定获取标定配置
* @param[in]	pSavePath  保存路径,格式为: Path/to/your/file, 默认为程序运行目录
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_GetTemperatureCalibrationConfig(char* pSavePath = nullptr);

/**
* @brief 二次标定设置标定配置
* @param[in]	pFilePath  文件路径,具体到文件,格式为: Path/to/your/file.data
* @return 返回说明
*		-<em>FALSE</em> 失败
*		-<em>TRUE</em>  成功
*/
USBSDK_API BOOL CALLMETHOD USBSDK_SetTemperatureCalibrationConfig(char* pFilePath);

#ifdef __cplusplus
}
#endif

#endif // !__USBLIB_H__

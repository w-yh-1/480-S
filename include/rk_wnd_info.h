/************************************************************************
* 版权所有 (C)2023, 深圳航天科技创新研究院。
* 
* 文件名称： rk_wnd_info.h
* 文件标识： 
* 内容摘要： 窗口信息处理
* 其它说明： 无
* 当前版本： V1.0
* 作    者： 李元琪
* 完成日期： 2023年3月01日
*
* 
************************************************************************/

#ifndef __RK_WND_INFO_H__
#define __RK_WND_INFO_H__
#include "rk_custom_struct.h"
#include "my_circle_buffer.h"
#include "rk_type.h"
class RkWndInfo
{
	public:
	RkWndInfo();
	~RkWndInfo();
/************************************************************************
* 函数名称： setWndBuffer
* 功能描述： 设置窗口解码buffer
* 输入参数： 
*            buf1 表示窗口1buf
			 buf2 表示窗口2buf
			 buf3 表示窗口3buf
			 buf4 表示窗口4buf
* 输出参数： 
* 返 回 值： 0，成功，-1，失败
* 其它说明： 
* ---------------------------------------------------------
* 修改日期              修改人              修改内容
* ---------------------------------------------------------
* 2023年3月1日        李元琪              创建函数
*************************************************************************/
	int setWndBuffer(mylib::MyCircleBuffer *buf1,mylib::MyCircleBuffer *buf2,mylib::MyCircleBuffer *buf3,mylib::MyCircleBuffer *buf4);
	mylib::MyCircleBuffer* getWndBuffer(int num);

	/************************************************************************
	* 函数名称： setWndArea
	* 功能描述： 设置窗口区域
	* 输入参数： 
	*			 wnd1 表示窗口1区域
				 wnd2 表示窗口2区域
				 wnd3 表示窗口3区域
				 wnd4 表示窗口4区域
				 wnd5 表示窗口全屏区域
	* 输出参数： 
	* 返 回 值： 0，成功，-1，失败
	* 其它说明： 
	* ---------------------------------------------------------
	* 修改日期				修改人			   修改内容
	* ---------------------------------------------------------
	* 2023年3月1日		 李元琪				创建函数
	*************************************************************************/
	int setWndArea(WndDisplayInfo wnd1,WndDisplayInfo wnd2,WndDisplayInfo wnd3,WndDisplayInfo wnd4,WndDisplayInfo wnd5,WndDisplayInfo wndMap);
	int getWndArea(WndDisplayInfo &wnd1,WndDisplayInfo &wnd2,WndDisplayInfo &wnd3,WndDisplayInfo &wnd4,WndDisplayInfo &wnd5,WndDisplayInfo &wndMap);
	/************************************************************************
	* 函数名称： setDecodeInfo
	* 功能描述： 解码流和分辨率设置
	* 输入参数： 
	*			 streamMode 表示 0 h264 和 1  h265 
				 w  表示分辨率的宽
				 h  表示分辨率的高
	* 输出参数： 
	* 返 回 值： 0，成功，-1，失败
	* 其它说明： 
	* ---------------------------------------------------------
	* 修改日期				修改人			   修改内容
	* ---------------------------------------------------------
	* 2023年3月1日		 李元琪				创建函数
	*************************************************************************/
	int setDecodeInfo(MppCodingType streamMode,int w, int h);
	int getDecodeInfo(MppCodingType &streamMode,int &w,int &h);
	
	static RkWndInfo* getInstance();
	private:
	mylib::MyCircleBuffer *m_wnd1_buf;
	mylib::MyCircleBuffer *m_wnd2_buf;
	mylib::MyCircleBuffer *m_wnd3_buf;
	mylib::MyCircleBuffer *m_wnd4_buf;
	WndDisplayInfo m_wnd1_info;
	WndDisplayInfo m_wnd2_info;
	WndDisplayInfo m_wnd3_info;
	WndDisplayInfo m_wnd4_info;
	WndDisplayInfo m_wndMap_info;
	WndDisplayInfo m_full_screen;
	MppCodingType m_stream_mode;
	int m_w_ratio;
	int m_h_ratio;
	static RkWndInfo *s_wndInfo;
};


#endif


#ifndef __XPKSLIDER_COMMON_H__
#define __XPKSLIDER_COMMON_H__

#pragma pack(1)

#include "PepAVToolStruct.h"

#if defined (WIN32) 
#include <time.h>
typedef void* Handle;
#include <windows.h>
#include <stdio.h>
#include <inttypes.h>
#define usleep(t) Sleep(t/1000) //t是微妙基本
#endif


//预览播放结束
typedef int (__stdcall *cb_play_finish)(void* pUser);

//媒体类型
enum MEDIA_TYPES{MEDIA_NONE=0,MEDIA_USER,MEDIA_TEXT,MEDIA_PICTURE,MEDIA_PICTURE_BUFFER,MEDIA_APNG,MEDIA_AUDIO,MEDIA_SLIDE,MEDIA_VIDEO};




typedef struct RECTF
{
	float left;			
	float top;			             
	float right;		
	float bottom;		
}RECTF;

//媒体翻转方向
enum SLIDE_FLIP
{
	FLIP_NONE=0,
	FLIP_H,			//水平翻转
	FLIP_V			//垂直翻转
};



//音频
typedef struct Audio_Stream_Info
{
	char	acodec[64];
	char	acodecTag[64];//reserve
	int		channels;
	int		samples;
	int		bitrate;//Bit
}Audio_Stream_Info;

//视频
typedef struct Video_Stream_Info 
{
	char	vcodec[64];
	char	vcodecTag[64];//reserve
	int		width;
	int		height;
	int		bitrate; //KBit
	double	framerate;
	double	aspect;
	int		aspectWidth; //reserve
	int		aspectHeight;//reserve
	int     rotate;
}Video_Stream_Info;



//动态裁剪
typedef struct MEDIA_CLIP_POINT
{
	RECTF rt;
	int64_t time;
}MEDIA_CLIP_POINT;


#define PEP_AV_ERROR(e)	(-(e))
#define ERROR_BASE_CODE						((__LINE__ & 0xffff)<<16)

#define ERROR_OK							ERROR_BASE_CODE+0x0000
#define ERROR_UNKNOWN						ERROR_BASE_CODE+0x0001
#define ERROR_CONNECT_RTMP_SERVER			ERROR_BASE_CODE+0x0002	
#define ERROR_UPLOAD_RTMP_PACKET			ERROR_BASE_CODE+0x0003
#define ERROR_OPEN_VIDEO_ENCODER			ERROR_BASE_CODE+0x0004
#define ERROR_OPEN_AUDIO_ENCODER			ERROR_BASE_CODE+0x0005
#define ERROR_OPEN_VIDEO_DECODER			ERROR_BASE_CODE+0x0006
#define ERROR_OPEN_AUDIO_DECODER			ERROR_BASE_CODE+0x0007
#define ERROR_ENCODE_VIDEO					ERROR_BASE_CODE+0x0008
#define ERROR_ENCODE_AUDIO					ERROR_BASE_CODE+0x0009
#define ERROR_DECODE_VIDEO					ERROR_BASE_CODE+0x000A
#define ERROR_DECODE_AUDIO					ERROR_BASE_CODE+0x000B
#define ERROR_WRITE_FRAME					ERROR_BASE_CODE+0x000C
#define ERROR_ALLOC_MEMORY					ERROR_BASE_CODE+0x000D
#define ERROR_INVALID_PARAM					ERROR_BASE_CODE+0x000E
#define ERROR_NOT_SUPPORT_FORAMT			ERROR_BASE_CODE+0x000F
#define ERROR_WRITE_FILE					ERROR_BASE_CODE+0x0010
#define ERROR_IMAGE_SWSCALE					ERROR_BASE_CODE+0x0011
#define ERROR_LOAD_IMAGE					ERROR_BASE_CODE+0x0012
#define ERROR_LOAD_VIDEO					ERROR_BASE_CODE+0x0013
#define ERROR_LOAD_AUDIO					ERROR_BASE_CODE+0x0014
#define ERROR_VIDEO_FILTER					ERROR_BASE_CODE+0x0015
#define ERROR_NOT_IMPLEMENTE				ERROR_BASE_CODE+0x0016
#define ERROR_NO_OUTPUT_FRAME				ERROR_BASE_CODE+0x0017
#define ERROR_RENDER_UNSUCCESSFUL			ERROR_BASE_CODE+0x0018
#define PEP_ERROR_TEST(R,T)					(((R)&0x0fff)==(DWORD)((T)&0x0fff))

#define PEP_STATUS_SUCESS					0x0001
#define PEP_STATUS_DROP_VIDEO_FRAME			0x0002

#pragma pack()	
#endif
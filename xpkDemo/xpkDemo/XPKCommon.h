#ifndef __XPK_DEMO_COMMON_H__
#define __XPK_DEMO_COMMON_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "XPKSlideCommon.h"
#include "XPKSlide.h"

#pragma comment(lib,"SlideKernel.lib")

#ifdef __cplusplus
}
#endif

#include "PepAVToolStruct.h"


#ifdef __GNUC__
typedef long long  int64_t;
typedef unsigned long long   uint64_t;
#else
typedef __int64  int64_t;
typedef unsigned __int64   uint64_t;
#endif

#define SWS_BICUBIC           4

#define TIME_DURATION_PICTURE 4000000
enum COMBOBOX_TYPE{COMBOBOX_NONE = 0,COMBOBOX_TRANSITION,COMBOBOX_MEDIA_PROPERTY,COMBOBOX_MUSIC_PROPERTY,COMBOBOX_PICTURE_PROPERTY};


#define COMBOBOX_MUSIC_PROPERTY_WIDTH 117
#define COMBOBOX_MUSIC_PROPERTY_HEIGHT 18

#define PREVIEW_WIDTH			640
#define PREVIEW_HEIGHT			360

// #define PREVIEW_WIDTH			640
// #define PREVIEW_HEIGHT			480


#define SNAPSHOT_WIDTH          100
#define SNAPSHOT_HEIGHT         100
#define SNAPSHOT_INTERSPACE     40
#define  RESOURCE_ID 100000
//组合框特效选择相关
#define  CBUTTON_WIDTH 100;
#define  CBUTTON_HEIGHT 10;

#define  DIALOG_SIZE_CHANGE 1


enum XPK_STATU
{
	XKP_NONE=0,
	XKP_PREVIEW,	//当前是预览状态
	XKP_EDIT		//当前是制作本地文件状态
};

//输出文件名
#define OUTPUT_FILE "xkp_slide.mp4"

//apng
#define XPK_APNG_BLACK_LINE "//resource//黑线.kxfx"
#define XPK_APNG_LOVE "//resource//爱心.kxfx"
#define XPK_APNG_SNOW "//resource//雪花_kxfx.kxfx"
#define XPK_APNG_FISH "//resource//年年有余.kxfx"

//music
#define XPK_MUSIC_FISH "//resource//年年有余.mp3"
#define XPK_MUSIC_MOON "//resource//明月几时有.mp3"


//动态裁剪
typedef struct XPK_DYNAMAIC_CLIP_LIST
{
	RECTF rect;
	int64_t time_start;
	int64_t time_end;

	XPK_DYNAMAIC_CLIP_LIST* next;

}XPK_DYNAMAIC_CLIP_LIST;


//配乐
typedef struct XPK_MUSIC_LIST
{
	Handle hAudio;
	char path[MAX_PATH];
	int64_t start_time;
	int64_t end_time;
	int64_t timeline_start;
	int64_t timeline_end;
	float factor;
	double speed;					//播放速度
	int64_t	fade_in;				//淡入
	int64_t fade_out;				//淡出
	int index;

	struct XPK_MUSIC_LIST* next;
};

//字幕
typedef struct XPK_SUBTITLE_LIST
{
	Handle hSubtitle;
	char srtPath[MAX_PATH];
	char name[256];
	int64_t timeline_start;
	int64_t timeline_end;
	char szText[MAX_PATH];
	RECTF rect;
	FONT_PEP font;
	RGBQUAD clr;
	int effect;
	int index;

	XPK_SUBTITLE_LIST* next;

}XPK_SUBTITLE_LIST;

//apng 特效
typedef struct XPK_APNG_RGB_OVERLAY_LIST
{

	Handle hApng;
	char szPath[MAX_PATH];
	int64_t timeline_start;
	int64_t timeline_end;
	int64_t time_start;
	int64_t time_end;
	int index;
	XPK_APNG_RGB_OVERLAY_LIST* next;

}XPK_APNG_RGB_OVERLAY_LIST;


//滤镜可以同时设置多个
typedef struct XPK_FILTER_LIST
{
	int id;
	XPK_FILTER_LIST* next;

}XPK_FILTER_LIST;

#endif;
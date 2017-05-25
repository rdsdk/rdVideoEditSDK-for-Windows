
#ifndef __PEPAVTOOLSTRUCT_INCLUDE__H_
#define __PEPAVTOOLSTRUCT_INCLUDE__H_

#pragma pack(1)

#if defined (WIN32) 
#include <time.h>
typedef void* Handle;
#include <windows.h>
#include <stdio.h>
#include <inttypes.h>
#define usleep(t) Sleep(t/1000) //t是微妙基本
#endif

#define MAX_FILES 100


#define AV_FALG_UNKNOWN		0
#define AV_FALG_FREE		1
#define AV_FALG_WORKING		2
#define AV_FALG_STOP		3
#define AV_FALG_PAUSE		4
#define AV_FALG_RESTART		5

#define AV_DELAY_MAX 0.100
#define MAX_AUDIO_PACKET_SIZE (2 * 6 * 48000)


typedef struct _tag_FONT_PEP {
	char name[32];
	int width;
	int height;
	BOOL blod; //粗体
	BOOL bias; //斜体
	BOOL underline;	//下划线
	RGBQUAD fontClr;//字体颜色
	UINT	charset;//字符集
}FONT_PEP,*PFONT_PEP;

enum TEXT_EFFECT{TEXT_NORMAL=0, TEXT_GLOW_OUT, TEXT_GLOW_INNER, TEXT_SHADOW,TEXT_STROKE};

#define ENABLE_GDIPLUSS

typedef struct _tag_WATERMASK_TEXT_ATTR {
	char text[260];
	RECT rect;
	FONT_PEP font;
	INT64 start_time;
	INT64 end_time;
#ifdef ENABLE_GDIPLUSS
	int align; //0 left 1 center 2 right
	int transparence;
	int effect;
	RGBQUAD clr_effect;
#endif
}TEXT_ATTR,*PTEXT_ATTR;

typedef struct _tag_WATERMASK_PIC_ATTR {
	char path[260];
	RECT rect;
	UINT transparence;//透明度--0～100;	
	INT64 start_time;
	INT64 end_time;
}PIC_ATTR,*PPIC_ATTR;


typedef struct _tag_AV_Metadata
{
	char lang[128];
	char title[256];
	char apsect[64];
}AV_Metadata,*PAV_Metadata;

typedef struct tag_AV_SUBTITLE_ATTR
{
	int		index;
	char	scodec[64];
	AV_Metadata metadata;
}AV_SUBTITLE_ATTR,*PAV_SUBTITLE_ATTR;


typedef struct _SUBTITLE_SRT_PARAM 
{
	char path[260];
	FONT_PEP font;
	RECT rect;
	int align; //0 left 1 center 2 right
	int transparence;
	int effect;
	RGBQUAD clr_effect;
}SUBTITLE_SRT_PARAM,*PSUBTITLE_SRT_PARAM;


#pragma pack()	
	
#endif

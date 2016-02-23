#ifndef __IMAGEFILTER_H__
#define __IMAGEFILTER_H__

#include "Curve.h"
#include "Common.h"



namespace KugouPlayer
{
	enum FilterEffectType
	{
		FILTER_EFFECT_NONE,
		FILTER_EFFECT_LITTLE_WHITENING,			//	01
		FILTER_EFFECT_MODERATE_WHITENING,		//	02
		FILTER_EFFECT_LITTLE_PINK,				//	04
		FILTER_EFFECT_MODERATE_PINK,			//	05
		FILTER_EFFECT_HIGH_PINK,				//	06
		FILTER_EFFECT_MODERATE_FLESH,			//	08
		FILTER_EFFECT_HIGH_FLESH,				//	09
		FILTER_EFFECT_CLASSIC_HDR,				//	23
		FILTER_EFFECT_YELLOWING_DARK_CORNERS,	//	24
		FILTER_EFFECT_PINK_LADY,				//	32
		FILTER_EFFECT_ICE_SPIRIT,				//	33
		FILTER_EFFECT_NIGHT_VIEW,				//	44
		FILTER_EFFECT_HIGH_WHITENING,			//	03
		FILTER_EFFECT_LITTLE_FLESH,				//	07
		FILTER_EFFECT_RAINBOW_GRADIENT,			//	12
	};

	class ImageFilter
	{
	public:
		ImageFilter();

		int mFilterType;//ÂË¾µÀàÐÍ

		void SetFilterType( int type );

		void Process( unsigned char * buffer, int width, int height);
	private:
		CurvesConfig config;
	};
}

#endif


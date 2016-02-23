#ifndef __CURVE_H__
#define __CURVE_H__

namespace KugouPlayer
{

	typedef unsigned char uint8;
	typedef unsigned char byte;

	class Vector2
	{
	public:
		Vector2(void)
		{
			x = y = 0.0;
		}

		Vector2(const Vector2 & r)
		{
			this->x = r.x;
			this->y = r.y;
		}

		const Vector2 & operator=(const Vector2 & r)
			{
				if (this != &r)
				{
					this->x = r.x;
					this->y = r.y;
				}

				return *this;
			}

	public:
		double x;
		double y;
	};

	typedef enum {CURVE_TYPE_UNKNOWN,CURVE_TYPE_SMOOTH,CURVE_TYPE_FREE} CurveType;

	/*
	 *曲线类
	 */
	class Curve
	{
	public:
		Curve(void);
		virtual ~Curve(void);
		void Initialize(void);
		void SetCurveType(CurveType type);
		void SetPointCount(int pointCount);
		void SetSampleCount(int sampleCount);
		void SetPoint(int index, double x, double y);
		void SetPoints(int count, Vector2 * points);
		void SetSamples(int count, double * samples);
		void Reset(bool resetType);
		void Calculate(void);
		void Plot(int p1, int p2, int p3, int p4);
		double GetValue(double value);
		byte GetValue(int index);
		void Cleanup(void);

		const Curve & operator=(const Curve & r);

	public:
		CurveType	type;
		int			pointCount;
		Vector2*	points;
		int			sampleCount;
		double*		samples;
		bool		identity;
		byte*		tables;
	};

	typedef enum
	{
		CURVE_CHANNEL_C = 0,		/*< desc="Value" >*/
		CURVE_CHANNEL_B = 1,		/*< desc="Blue"  >*/
		CURVE_CHANNEL_G = 2,		/*< desc="Green" >*/
		CURVE_CHANNEL_R = 3,		/*< desc="Red"   >*/
		CURVE_CHANNEL_A = 4,		/*< desc="Alpha" >*/
		CURVE_CHANNEL_RGB = 5		/*< desc="RGB", pdb-skip >*/
	} CurveChannel;

	/*
	 * 曲线配置类
	 */
	class CurvesConfig
	{
	public:
		CurvesConfig(void);
		virtual ~CurvesConfig(void);
		void Initialize(void);
		void Calculate(void);
		void CreateSpline(int channel, int count, double * points);
		void CreateSplineCruft(int channel, int count, uint8 * points);
		void CurvesSpline(int channel, int count, uint8 * points);
		void Reset(void);
		void Cleanup(void);

		const CurvesConfig & operator=(const CurvesConfig & r);//赋值运算符重载函数
	public:
		int channel;
		Curve * curves[5];//曲线配置类中定义了一个曲线类指针的数组,数组中存放的就是曲线类的指针
	};
}

#endif

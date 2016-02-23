#include "Curve.h"
#include <stdlib.h>
#include <assert.h>
#include <malloc.h>
#include <memory.h>
#include <float.h>
#include <math.h>

//#define min(a, b) ((a) < (b) ? (a) : (b))
//#define max(a, b) ((a) > (b) ? (a) : (b))
#ifndef min
#define min(a, b) ((a)<(b)?(a):(b))
#endif

#ifndef max
#define max(a, b) ((a)<(b)?(b):(a))
#endif
#define clamp(x, a, b)	max((a), min((x), (b)))
#define ROUND(x) ((int) ((x) + 0.5))

namespace KugouPlayer
{
	Curve::Curve()
	{
		type = CURVE_TYPE_UNKNOWN;
		pointCount = 0;
		points = 0;
		sampleCount = 0;
		samples = 0;
		identity = false;
		tables = 0;
	}


	Curve::~Curve()
	{
		Cleanup();
	}

	void Curve::Initialize(void)
	{
		SetCurveType(CURVE_TYPE_SMOOTH);

		SetPointCount(17);

		//Point p[17];
		//memset(&p, 0, sizeof(Point)* 17);
		//SetPoints(17, p);

		SetSampleCount(256);

		//double s[256] = { 0.0 };
		//SetSamples(256, s);
	}

	void Curve::SetCurveType(CurveType type)
	{
		if (this->type != type)
		{
			this->type = type;

			if (this->type == CURVE_TYPE_SMOOTH)
			{
				for (int i = 0; i < pointCount; i++)
				{
					points[i].x = -1.0;
					points[i].y = -1.0;
				}

				int count = clamp(9, pointCount / 2, pointCount);

				for (int i = 0; i < count; i++)
				{
					int sample = i * (sampleCount - 1) / (count - 1);
					int point = i * (pointCount - 1) / (count - 1);

					points[point].x = (double)sample / (double)(sampleCount - 1);
					points[point].y = samples[sample];
				}
			}
		}
	}

	void Curve::SetPointCount(int pointCount)
	{
		assert(pointCount >= 2 && pointCount <= 1024);

		if (this->pointCount != pointCount)
		{
			this->pointCount = pointCount;

			points = (Vector2*)realloc(points, sizeof(Vector2)* pointCount);

			points[0].x = 0.0;
			points[0].y = 0.0;

			for (int i = 1; i < pointCount - 1; i++)
			{
				points[i].x = -1.0;
				points[i].y = -1.0;
			}

			points[pointCount - 1].x = 1.0;
			points[pointCount - 1].y = 1.0;

			if (this->type == CURVE_TYPE_SMOOTH)
			{
				identity = true;
			}
		}
	}

	void Curve::SetSampleCount(int sampleCount)
	{
		if (sampleCount<256 || sampleCount>4096)
		{
			return;
		}

		if (this->sampleCount != sampleCount)
		{
			this->sampleCount = sampleCount;

			samples = (double*)realloc(samples, sampleCount * sizeof(double));

			for (int i = 0; i < sampleCount; i++)
			{
				samples[i] = (double)i / (double)(sampleCount - 1);
			}

			tables = (byte*)realloc(tables, sampleCount * sizeof(byte));

			for (int i = 0; i < sampleCount; i++)
			{
				tables[i] = (byte)(samples[i] * 255);
			}

			if (this->type == CURVE_TYPE_FREE)
			{
				identity = true;
			}
		}
	}

	void Curve::SetPoint(int index, double x, double y)
	{
		assert(x == -1.0 || x >= 0.0 && x <= 1.0);
		assert(y == -1.0 || y >= 0.0 && y <= 1.0);

		if (this->type == CURVE_TYPE_FREE)
		{
			return;
		}

		points[index].x = x;
		points[index].y = y;
	}

	void Curve::SetPoints(int count, Vector2 * points)
	{
		for (int i = 0; i < pointCount && i < count; i++)
		{
			this->points[i] = points[i];
		}
	}

	void Curve::SetSamples(int count, double * samples)
	{
		for (int i = 0; i < sampleCount && i < count; i++)
		{
			this->samples[i] = samples[i];
		}
	}

	void Curve::Reset(bool resetType)
	{
		for (int i = 0; i < sampleCount; i++)
		{
			samples[i] = (double)i / (double)(sampleCount - 1);
			tables[i] = (byte)(samples[i] * 255);
		}

		points[0].x = 0.0;
		points[0].y = 0.0;

		for (int i = 1; i < pointCount - 1; i++)
		{
			points[i].x = -1.0;
			points[i].y = -1.0;
		}

		points[pointCount - 1].x = 1.0;
		points[pointCount - 1].y = 1.0;

		if (resetType)
		{
			this->type = CURVE_TYPE_SMOOTH;
		}

		identity = true;
	}

	void Curve::Calculate(void)
	{
		int p1, p2, p3, p4;
		int count;
	
		int* p = (int*)alloca(this->pointCount * sizeof(int));

		switch (this->type)
		{
		case CURVE_TYPE_SMOOTH:
			count = 0;
			for (int i = 0; i < pointCount; i++)
			{
				if (points[i].x >= 0.0)
				{
					p[count++] = i;
				}
			}

			if (count != 0)
			{
				Vector2 pt = points[p[0]];
				int boundary = ROUND(pt.x * (double)(sampleCount - 1));

				for (int i = 0; i < boundary; i++)
				{
					samples[i] = pt.y;
				}

				pt = points[p[count - 1]];
				boundary = ROUND(pt.x * (double)(sampleCount - 1));

				for (int i = boundary; i < sampleCount; i++)
				{
					samples[i] = pt.y;
				}

				for (int i = 0; i < count-1; i++)
				{
					p1 = p[max(i - 1, 0)];
					p2 = p[i];
					p3 = p[i + 1];
					p4 = p[min(i + 2, count - 1)];

					Plot(p1, p2, p3, p4);
				}

				for (int i = 0; i < count; i++)
				{
					double x = points[p[i]].x;
					double y = points[p[i]].y;

					samples[ROUND(x * (double)(sampleCount - 1))] = y;
				}
			}

			for (int i = 0; i < sampleCount; i++)
			{
				this->tables[i] = (byte)(samples[i] * 255);
			}

			break;
		case CURVE_TYPE_FREE:

			break;
		}
	}

	void Curve::Plot(int p1, int p2, int p3, int p4)
	{
		double y1, y2, slope;
		double x0 = points[p2].x;
		double y0 = points[p2].y;
		double x3 = points[p3].x;
		double y3 = points[p3].y;

		/*
		 * the x values of the inner control points are fixed at
		 * x1 = 2/3*x0 + 1/3*x3   and  x2 = 1/3*x0 + 2/3*x3
		 * this ensures that the x values increase linearily with the
		 * parameter t and enables us to skip the calculation of the x
		 * values altogehter - just calculate y(t) evenly spaced.
		 */

		double dx = x3 - x0;
		double dy = y3 - y0;

		if (dx <= 0) return;

		if (p1 == p2 && p3 == p4)
		{
			/* No information about the neighbors,
			 * calculate y1 and y2 to get a straight line
			 */
			y1 = y0 + dy / 3.0;
			y2 = y0 + dy * 2.0 / 3.0;
		}
		else if (p1 == p2 && p3 != p4)
		{
			/* only the right neighbor is available. Make the tangent at the
			 * right endpoint parallel to the line between the left endpoint
			 * and the right neighbor. Then point the tangent at the left towards
			 * the control handle of the right tangent, to ensure that the curve
			 * does not have an inflection point.
			 */
			slope = (points[p4].y - y0) / (points[p4].x - x0);

			y2 = y3 - slope * dx / 3.0;
			y1 = y0 + (y2 - y0) / 2.0;
		}
		else if (p1 != p2 && p3 == p4)
		{
			/* see previous case */
			slope = (y3 - points[p1].y) / (x3 - points[p1].x);

			y1 = y0 + slope * dx / 3.0;
			y2 = y3 + (y1 - y3) / 2.0;
		}
		else /* (p1 != p2 && p3 != p4) */
		{
			/* Both neighbors are available. Make the tangents at the endpoints
			 * parallel to the line between the opposite endpoint and the adjacent
			 * neighbor.
			 */
			slope = (y3 - points[p1].y) / (x3 - points[p1].x);

			y1 = y0 + slope * dx / 3.0;

			slope = (points[p4].y - y0) / (points[p4].x - x0);

			y2 = y3 - slope * dx / 3.0;
		}

		/*
		 * finally calculate the y(t) values for the given bezier values. We can
		 * use homogenously distributed values for t, since x(t) increases linearily.
		 */
		for (int i = 0; i <= ROUND(dx * (double)(sampleCount - 1)); i++)
		{
			double t = i / dx / (double)(sampleCount - 1);
			double y = y0 * (1 - t) * (1 - t) * (1 - t) +
				3 * y1 * (1 - t) * (1 - t) * t +
				3 * y2 * (1 - t) * t     * t +
				y3 * t     * t     * t;

			int index = i + ROUND(x0 * (double)(sampleCount - 1));

			if (index < sampleCount)
			{
				samples[index] = clamp(y, 0.0, 1.0);
			}
		}
	}

	double Curve::GetValue(double value)
	{
		if (this->identity)
		{
			if (isfinite(value))
			{
				return clamp(value, 0.0, 1.0);
			}

			return 0.0;
		}

		if (value > 0.0 && value < 1.0)
		{
			value = value * (sampleCount - 1);

			int index = (int)value;

			double f = value - index;

			return (1.0 - f) * samples[index] + f * samples[index + 1];
		}
		else if (value >= 1.0)
		{
			return samples[sampleCount - 1];
		}
		else
		{
			return samples[0];
		}
	}

	byte Curve::GetValue(int index)
	{
		assert(index >= 0 && index < sampleCount);
		return tables[index];
	}

	void Curve::Cleanup(void)
	{
		if (points != 0)
		{
			free(points);
			points = 0;
		}

		if (samples != 0)
		{
			free(samples);
			samples = 0;
		}

		if (tables != 0)
		{
			free(tables);
			tables = 0;
		}
	}

	const Curve & Curve::operator=(const Curve & r)
	{
		if (this != &r)
		{
			this->type = r.type;
			this->pointCount = r.pointCount;
			this->points = (Vector2*)realloc(this->points, sizeof(Vector2)* this->pointCount);
			for (int i = 0; i < pointCount; i++)
			{
				this->points[i] = r.points[i];
			}

			this->sampleCount = r.sampleCount;
			this->samples = (double*)realloc(this->samples, sizeof(double)* this->sampleCount);
			for (int i = 0; i < sampleCount; i++)
			{
				this->samples[i] = r.samples[i];
			}

			this->tables = (byte*)realloc(this->tables, sizeof(byte) * this->sampleCount);
			for (int i = 0; i < sampleCount; i++)
			{
				this->tables[i] = r.tables[i];
			}

			this->identity = r.identity;
		}

		return *this;
	}

	CurvesConfig::CurvesConfig(void)
	{
		channel = 0;
		for (int i = 0; i < 5; i++)
		{
			curves[i] = 0;
		}
	}

	CurvesConfig::~CurvesConfig(void)
	{
		Cleanup();
	}

	void CurvesConfig::Initialize(void)
	{
		for (int i = 0; i < 5; i++)
		{
			curves[i] = new Curve;
			assert(curves[i] != 0);

			curves[i]->Initialize();
		}
	}

	void CurvesConfig::Calculate(void)
	{
		for (int i = 0; i < 5; i++)
		{
			curves[i]->identity = false;

			curves[i]->Calculate();
		}
	}

	void CurvesConfig::CreateSpline(int channel, int count, double * points)
	{
		assert(channel >= CURVE_CHANNEL_C && channel <= CURVE_CHANNEL_A);
		assert(count >= 2 && count <= 1024);

		Curve * curve = curves[channel];

		curve->SetCurveType(CURVE_TYPE_SMOOTH);
		curve->SetSampleCount(count);

		curve->SetPoint(curve->pointCount - 1, -1.0, -1.0);

		for (int i = 0; i < count; i++)
		{
			curve->SetPoint(i, points[2 * i], points[2 * i + 1]);
		}
	}

	void CurvesConfig::CreateSplineCruft(int channel, int count, uint8 * points)
	{
		assert(channel >= CURVE_CHANNEL_C && channel <= CURVE_CHANNEL_A);
		assert(count >= 2 && count <= 1024);

		double * p = (double*)malloc(sizeof(double)* 2 * count);

		for (int i = 0; i < count; i++)
		{
			p[2 * i + 0] = points[2 * i + 0] / 255.0;
			p[2 * i + 1] = points[2 * i + 1] / 255.0;
		}

		CreateSpline(channel, count, p);

		free(p);
	}

	void CurvesConfig::CurvesSpline(int channel, int count, uint8 * points)
	{
		CreateSplineCruft(channel, count / 2, points);
	}

	void CurvesConfig::Reset(void)
	{
		for (int i = 0; i < 5; i++)
		{
			if (curves[i] != 0)
			{
				curves[i]->Reset(true);
			}
		}
	}

	void CurvesConfig::Cleanup(void)
	{
		for (int i = 0; i < 5; i++)
		{
			if (curves[i] != 0)
			{
				free(curves[i]);
				curves[i] = 0;
			}
		}
	}

	const CurvesConfig & CurvesConfig::operator=(const CurvesConfig & r)
	{
		if (this != &r)
		{
			this->channel = r.channel;
			for (int i = 0; i < 5; i++)
			{
				*curves[i] = *(r.curves[i]);
			}
		}

		return *this;
	}
}

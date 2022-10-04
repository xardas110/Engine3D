#include "GJK.h"
#include <iostream>

GJKStatus updateGJKSimplex(GJKSimplex& s, const GJKSupportPoint& a, glm::vec3& dir)
{
	if (s.numPoints == 2)
	{
		// Triangle case.
		glm::vec3 ao = -a.minkowski;
		glm::vec3 ab = s.b.minkowski - a.minkowski;
		glm::vec3 ac = s.c.minkowski - a.minkowski;
		glm::vec3 abc = glm::cross(ab, ac);

		glm::vec3 abp = glm::cross(ab, abc);
		if (glm::dot(ao, abp) > 0.f)
		{
			s.c = a;
			dir = CrossABA(ab, ao);
			return gjk_dont_stop;
		}
		glm::vec3 acp = glm::cross(abc, ac);
		if (dot(ao, acp) > 0.f)
		{
			s.b = a;
			dir = CrossABA(ac, ao);
			return gjk_dont_stop;
		}

		// Sort so that normal abc of triangle points outside (negative new search dir).
		if (glm::dot(ao, abc) >= 0.f)
		{
			s.d = s.b;
			s.b = a;
			s.numPoints = 3;
			dir = abc;
			return gjk_dont_stop;
		}
		if (glm::dot(ao, -abc) >= 0.f)
		{
			s.d = s.c;
			s.c = s.b;
			s.b = a;
			s.numPoints = 3;
			dir = -abc;
			return gjk_dont_stop;
		}

		//assert(false);
		return gjk_unexpected_error;
	}
	if (s.numPoints == 3)
	{
		// Tetrahedron case.
		glm::vec3 ao = -a.minkowski;
		glm::vec3 ab = s.b.minkowski - a.minkowski;
		glm::vec3 ac = s.c.minkowski - a.minkowski;
		glm::vec3 ad = s.d.minkowski - a.minkowski;

		glm::vec3 bcd = cross(s.c.minkowski - s.b.minkowski, s.d.minkowski - s.b.minkowski);
		if (glm::dot(bcd, dir) > 0.00001f || glm::dot(bcd, s.b.minkowski) < -0.00001f)
		{
			std::cerr << "GJK ERROR 0\n";
			return gjk_unexpected_error;
		}

		// Normals of faces (point outside).
		glm::vec3 abc = cross(ac, ab);
		glm::vec3 abd = cross(ab, ad);
		glm::vec3 adc = cross(ad, ac);

		int32 flags = 0;
		const int32 overABCFlag = 1;
		const int32 overABDFlag = 2;
		const int32 overADCFlag = 4;

		flags |= (glm::dot(abc, ao) > 0.f) ? overABCFlag : 0;
		flags |= (glm::dot(abd, ao) > 0.f) ? overABDFlag : 0;
		flags |= (glm::dot(adc, ao) > 0.f) ? overADCFlag : 0;

		if (flags == (overABCFlag | overABDFlag | overADCFlag))
		{
			std::cerr << "GJK ERROR 0\n";
			return gjk_unexpected_error;
		}

		if (flags == 0)
		{
			return gjk_stop;
		}

		if (flags == overABCFlag)
		{
		overABC1:
			if (glm::dot(glm::cross(abc, ab), ao) > 0.f)
			{
				// New line: ab
				s.c = a;
				s.numPoints = 2;
				dir = CrossABA(ab, ao);
				return gjk_dont_stop;
			}
		overABC2:
			if (glm::dot(glm::cross(ac, abc), ao) > 0.f)
			{
				// New line: ac
				s.b = a;
				s.numPoints = 2;
				dir = CrossABA(ac, ao);
				return gjk_dont_stop;
			}

			// Stay in triangle case: bca
			s.d = a;
			dir = abc;
			return gjk_dont_stop;
		}

		if (flags == overABDFlag)
		{
		overABD1:
			if (glm::dot(glm::cross(abd, ad), ao) > 0.f)
			{
				// New line: ad
				s.b = s.d;
				s.c = a;
				s.numPoints = 2;
				dir = CrossABA(ad, ao);
				return gjk_dont_stop;
			}
		overABD2:
			if (glm::dot(glm::cross(ab, abd), ao) > 0.f)
			{
				// New line: ab
				s.c = a;
				s.numPoints = 2;
				dir = CrossABA(ab, ao);
				return gjk_dont_stop;
			}

			// Stay in triangle case: bad
			s.c = a;
			dir = abd;
			return gjk_dont_stop;
		}

		if (flags == overADCFlag)
		{
		overADC1:
			if (glm::dot(glm::cross(adc, ac), ao) > 0.f)
			{
				// New line: ac
				s.b = a;
				s.numPoints = 2;
				dir = CrossABA(ac, ao);
				return gjk_dont_stop;
			}
		overADC2:
			if (glm::dot(glm::cross(ad, adc), ao) > 0.f)
			{
				// New line: ad
				s.b = a;
				s.c = s.d;
				s.numPoints = 2;
				dir = CrossABA(ad, ao);
				return gjk_dont_stop;
			}

			// Stay in triangle case: acd
			s.b = a;
			dir = adc;
			return gjk_dont_stop;
		}

		if (flags == (overABCFlag | overABDFlag))
		{
			if (glm::dot(glm::cross(abc, ab), ao) > 0.f)
			{
				goto overABD1;
			}
			goto overABC2;
		}

		if (flags == (overABDFlag | overADCFlag))
		{
			if (glm::dot(glm::cross(abd, ad), ao) > 0.f)
			{
				goto overADC1;
			}
			goto overABD2;
		}

		if (flags == (overADCFlag | overABCFlag))
		{
			if (glm::dot(glm::cross(adc, ac), ao) > 0.f)
			{
				goto overABC1;
			}
			goto overADC2;
		}

		//assert(false);
		std::cerr << "GJK ERROR 1\n";
		return gjk_unexpected_error;
	}

	//assert(false);
	std::cerr << "GJK ERROR 2\n";
	return gjk_unexpected_error;
}


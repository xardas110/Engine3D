
/// ==========================================================================
///	Header for raycast intersections
/// By: AAA 5/16/2022
/// ==========================================================================

/*Optimized variant*/
float RaySphereIntersect(
	vec3 origin,
	vec3 rayDir,
	vec3 sphereCenter,
	float sphereRadius)
{
	vec3 tmp = origin - sphereCenter;

	float half_b = dot(tmp, rayDir);
	float c = dot(tmp, tmp) - sphereRadius * sphereRadius;
	float sqrtDiscriminant = sqrt(half_b * half_b - c);

	float root0 = (-half_b - sqrtDiscriminant);
	float root1 = (-half_b + sqrtDiscriminant);

	return
		sqrtDiscriminant < 0.0 ? // any real roots?
		-1.0 : // no hit
		min(root0, root1);
}

/*Raytracing one week*/
float RaySphereIntersect1(
    vec3 ray,
    vec3 rayDir,
    vec3 sphereCenter,
    float sphereRadius)
{
    vec3 tmp = ray - sphereCenter;
    
    float a = dot(rayDir, rayDir);
    float half_b = dot(tmp, rayDir);
    float c = dot(tmp, tmp) - sphereRadius * sphereRadius;
    float discriminant = half_b * half_b - a * c;
    
    if (discriminant < 0.0) { return -1.0; }
      
    float e = sqrt(discriminant);

    float root = (-half_b - e) / a;

    if (root >= 0.0) { return root; }

    root = (-half_b + e) / a;

    if (root >= 0.0) { return root; }
    
    return -1.0;
}

///result.x = is box hit?
///result.y = tMin
///result.z = tMax
vec3 RayBoxIntersect(vec3 rayOrig, vec3 rayDir, vec3 minBounds, vec3 maxBounds) {
  vec3 inverse_dir = 1.0 / rayDir;
  vec3 tbot = inverse_dir * (minBounds - rayOrig);
  vec3 ttop = inverse_dir * (maxBounds - rayOrig);
  vec3 tmin = min(ttop, tbot);
  vec3 tmax = max(ttop, tbot);
  vec2 traverse = max(tmin.xx, tmin.yz);
  float traverselow = max(traverse.x, traverse.y);
  traverse = min(tmax.xx, tmax.yz);
  float traversehi = min(traverse.x, traverse.y);
  return vec3(float(traversehi > max(traverselow, 0.0)), traverselow, traversehi);
}


bool RaySphereIntersect0(vec3 o, vec3 d, vec3 sphereCenter, float innerSphereRadius, float outerSphereRadius, out vec3 minT, out vec3 maxT)
{
	// Intersect inner sphere
	vec3 sphereToOrigin = (o - sphereCenter);
	float b = dot(d, sphereToOrigin);
	float c = dot(sphereToOrigin, sphereToOrigin);
	float sqrtOpInner = b*b - (c - innerSphereRadius * innerSphereRadius);

	// No solution (we are outside the sphere, looking away from it)
	float maxSInner;
	if(sqrtOpInner < 0.0)
	{
		return false;
	}
	
	float deInner = sqrt(sqrtOpInner);
	float solAInner = -b - deInner;
	float solBInner = -b + deInner;

	maxSInner = max(solAInner, solBInner);

	if(maxSInner < 0.0) return false;

	maxSInner = maxSInner < 0.0? 0.0 : maxSInner;
	
	// Intersect outer sphere
	float sqrtOpOuter = b*b - (c - outerSphereRadius * outerSphereRadius);

	// No solution - same as inner sphere
	if(sqrtOpOuter < 0.0)
	{
		return false;
	}
	
	float deOuter = sqrt(sqrtOpOuter);
	float solAOuter = -b - deOuter;
	float solBOuter = -b + deOuter;

	float maxSOuter = max(solAOuter, solBOuter);

	if(maxSOuter < 0.0) return false;

	maxSOuter = maxSOuter < 0.0? 0.0 : maxSOuter;

	// Compute entering and exiting ray points
	float minSol = min(maxSInner, maxSOuter);
	float maxSol = max(maxSInner, maxSOuter);

	minT = o + d * minSol;
	maxT = o + d * maxSol;

	return true;
}

bool PtBoxIntersect(vec3 q, vec3 a, vec3 b)
{
	for(int i = 0 ; i < 3; i++)
	{
		if (q[i] < a[i]) return false;
		if (q[i] > b[i]) return false;
	}
	return true;
}

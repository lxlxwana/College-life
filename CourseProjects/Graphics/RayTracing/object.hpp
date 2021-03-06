#pragma once

#include <string>
#include <vector>

#include "vec3d.hpp"


// 距离误差，小于该值认为是0
#define MIN_DISTANCE 1e-4

// 光线
class ray {
public:
	vec3d origin;       //起点
	vec3d direction;    //方向

	ray() {};
	ray(vec3d _origin, vec3d _direction) : origin(_origin), direction(_direction) {};
};

// 材质
struct material {
	std::string name;
	vec3d Ka, Kd, Ks;
	double Ns;
	double Tr = 0;
	double Ni = 1;
	int illum;
	bool isLight;
};

class rootObject;

struct intersaction {
	bool isIntersacted;
	rootObject *obj;
	vec3d pos;
	vec3d normal;
	double min_t;
};

enum objectType {
	MESH, SPHERE
};

// object基类，即三角面片
class rootObject {
public:
	material *mat;   
	enum objectType type;
	std::vector<std::vector<int>> faceList;

	rootObject();
	virtual ~rootObject();

	vec3d calcNormal(const vec3d a, const vec3d b, const vec3d c);
	bool intersectTriangle(const ray &r, const vec3d v0, const vec3d v1, const vec3d v2,
		double *t, double *u, double *v);

	virtual void hit(intersaction &inter, std::vector<vec3d> vertexList, ray r);
};

// sphere
class sphere : public rootObject {
public:
	// 球心和半径
	vec3d  center;
	double radius;

	sphere();
	~sphere();

	void hit(intersaction &inter, std::vector<vec3d> vertexList, ray r);
};

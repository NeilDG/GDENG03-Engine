#pragma once
#include "AHittable.h"
#include <memory>
#include <vector>

using std::shared_ptr;
using std::make_shared;

class HittableList :   public AHittable
{
public:
	HittableList() = default;
	HittableList(shared_ptr<AHittable> object);
	bool hit(const Ray r, float tMin, float tMax, HitRecord& rec) const override;

	void clear();
	void add(shared_ptr<AHittable> object);

private:
	typedef std::vector<shared_ptr<AHittable>> List;
	List objects;
	
};


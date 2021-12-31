#include "HittableList.h"

HittableList::HittableList(shared_ptr<AHittable> object)
{
	this->add(object);
}

bool HittableList::hit(const Ray r, float tMin, float tMax, HitRecord& rec) const
{
	HitRecord tempRec;
	bool hasHitAnything = false;
	float closest = tMax;

	for(int i = 0; i < this->objects.size(); i++)
	{
		if(this->objects[i]->hit(r, tMin, closest, tempRec))
		{
			hasHitAnything = true;
			closest = tempRec.t;
			rec = tempRec;
		}
	}

	return hasHitAnything;
}

void HittableList::clear()
{
	this->objects.clear();
}

void HittableList::add(shared_ptr<AHittable> object)
{
	this->objects.push_back(object);
}

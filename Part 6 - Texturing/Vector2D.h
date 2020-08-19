#pragma once
class Vector2D
{
public:
	Vector2D();
	~Vector2D();

	Vector2D(float x, float y);
	Vector2D(const Vector2D* vector);

	float getX() const;
	float getY() const;

	Vector2D operator *(float num);
	Vector2D operator +(Vector2D vec);

	static Vector2D lerp(const Vector2D start, const Vector2D end, float delta);
	static Vector2D zeros();
	static Vector2D ones();

private:
	float x = 0.0f;
	float y = 0.0f;
};


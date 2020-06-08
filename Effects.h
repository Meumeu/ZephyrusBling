#include "Bling.h"

class Rotate : public BlingFx<Rotate, float>
{
public:
	glm::mat3 transform(float t) override;
	~Rotate();
};

class Translate : public BlingFx<Translate, glm::vec2>
{
public:
	glm::mat3 transform(float t) override;
	~Translate();
};

class Scale : public BlingFx<Scale, glm::vec2>
{
public:
	glm::mat3 transform(float t) override;
	~Scale();
};

class Brightness : public BlingFx<Brightness, float>
{
public:
	double brightness(float t) override;
	~Brightness();
};

class Alpha : public BlingFx<Alpha, float>
{
public:
	double alpha(float t) override;
	~Alpha();
};

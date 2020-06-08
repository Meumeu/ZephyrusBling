#include "Effects.h"

#include <glm/gtx/matrix_transform_2d.hpp>

Rotate::~Rotate() {}
Translate::~Translate() {}
Scale::~Scale() {}
Brightness::~Brightness() {}
Alpha::~Alpha() {}

glm::mat3 Rotate::transform(float t)
{
	return glm::rotate(glm::mat3(1.0), get_value_at(t));
}

glm::mat3 Translate::transform(float t)
{
	return glm::translate(glm::mat3(1.0), get_value_at(t));
}

glm::mat3 Scale::transform(float t)
{
	return glm::scale(glm::mat3(1.0), get_value_at(t));
}

double Brightness::brightness(float t)
{
	return get_value_at(t);
}

double Alpha::alpha(float t)
{
	return get_value_at(t);
}

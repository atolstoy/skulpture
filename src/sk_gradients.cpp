/*
 * skulpture_gradients.cpp
 *
 */

#include "skulpture_p.h"
#include "sk_factory.h"


/*-----------------------------------------------------------------------*/
/**
 * GradientFactory - create a QGradient from a description
 *
 * The gradient description is a bytecode stream that allows simple arithmetic,
 * conditionals, and looping.
 *
 */


/*-----------------------------------------------------------------------*/

void GradientFactory::executeCode(Code code)
{
	qreal v;

	switch (code) {
		case ColorAt:
			v = evalValue();
			gradient.setColorAt(v, evalColor());
			break;
		default:
			AbstractFactory::executeCode(code);
			break;
	}
}


void GradientFactory::skipCode(Code code)
{
	switch (code) {
		case ColorAt:
			skipValue();
			skipColor();
			break;
		default:
			AbstractFactory::skipCode(code);
			break;
	}
}


/*-----------------------------------------------------------------------*/

QGradient GradientFactory::createGradient(GradientFactory::Description description, qreal var[])
{
	GradientFactory factory;

	factory.setDescription(description);
	for (int n = MinVar; n <= MaxVar; ++n) {
		factory.setVar(n, var[n]);
	}
	factory.create();
	for (int n = MinVar; n <= MaxVar; ++n) {
		var[n] = factory.getVar(n);
	}
	return factory.getGradient();
}


QGradient GradientFactory::createGradient(GradientFactory::Description description)
{
	GradientFactory factory;

	factory.setDescription(description);
	factory.create();
	return factory.getGradient();
}



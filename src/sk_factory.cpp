/*
 * skulpture_factory.cpp
 *
 */

#include "skulpture_p.h"
#include "sk_factory.h"


/*-----------------------------------------------------------------------*/

bool AbstractFactory::evalCondition()
{
	Code code = *p++;
	if (code < Or) {
		qreal v1 = evalValue();
		qreal v2 = evalValue();
		switch (code) {
			case EQ: return qAbs(v1 - v2) < 1.0e-9;
			case NE: return qAbs(v1 - v2) >= 1.0e-9;
			case LT: return v1 < v2;
			case GE: return v1 >= v2;
			case GT: return v1 > v2;
			case LE: return v1 <= v2;
			default:
				break;
		}
	} else {
		switch (code) {
			case OptionState:
				return opt && (opt->state & (1 << *p++));
			case OptionRTL:
				return opt && (opt->direction != Qt::LeftToRight);
			case OptionVersion:
				return opt && (opt->version >= *p++);
			case OptionType:
				return opt && (!*p || opt->type == *p++);
			case OptionComplex:
				return opt && ((!*p && opt->type >= QStyleOption::SO_Complex) || (opt->type == QStyleOption::SO_Complex + *p++));
			case FactoryVersion:
				return version() >= *p++;
			case Not:
				return !evalCondition();
			case Or:
				if (evalCondition()) {
					skipCondition();
					return true;
				} else {
					return evalCondition();
				}
			case And:
				if (!evalCondition()) {
					skipCondition();
					return false;
				} else {
					return evalCondition();
				}
			default:
				break;
		}
	}
	return false;
}


void AbstractFactory::skipCondition()
{
	Code code = *p++;
	if (code < Or) {
		skipValue();
		skipValue();
	} else {
		skipCondition();
		skipCondition();
	}
}


/*-----------------------------------------------------------------------*/

qreal AbstractFactory::evalValue()
{
	Code code = *p++;
	if (code >= MinVal && code <= MaxVal) {
		return code * 0.01;
	} else if (code >= GetVar + MinVar && code <= GetVar + MaxVar) {
		return var[code - GetVar];
	} else if (code >= Add && code <= Max) {
		qreal v1 = evalValue();
		qreal v2 = evalValue();
		switch (code) {
			case Add: return v1 + v2;
			case Sub: return v1 - v2;
			case Mul: return v1 * v2;
			case Div: return v2 != 0 ? v1 / v2 : 0;
			case Min: return qMin(v1, v2);
			case Max: return qMax(v1, v2);
		}
	} else if (code == Mix) {
		qreal v = evalValue();
		return v * evalValue() + (1 - v) * evalValue();
	} else if (code == Cond) {
		if (evalCondition()) {
			qreal v = evalValue();
			skipValue();
			return v;
		} else {
			skipValue();
			return evalValue();
		}
	}
	return 0;
}


void AbstractFactory::skipValue()
{
	Code code = *p++;
	if (code >= MinVal && code <= MaxVal) {
		return;
	} else if (code >= GetVar + MinVar && code <= GetVar + MaxVar) {
		return;
	} else if (code >= Add && code <= Max) {
		skipValue();
		skipValue();
		return;
	} else if (code == Mix) {
		skipValue();
		skipValue();
		skipValue();
		return;
	} else if (code == Cond) {
		skipCondition();
		skipValue();
		skipValue();
		return;
	}
}


/*-----------------------------------------------------------------------*/

QColor AbstractFactory::evalColor()
{
	Code code = *p++;
	switch (code) {
		case RGB: {
			const quint8 *c = (const quint8 *) p;
			p += 3;
			return QColor(c[0], c[1], c[2]);
		}
		case RGBA: {
			const quint8 *c = (const quint8 *) p;
			p += 4;
			return QColor(c[0], c[1], c[2], c[3]);
		}
		case RGBAf: {
			qreal v[4];
			for (int n = 0; n < 4; ++n) {
				v[n] = qMin(qMax(qreal(0), evalValue()), qreal(1));
			}
			return QColor::fromRgbF(v[0], v[1], v[2], v[3]);
		}
		case Blend: {
			QColor color0 = evalColor();
			QColor color1 = evalColor();
			return blend_color(color0, color1, evalValue());
		}
		case Palette: {
			if (opt) {
				return opt->palette.color(QPalette::ColorRole(*p++));
			}
			break;
		}
		case Shade: {
			QColor color = evalColor();
			return shaded_color(color, int(evalValue() * 200));
		}
		case Darker: {
			QColor color = evalColor();
                        return color.darker(*p++);
                }
		case Lighter: {
			QColor color = evalColor();
                        return color.lighter(*p++);
                }
		default:
			break;
	}
	return QColor();
}


void AbstractFactory::skipColor()
{
	Code code = *p++;
	switch (code) {
		case RGB: {
			p += 3;
			return;
		}
		case RGBA: {
			p += 4;
			return;
		}
		case RGBAf: {
			for (int n = 0; n < 4; ++n) {
				skipValue();
			}
			return;
		}
		case Blend: {
			skipColor();
			skipColor();
			skipValue();
			return;
		}
		case Palette: {
			++p;
			return;
		}
		case Shade: {
			skipColor();
			skipValue();
			return;
		}
		case Darker:
		case Lighter: {
			skipColor();
			p++;
			return;
		}
		default:
			break;
	}
}


/*-----------------------------------------------------------------------*/

void AbstractFactory::executeCode(Code code)
{
	if (code >= SetVar + MinVar && code <= SetVar + MaxVar) {
		var[code - SetVar] = evalValue();
	} else switch (code) {
		case Begin: {
			while (*p != End) {
				Code code = *p++;
				executeCode(code);
			}
			++p;
			return;
		}
		case While: {
			const Code *loop_p = p;
			int counter = 100; // prevent infinite loop
			while (evalCondition() && --counter >= 0) {
				Code code = *p++;
				executeCode(code);
				p = loop_p;
			}
			Code code = *p++;
			skipCode(code);
			return;
		}
		case If: {
			if (evalCondition()) {
				Code code = *p++;
				executeCode(code);
				if (*p == Else) {
					++p;
					Code code = *p++;
					skipCode(code);
				}
			} else {
				Code code = *p++;
				skipCode(code);
				if (*p == Else) {
					++p;
					Code code = *p++;
					executeCode(code);
				}
			}
			return;
		}
	}
}


void AbstractFactory::skipCode(Code code)
{
	if (code >= SetVar + MinVar && code <= SetVar + MaxVar) {
		skipValue();
	} else switch (code) {
		case Begin: {
			while (*p != End) {
				Code code = *p++;
				skipCode(code);
			}
			++p;
			return;
		}
		case While: {
			skipCondition();
			Code code = *p++;
			skipCode(code);
			return;
		}
		case If: {
			skipCondition();
			Code code = *p++;
			skipCode(code);
			if (*p == Else) {
				++p;
				Code code = *p++;
				skipCode(code);
			}
			return;
		}
	}
}


/*-----------------------------------------------------------------------*/

void AbstractFactory::create()
{
	if (p != 0) {
		while (*p != End) {
			Code code = *p++;
			executeCode(code);
		}
	}
}



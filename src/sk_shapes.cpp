/*
 * skulpture_shapes.cpp
 *
 */

#include "skulpture_p.h"
#include "sk_factory.h"


/*-----------------------------------------------------------------------*/
/**
 * ShapeFactory - create a QPainterPath from a description
 *
 * The shape description is a bytecode stream that allows simple arithmetic,
 * conditionals, and looping.
 *
 * Syntax is as follows:
 *	Description:	Instruction* End
 *	Instruction:	Close | Move X Y | Line X Y | Quad X1 Y1 X2 Y2 | Cubic X1 Y1 X2 Y2 X3 Y3
 *	Instruction:	Nop | SetVar X | Begin Instruction* End | While Condition Instruction | If Condition Instruction1 [ Else Instruction2 ]
 *	Condition:	Not Condition | And Condition1 Condition2 | Or Condition1 Condition2 | EQ X Y | NE X Y | LT X Y | GE X Y | GT X Y | LE X Y
 *	Expression:	Number | GetVar | Add X Y | Sub X Y | Mul X Y | Div X Y | Min X Y | Max X Y | Mix V X Y | Cond Condition X Y
 *
 * TODO
 *
 * Mod, MultiLine, DoWhile, Integer/Float, Switch, Exp, Sin, Cos, Tan, Atan, Atan2, Pi
 * Colors, Gradients, Functions (Call/Ret), Frames, Text
 *
 */


/*-----------------------------------------------------------------------*/

void ShapeFactory::executeCode(Code code)
{
	qreal v[6];

	switch (code) {
		case Move:
		case Line:
			v[0] = evalValue();
			v[1] = evalValue();
			if (code == Move) {
				path.moveTo(v[0], v[1]);
			} else {
				path.lineTo(v[0], v[1]);
			}
			break;
		case Close:
			path.closeSubpath();
			break;
		case Quad:
		case Cubic: {
			for (int n = 0; n < (code == Quad ? 4 : 6); ++n) {
				v[n] = evalValue();
			}
			if (code == Quad) {
				path.quadTo(v[0], v[1], v[2], v[3]);
			} else {
				path.cubicTo(v[0], v[1], v[2], v[3], v[4], v[5]);
			}
			break;
		}
		default:
			AbstractFactory::executeCode(code);
			break;
	}
}


void ShapeFactory::skipCode(Code code)
{
	switch (code) {
		case Move:
		case Line:
			skipValue();
			skipValue();
			break;
		case Close:
			break;
		case Quad:
		case Cubic: {
			for (int n = 0; n < (code == Quad ? 4 : 6); ++n) {
				skipValue();
			}
			break;
		}
		default:
			AbstractFactory::skipCode(code);
			break;
	}
}


/*-----------------------------------------------------------------------*/

QPainterPath ShapeFactory::createShape(ShapeFactory::Description description, qreal var[])
{
	ShapeFactory factory;

	factory.setDescription(description);
	for (int n = MinVar; n <= MaxVar; ++n) {
		factory.setVar(n, var[n]);
	}
	factory.create();
	for (int n = MinVar; n <= MaxVar; ++n) {
		var[n] = factory.getVar(n);
	}
	return factory.getPath();
}


QPainterPath ShapeFactory::createShape(ShapeFactory::Description description)
{
	ShapeFactory factory;

	factory.setDescription(description);
	factory.create();
	return factory.getPath();
}



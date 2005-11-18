#include "CSSTools.h"	// Needed for CSSTOOLS_API
#include <sstream>	// Needed for ostringstream
#include <iomanip>	// Needed for iosflags

/* 
	This header replaces the string formatting utility class called Fmt.
	It accepts any basic type such as:
	char, unsigned char, char*, const char*, 
	int, unsigned int, long, short, 
	float, double, 
	bool
	  
	and returns a string representation of the value.
	
	Some extra functionality was added to allow the formatting of double/float values,
	change the base (octal, decimal or hexadecimal) of integers and padding of strings
	with any supplied character supplied length and alignment.

	Prototypes:
	string ToStr(variable)
	string ToStr(float/double, Style style, int prec) 
	string Pad(string text, int length, char padChar, Align align)
	string ToStr(integer, int base=0)

	
*/

#ifndef __TOSTR__INCLUDED__
#define __TOSTR__INCLUDED__

// The following enum's are needed in order to be backward compatible
enum CSSTOOLS_API Style { Sci, Fix, Auto };
enum CSSTOOLS_API Align { Left, Right };

inline std::string ToStr(float r, Style style, int prec=0) 
{ 
	std::ostringstream buf; 

	// Determine which floating-point representation to use
	if(style==Sci) 
	{
    buf << std::setiosflags(std::ios::scientific);
	}
	else if(style==Fix) 
	{
		buf << std::setiosflags(std::ios::fixed);
	}
	else if(style==Auto) 
	{
		// No documentation was available as to what "Auto" means exactly
		// We reset the formatting flags of the stream to C++ defaults
    buf << std::resetiosflags(std::ios::fixed | std::ios::scientific);
	}

	// Set precision of floating-point number and read in a double
	buf << std::setprecision(prec) << r;

	return buf.str();
}

inline std::string ToStr(double r, Style style, int prec=0) 
{ 
	std::ostringstream buf; 

	// Determine which floating-point representation to use
	if(style==Sci) 
	{
		buf << std::setiosflags(std::ios::scientific);
	}
	else if(style==Fix) 
	{
		buf << std::setiosflags(std::ios::fixed);
	}
	else if(style==Auto) 
	{
		// No documentation was available as to what "Auto" means exactly
		// We reset the formatting flags of the stream
		buf << std::resetiosflags(std::ios::fixed | std::ios::scientific);
	}

	// Set precision of floating-point number and read in a double
	buf << std::setprecision(prec) << r;

	return buf.str();
}

inline std::string ToStr(char *c)
{ 
	std::ostringstream buf; 
	buf << c;
	return buf.str();
}

inline std::string ToStr(const char *c)
{
	std::ostringstream buf; 
	buf << c;
	return buf.str();
}

inline std::string ToStr(long i, int base=10)
{
	std::ostringstream buf; 
	
	// Determining which base to use: 8, 10, 16
	if(base==8)
	{
		buf << std::oct << i;
	}
	else if(base==10) 
	{
		buf << std::dec << i;
	} 
	else if(base==16)
	{
		buf << std::hex << i;
	}

	return buf.str();

}

inline std::string ToStr(short i, int base=10)
{
	std::ostringstream buf; 

	// Determining which base to use: 8, 10, 16
	if(base==8)
	{
		buf << std::oct << i;
	}
	else if(base==10) 
	{
		buf << std::dec << i;
	} 
	else if(base==16)
	{
		buf << std::hex << i;
	}

	return buf.str();
}

inline std::string ToStr(int i, int base=10)
{ 
	std::ostringstream buf; 
	
	// Determining which base to use: 8, 10, 16
	if(base==8)
	{
		buf << std::oct << i;
	}
	else if(base==10) 
	{
		buf << std::dec << i;
	} 
	else if(base==16)
	{
		buf << std::hex << i;
	}

	return buf.str();
}
	
inline std::string ToStr(unsigned int i, int base=10)
{
	std::ostringstream buf; 

	// Determining which base to use: 8, 10, 16
	if(base==8)
	{
		buf << std::oct << i;
	}
	else if(base==10) 
	{
		buf << std::dec << i;
	} 
	else if(base==16)
	{
		buf << std::hex << i;
	}

	return buf.str();
}

inline std::string ToStr(bool b) 
{
	// Return a string representation of true (1) or false (0)
	if (b) {
		return std::string("TRUE");
	} else {
		return std::string("FALSE");
	};
}

inline std::string ToStr(char c)
{ 
	std::ostringstream buf; 
	buf << c;
	return buf.str();
}

inline std::string ToStr(unsigned char c)
{ 
	std::ostringstream buf; 
	buf << c;
	return buf.str();
}

inline std::string ToStr(float f)
{
	std::ostringstream buf; 
	buf << f;
	return buf.str();
}

inline std::string ToStr(double d)
{
	std::ostringstream buf; 
	buf << d;
	return buf.str();
}

inline std::string Pad(std::string text, int length, char padChar=' ', Align align=Right)
{
	std::ostringstream buf;

	// Set fill character and the length of the string
	buf << std::setfill(padChar) << std::setw(length);
	
	// Determining how to align the text
	if(align == Left)
	{
		buf << std::setiosflags(std::ios::left);
	}
	else
	{
		buf << std::setiosflags(std::ios::right);
	}

	buf << text;

	return buf.str();
}

#endif
#pragma once
#ifndef _CONFIG_H
#define _CONFIG_H

typedef unsigned char BYTE;
typedef signed char SIGNED_BYTE;
typedef unsigned short int WORD;
typedef signed short int SIGNED_WORD ;

template< typename typeData >
inline bool TestBit( typeData inData, int inBitPosition )
{
	typeData lMsk = 1 << inBitPosition ;
	return ( inData & lMsk ) ? true : false ;
}

template< typename typeData >
inline typeData BitGet( typeData inData, int inBitPosition )
{
	typeData lMsk = 1 << inBitPosition ;
	return lMsk ;
}

template< typename typeData >
inline typeData BitGetVal( typeData inData, int inBitPosition )
{
	typeData lMsk = 1 << inBitPosition ;
	return ( inData & lMsk ) ? 1 : 0 ;
}

template< typename typeData >
inline typeData BitSet( typeData inData, int inBitPosition )
{
	typeData lMsk = 1 << inBitPosition ;
	inData |= lMsk ;
	return inData ;
}

template< typename typeData >
inline typeData BitReset( typeData inData, int inBitPosition )
{
	typeData lMsk = 1 << inBitPosition ;
	inData &= ~lMsk ;
	return inData ;
}

template< typename typeData >
int BitCount (typeData inData, int totalBits )
{
	int res = 0 ;
	for (int i = 0 ; i < totalBits; i++)
	{
		if (TestBit(inData,i))
			res++ ;
	}
	return res ;
}

#endif

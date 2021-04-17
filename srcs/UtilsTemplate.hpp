#pragma once

namespace ft
{

template	<typename T>
T			hton(T x)
{
	#if BYTE_ORDER == LITTLE_ENDIAN
	if (sizeof(x) == 2)
	{
		return ((x & 0xff) << 8) | ((x & 0xff00) >> 8);
	}
	else if (sizeof(x) == 4)
	{
		return \
			  ((x & 0x000000ff) << 24) \
			| ((x & 0x0000ff00) <<  8) \
			| ((x & 0x00ff0000) >>  8) \
			| ((x & 0xff000000) >> 24);
	}
	else if (sizeof(x) == 8)
	{
		return \
			  ((x & 0x00000000000000ffull) << 56) \
			| ((x & 0x000000000000ff00ull) << 40) \
			| ((x & 0x0000000000ff0000ull) << 24) \
			| ((x & 0x00000000ff000000ull) <<  8) \
			| ((x & 0x000000ff00000000ull) >>  8) \
			| ((x & 0x0000ff0000000000ull) >> 24) \
			| ((x & 0x00ff000000000000ull) >> 40) \
			| ((x & 0xff00000000000000ull) >> 56);
	}
	#else
	return x;
	#endif
}

}	//end ft
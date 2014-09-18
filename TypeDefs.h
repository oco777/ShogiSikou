//
//  TypeDefs.h
//  MySampleProject
//
//  Created by 小森久 on 2014/06/22.
//
//

#ifndef MySampleProject_TypeDefs_h
#define MySampleProject_TypeDefs_h

typedef signed char INT8;
typedef unsigned char UINT8;
typedef signed short INT16;
typedef unsigned short UINT16;
typedef signed int INT32;
typedef unsigned int UINT32;

#if defined(WINDOWS)
typedef __int64 INT64;
typedef unsigned __int64 UINT64;
#define nullptr	NULL
#else // defined(WINDOWS).
typedef long long INT64;
typedef unsigned long long UINT64;
#endif // defined(WINDOWS).

#define ASSERT	assert

#endif

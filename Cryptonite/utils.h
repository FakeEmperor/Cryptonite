#pragma once
#define _USE_MATH_DEFINES
#define _CRT_SECURE_NO_WARNINGS
#include <string>
#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <ctype.h>
#include <sstream>
#include <cstdint>
#include <cmath>
#include <ctime>
#include <vector>
#include <map>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <fcntl.h>
#include <sstream>
#include <algorithm>
/****C++11 ONLY Definitions*****/
#if ((__cplusplus==201103L)||(__cplusplus==199711L))
#include <thread>
#include <mutex>
#include <typeinfo>
#include <memory>
#define __HAS_C11
#else
#warning "Warning C11: [Utils.h] Some functions may fail"
#endif
/* COMPATABILITY SECTOR IN INCLUDES*/
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS)
#include <Windows.h>
#include <process.h>
#define __WINDOWS_FOREVAH
//High-precision timer
#include <intrin.h>
#elif defined(_LINUX) || defined(LINUX)
#define __WINDOWS_MUSTDIE
#include <unistd.h>
int Sleep(int sleepMs) { return usleep(sleepMs * 1000); }
#else
#error Unknown OS
#endif




/**** OS DEPENDANT DECLARATIONS ****/
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS)


#elif defined(_LINUX) || defined(LINUX)
#error Linux version of Threading system is not implemented
#else
#error Unknown OS
#endif
typedef unsigned long long int u_llong;





//Useful functions for 
unsigned long long int random(unsigned long long int max_exclusive = 0);

unsigned __int64 rdtsc();

std::string xorStrings(const std::string &a, const std::string &b);

size_t ignoreWhitespaces(const std::string &str, const size_t pos);

DWORD makeARGB(byte r, byte g, byte b, byte alpha);
COLORREF argb2Colorref(DWORD argb);

template <class T>
void toChars(char *buf, const T &a){
	memcpy(buf, &a, sizeof(a)); //shallow copy
};
template <class T>
void fromChars(const char *buf, T &a){
	memcpy(&a, buf, sizeof(a)); //shallow paste
};

namespace Utils{
	/***** FORWARD DECLARATIONS ********/
	class Storage;
	
	/********** FUNCTIONS ************/
	/**
	*	@brief Clears the default command-line screen
	**/
	void clearScreen();

	
	/***********VARIABLES ***********/
	static bool __RAND_INITED = false;

	

	class StorageCell{
		void* data;
		size_t sz;
		size_t id;
		HANDLE mutex;
	public:
		StorageCell();
		StorageCell(void* data, size_t size, size_t id = -1);

		void hold();
		void release();

		inline size_t getSize();
		inline void* getData();

		friend class Storage;
	};

	class Storage{
		static std::vector<StorageCell> data;
		static size_t last_id;
		static bool initialized;
		static HANDLE v_mutex;
	public:
		Storage();
		inline size_t add(void *data, size_t size);
		inline size_t remove(size_t id);
		inline StorageCell& get(size_t id);
		/*******FIND FUNCTIONS*********/
	};

	



};
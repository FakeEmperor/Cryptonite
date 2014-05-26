#include "utils.h"




/******** COMPILER DEPENDANT DEFINITIONS *******/
#ifdef _MSC_VER
#pragma intrinsic(__rdtsc)
unsigned __int64 rdtsc() {
	return __rdtsc();
}
#else 
__inline__ uint64_t rdtsc() {
	uint64_t x;
	__asm__ volatile ("rdtsc\n\tshl $32, %%rdx\n\tor %%rdx, %%rax" : "=a" (x) : : "rdx");
	return x;
}
#endif

/******* JUST USEFUL FUNCTIONS *******/
unsigned long long int random(unsigned long long int max_exclusive){
	if (!Utils::__RAND_INITED){
		srand(time(NULL));
		Utils::__RAND_INITED = true;
	}
	return (!max_exclusive) ? rand()*rand() : (rand()*rand() % max_exclusive);
};

std::string xorStrings(const std::string &a, const std::string &b){
	std::string res;
	size_t l = min(b.length(), a.length());
	for (size_t i = 0; i < l; i++)
		res.append(1, (char)(b[i] ^ a[i]));
	return res;
}

void Utils::clearScreen(){
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS)
	COORD topLeft = { 0, 0 };
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO screen;
	DWORD written;

	GetConsoleScreenBufferInfo(console, &screen);
	FillConsoleOutputCharacterA(
		console, ' ', screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
	FillConsoleOutputAttribute(
		console, FOREGROUND_GREEN | FOREGROUND_RED | FOREGROUND_BLUE,
		screen.dwSize.X * screen.dwSize.Y, topLeft, &written
		);
	SetConsoleCursorPosition(console, topLeft);
#elif defined(LINUX)
	std::cout << "\x1B[2J\x1B[H";
#endif
}

size_t ignoreWhitespaces(const std::string &str, const size_t pos){
	size_t s = str.length(), i;
	if (pos >= s)
		return -1;
	for (i = pos; i < s; i++)
		if (!isspace(str[i]))
			return i;
	return i;
}


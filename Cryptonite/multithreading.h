#pragma once
#include <string>

//Include for events, threads etc.
#include <intrin.h>
#include <ctime>
#include <process.h>
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS)
#include <Windows.h>
#else
#error Error 42: Unix-based systems are not supported. Sorry.
#endif
//Include for memory
#if !((__cplusplus==201103L)||(__cplusplus==199711L))
#include <memory>
template <typename T> 
using __SharedPointer = std::shared_ptr<T>;
#define __HAS_C11
#else
template<typename T>
class __SharedPointer {
	T *data;
	long int *counter;
protected:
	void inc() {
		if (!this->counter)
			this->counter = new long(1);
		else
			++*counter;
	};
	void dec() const {
		//check if counter exists, so it can be deleted
		if (counter){
			//decrement the number of references
			--*counter;
			//if the data is no longer needed - delete it
			if (!*counter) {
				delete counter;
				delete data;
			}
		}
	};
public:
	//constructors
	__SharedPointer(): data(NULL), counter(NULL){
		
	};
	__SharedPointer(T *value_ptr): data(value_ptr), counter(new long(1)) {


	};
	__SharedPointer(const T& value): data(new T(value)), counter(new long(1)){

	};
	__SharedPointer(const __SharedPointer<T> &ptr): data(ptr.data), counter(ptr.counter) {
		inc();
	}
	//<OPERATORS>
	//assignement
	__SharedPointer<T>& operator = (const __SharedPointer<T> &ptr){
		if (this != &ptr&&*this != ptr) {
			this->dec();
			memcpy(this, &ptr, sizeof(ptr));
			this->inc();
		}
		return *this;
	}
	//dereferencing
	T* operator -> () const{
		return this->data;
	}
	T& operator *() const{
		return *this->data;
	}
	//check null
	operator bool() const{
		return this->data != NULL;
	}
	//check equality
	bool operator == (const T& val) const{
		return (!this->data)?false:memcmp(&val, this->data, sizeof(T));
	}
	bool operator != (const T& val) const{
		return !this->operator==(val);
	}
	bool operator == (const __SharedPointer<T> &ptr) const{
		return ptr.data == this->data;
	}
	bool operator != (const __SharedPointer<T> &ptr) const{
		return !this->operator==(ptr);
	}
	//</OPERATORS>
	//getters
	long int use_count() const{
		return this->counter?*this->counter:0;
	}
	//destructors
	~__SharedPointer(){
		this->dec();
	}
	

};
#endif

namespace Multithreading{

	/***** FORWARD DECLARATIONS ********/
	class Thread;
	class Event;

	/********* CUSTOM TYPES ***********/


	enum EventType { EV_TRACING = 0x01, EV_WARNING, EV_ERROR, EV_SYSINFO, EV_NONE = 0x00 };
	enum ThreadStatus {
		TS_ERROR = 0x01, TS_EMPTY = 0x02, TS_CREATED = 0x04,
		TS_PAUSED = 0x08, TS_WORKING = 0x10, TS_FINISHED = 0x20,
		TS_USER_SIGNAL = 0x40,
		TS_NONE = 0x00, TS_ALL = 0xFF,

	};
	enum ThreadResult { TR_SUCCESS = 0, TR_HALTED = 1, TR_ERROR = 2, TR_NOWORK = 4 };
	enum ThreadEventCode {
		/*SYSINFO Events*/
		TEC_CREATED = 0x00, TEC_STOPPING = 0x01, TEC_FINISHED = 0x02,
		/* ERROR Codes */
		TEC_CREATION_FAILED = 0x0100,
		/* WARNING Codes */
		TEC_MUTEX_ABANDONED = 0x010000
	};
	enum ThreadOption { TO_INVALID = 0x10, TO_NONE = 0x00, TO_DETACHED = 0x01, TO_SINGLETON = 0x02 };

	typedef ThreadResult(__stdcall *ThreadFunction)(Thread*, void*);

	/**
	*	@brief Entry function-wrapper for Thread class.
	*	@param arg [in] - Thread object, that is the holder for created thread HANDLE.
	**/
	extern unsigned int __stdcall defThreadFunction(void *arg);
	extern int setJoin(Thread &what, const Thread &to);
	//DELETED: Under revision of usage
	//extern unsigned int __stdcall defPauseUntilFunction(void *arg);
	
	struct EventInfo{
		EventType type;
		long long int code;
		std::string str;
		void* data;
		mutable HANDLE handle;
		mutable HANDLE mutex;
		//constructor
		EventInfo();
		EventInfo(const EventType type, const long long int code, const std::string &str,
										void* data, const HANDLE handle, const HANDLE mutex);
		~EventInfo();
	};

	class Event{
		__SharedPointer<EventInfo> info;

	public:
		/** CLASS CONSTRUCTORS **/
		Event();
		Event(EventType type, long long int code = 0,const std::string &event_string = "", void* data = nullptr);
		Event(const Event& e); //copy semantics
#ifdef __HAS_C11
		Event(const Event&& e); //move semantics (C++11 only)
#endif
		Event& operator = (const Event &ev);
		/** DESTRUCTORS **/
		~Event();
		/** GETTERS **/
		EventType getType() const;
		long long int getCode() const;
		std::string getStr() const;
		void* getData() const;
		const HANDLE getHandle() const;
		/** SETTERS **/
		int setData(void* data);
		int set(int setMask, EventType type = EV_NONE, long long int code = 0, std::string event_string = "", void* data = nullptr);
		int set(int setMask, const Event &e);
		/** CONTROLS **/
		int triggerEvent();
		int untriggerEvent();
	};

	struct ThreadInfo {
		ThreadFunction f;
		HANDLE thread;
		void* data;
		size_t t_id;
		size_t options;
		size_t stack_size;
		HANDLE res_mutex;

		ThreadStatus status;
		HANDLE st_ev;

		Event recent;
		bool event_locked;

		int th_errno;


		/** STATIC MEMBERS AND CONSTANTS **/
		/** Default stack size for a thread	**/
		static const size_t defStackSize = 65536;
		/**	Default maximum wait delay for writing operations until timeout, in milliseconds. **/
		static const size_t writeMaxDelay = 200;
		/**	Default maximum wait delay for reading operations until timeout, in milliseconds. **/
		static const size_t readMaxDelay = 100;
		/**	Default maximum wait delay for thread to close after destructor is called **/
		static const size_t closeMaxDelay = 50;
		/**	Default maximum wait delay for thread to close after stop() method is called **/
		static const size_t stopMaxDelay = 10;


		//constructor
		ThreadInfo();
		ThreadInfo(ThreadFunction f, const HANDLE thread, void* data, const size_t t_id, const size_t options,
					const size_t stack_size, const HANDLE res_mutex, const ThreadStatus status,
					HANDLE st_ev, const Event &recent, const bool event_locked,const int th_errno);
		//destructor
		~ThreadInfo();
	};

	class Thread{
		__SharedPointer<ThreadInfo> info;
	protected:
		void setStatus(ThreadStatus ts);
		void syncStatus();
		void setLockEvent(bool lock_status = true);

		/** FRIEND FUNCTIONS **/
		friend unsigned int __stdcall defThreadFunction(void* arg);
		friend unsigned int __stdcall defPauseUntilFunction(void *arg);
		friend int setJoin(Thread &what, const Thread &to);
	public:
		


		/** CLASS CONSTRUCTORS **/
		Thread();
		Thread(ThreadFunction f, void* data = nullptr, bool autostart = false,
				size_t stack_size = ThreadInfo::defStackSize, size_t opts = TO_NONE);
		Thread(const Thread &thr);
#ifdef __HAS_C11
		Thread(Thread &&thr);
#endif
		/** OPERATORS **/
		Thread& operator = (Thread &thread);
		Thread& operator >> (const Thread &thread);
		Thread& operator << (const Thread &thread);

		/** DESTRUCTORS **/
		~Thread();

		/** ASSIGNATIONS **/
		int assign(ThreadFunction f, void* data = nullptr, size_t stack_size = ThreadInfo::defStackSize);

		/** CONTROLS **/
		int start();
		int stop(size_t wait = ThreadInfo::stopMaxDelay);
		int kill();
		int pause();
		int resume();
		int pauseUntil(DWORD milliseconds, const Thread *thread = nullptr, ThreadStatus status = TS_NONE);

		/** SYNCHRONISATIONS **/
		int join() const;
		int detach();

		Event waitForEvent(DWORD milliseconds = 0) const;
		int waitForStatus(DWORD milliseconds = 0, ThreadStatus status = TS_NONE) const;

		/** EVENTING **/
		int sendEvent(const Event &e);
		const Event& getEvent() const;

		/** SETTERS **/
		//DELETED: setOptions() function is redundant
		//size_t setOptions(size_t options);//

		//DELETed: Under revision of usefullness
		//int makeSingleton();

		/** GETTERS **/
		size_t getOptions() const;
		size_t getThreadId() const;
		ThreadStatus getStatus() const;
		int getLastError() const;
	};
}
#include "multithreading.h"
/*********** EVENTINFO Class (info for event class) ***************/
Multithreading::EventInfo::EventInfo():
type(EV_NONE), code(0), data(NULL), handle((HANDLE)CreateEvent(NULL, FALSE, TRUE, NULL)), mutex((HANDLE)CreateMutex(NULL, FALSE, NULL))
{

};
Multithreading::EventInfo::EventInfo(const EventType type, const long long int code, const std::string &str,
	void* data, const HANDLE handle, const HANDLE mutex) 
	:type(type), code(code), str(str), data(data), handle(handle), mutex(mutex)
{

};
Multithreading::EventInfo::~EventInfo(){
	if (this->handle) CloseHandle(this->handle);
	if (this->mutex) CloseHandle(this->mutex);

};

/********** EVENT Class ***********/
Multithreading::Event::Event():info(new EventInfo()){
	
};
Multithreading::Event::Event(Multithreading::EventType type, long long int code,const std::string &str, void* data):
info(new EventInfo(type, code, str, data, (HANDLE)CreateEvent(NULL, FALSE, TRUE, NULL), (HANDLE)CreateMutex(NULL, FALSE, NULL))){
	
};
Multithreading::Event::Event(const Multithreading::Event &e):info(e.info){
	
};
#ifdef __HAS_C11
Multithreading::Event::Event(const Multithreading::Event&& e):info(e.info){
	e.info->handle = NULL;
	e.info->mutex = NULL;
}
#endif

Multithreading::Event& Multithreading::Event::operator = (const Multithreading::Event &e){
	this->info = e.info;
	return *this;
}

Multithreading::EventType Multithreading::Event::getType()const{
	DWORD r = WaitForSingleObject(this->info->mutex, Multithreading::ThreadInfo::readMaxDelay);
	if (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED){
		Multithreading::EventType res = this->info->type;
		ReleaseMutex(this->info->mutex);
		return res;
	}
	else {
		ReleaseMutex(this->info->mutex);
		return Multithreading::EV_NONE;
	}
}
long long int Multithreading::Event::getCode()const{
	DWORD r = WaitForSingleObject(this->info->mutex, Multithreading::ThreadInfo::readMaxDelay);
	if (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED){
        DWORD res = this->info->code;
		ReleaseMutex(this->info->mutex);
		return res;
	}
	else {
		ReleaseMutex(this->info->mutex);
		return Multithreading::EV_NONE;
	}
}
std::string Multithreading::Event::getStr()const{
	DWORD r = WaitForSingleObject(this->info->mutex, Multithreading::ThreadInfo::readMaxDelay);
	if (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED){
		std::string res = this->info->str;
		ReleaseMutex(this->info->mutex);
		return res;
	}
	else {
		ReleaseMutex(this->info->mutex);
		return "";
	}
}
void* Multithreading::Event::getData()const{
	DWORD r = WaitForSingleObject(this->info->mutex, Multithreading::ThreadInfo::readMaxDelay);
	if (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED){
		void* res = this->info->data;
		ReleaseMutex(this->info->mutex);
		return res;
	}
	else {
		ReleaseMutex(this->info->mutex);
		return NULL;
	}
}
const HANDLE Multithreading::Event::getHandle()const{
	DWORD r = WaitForSingleObject(this->info->mutex, Multithreading::ThreadInfo::readMaxDelay);
	if (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED){
		void* res = this->info->handle;
		ReleaseMutex(this->info->mutex);
		return res;
	}
	else {
		ReleaseMutex(this->info->mutex);
		return NULL;
	}
}

int Multithreading::Event::setData(void* data){
	DWORD r = WaitForSingleObject(this->info->mutex, Multithreading::ThreadInfo::writeMaxDelay);
	if (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED){
		this->info->data = data;
		ReleaseMutex(this->info->mutex);
		return 0;
	}
	else {
		ReleaseMutex(this->info->mutex);
		return 1;
	}
}
int Multithreading::Event::set(int setMask, Multithreading::EventType type, long long int code, std::string event_string, void* data){
	DWORD r = WaitForSingleObject(this->info->mutex, Multithreading::ThreadInfo::writeMaxDelay);
	if (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED){
		if (setMask & 1) this->info->type = type;
		if (setMask & 2) this->info->code = code;
		if (setMask & 4) this->info->str = event_string;
		if (setMask & 8) this->info->data = data;
		ReleaseMutex(this->info->mutex);
		return 0;
	}
	else{
		ReleaseMutex(this->info->mutex);
		return setMask;
	}

};
int Multithreading::Event::set(int setMask, const Multithreading::Event &e){
	DWORD r = WaitForSingleObject(this->info->mutex, Multithreading::ThreadInfo::writeMaxDelay);
	if (r == WAIT_OBJECT_0 || r == WAIT_ABANDONED){
		if (setMask & 1) this->info->type = e.info->type;
		if (setMask & 2) this->info->code = e.info->code;
		if (setMask & 4) this->info->str = e.info->str;
		if (setMask & 8) this->info->data = e.info->data;
		ReleaseMutex(this->info->mutex);
		return 0;
	}
	else{
		ReleaseMutex(this->info->mutex);
		return setMask;
	}

};

int Multithreading::Event::triggerEvent(){
	DWORD waitRes = WaitForSingleObject(this->info->mutex, Multithreading::ThreadInfo::readMaxDelay);
	if (waitRes == WAIT_OBJECT_0 || waitRes == WAIT_ABANDONED){
		int res = (this->info->mutex) ? (SetEvent(this->info->handle) ? 0 : 1) : 1;
		ReleaseMutex(this->info->mutex);
		return res;
	}
	else {
		ReleaseMutex(this->info->handle);
		return 1;
	}

};
int Multithreading::Event::untriggerEvent(){
	if (this->info->handle)
		return ResetEvent(this->info->handle) ? 0 : 1;
	else
		return 1;
};

Multithreading::Event::~Event(){
	
}


/*********** THREADINFO Class (info for event class) ***************/
Multithreading::ThreadInfo::ThreadInfo() :
f(NULL), data(NULL), t_id(0), thread(NULL), options(0), status(Multithreading::TS_EMPTY),st_ev( (HANDLE)CreateEvent(NULL, 1, 0, NULL)),
stack_size(0),res_mutex((HANDLE)CreateMutex(NULL, 0, NULL)),recent(EV_NONE), event_locked(false)
{

};
Multithreading::ThreadInfo::ThreadInfo(ThreadFunction f, const HANDLE thread, void* data, const size_t t_id,
										const size_t options, const size_t stack_size, const HANDLE res_mutex,
										const ThreadStatus status, HANDLE st_ev, const Event &recent,
										const bool event_locked, const int th_errno):
										f(f), data(data), t_id(t_id), thread(thread), options(options),
										status(status), st_ev(st_ev),
										stack_size(stack_size), res_mutex(res_mutex),
										recent(recent), event_locked(event_locked)
{

}
Multithreading::ThreadInfo::~ThreadInfo(){
	if (this->status != TS_NONE){
		if(this->st_ev)
			CloseHandle(this->st_ev);
		if (this->res_mutex)
			ReleaseMutex(this->res_mutex);
		if (this->res_mutex)
			CloseHandle(this->res_mutex);
		if (this->thread)
			CloseHandle(this->thread);

	}
};


/********** THREAD Class ***********/

unsigned int __stdcall Multithreading::defThreadFunction(void *arg){
	Multithreading::Thread* th = (Multithreading::Thread*)arg;
	//check if the thread already stopping
	//Obtained temporary event variable
	if (th->info->event_locked){
		Multithreading::Event temp_e = th->getEvent();
		if (temp_e.getType() == Multithreading::EV_SYSINFO && temp_e.getCode() == 1) {
			th->setStatus(Multithreading::TS_FINISHED);
			return Multithreading::TR_NOWORK;
		}
	}
	th->setStatus(Multithreading::TS_WORKING);
	ThreadResult res = th->info->f(th, th->info->data);
	WaitForSingleObject(th->info->res_mutex, ThreadInfo::readMaxDelay);
	th->setStatus(Multithreading::TS_FINISHED);
	ReleaseMutex(th->info->res_mutex);
	return (unsigned int)res;

};
int Multithreading::setJoin(Multithreading::Thread &what, const Multithreading::Thread &to) {
	size_t opts = what.getOptions(), opts_t = to.getOptions();
	if (opts_t == -1 || opts == -1)
		return EFAULT;
	else if ((opts | opts_t)&Multithreading::TO_DETACHED)
		return EINVAL;
	else if (&what == &to)
		return ECANCELED;
	what.pauseUntil(INFINITE, &to, Multithreading::TS_FINISHED);
	return 0;
}

unsigned int __stdcall Multithreading::defPauseUntilFunction(void *arg){



	return 0;
}


void Multithreading::Thread::setStatus(ThreadStatus ts){
	DWORD r = WaitForSingleObject(this->info->res_mutex, ThreadInfo::readMaxDelay);
	if (r == WAIT_ABANDONED || r == WAIT_OBJECT_0){
		this->info->status = ts;
		ReleaseMutex(this->info->res_mutex);
	}
	PulseEvent(this->info->st_ev);
};
void Multithreading::Thread::syncStatus(){
	//Check if thread is a) not created, b) running, c) suspended, d) finished
	const size_t wait_time = 5;
	//First things first: wait for the thread some small time
	DWORD res = WaitForSingleObject(this->info->thread, wait_time);
	//a
	if (this->info->t_id == 0 || this->info->thread == NULL || res == WAIT_FAILED) {
		this->info->status = Multithreading::TS_EMPTY;
		return;
	}
	//b
	if (res == WAIT_TIMEOUT && !(this->info->status == Multithreading::TS_CREATED || this->info->status == Multithreading::TS_WORKING)) {
		this->info->status = Multithreading::TS_CREATED;
	}

	//c
	/** USING UNDOCUMENTED FEATURE **/
	else if (res == WAIT_ABANDONED && this->info->status != TS_PAUSED) {
		this->info->status = TS_PAUSED;
	}
	/*^^^^ NOW ALL IS DOCUMENTED ^^^^*/
	//d
	else if (res == WAIT_OBJECT_0 && this->info->status != TS_FINISHED){
		this->info->status = TS_FINISHED;
	}
}
void Multithreading::Thread::setLockEvent(bool lock_status){
	this->info->event_locked = lock_status;
};

int Multithreading::Thread::sendEvent(const Event& e){
	DWORD result;
	if (!this->info->event_locked){
		result = WaitForSingleObject(this->info->res_mutex, Multithreading::ThreadInfo::writeMaxDelay);
		if (result == WAIT_ABANDONED || result == WAIT_OBJECT_0){
			/*if (result == WAIT_ABANDONED)*/ //send debug info to thread management system 
			this->info->recent.set(0x0F, e);
			this->info->recent.triggerEvent();
			ReleaseMutex(this->info->res_mutex);

			return 0;
		}
		else{
			ReleaseMutex(this->info->res_mutex);

			return result;
		}

	}
	else return 1;

};
const Multithreading::Event& Multithreading::Thread::getEvent() const{
	DWORD r = WaitForSingleObject(this->info->res_mutex, ThreadInfo::readMaxDelay);
	if (r == WAIT_ABANDONED || r == WAIT_OBJECT_0){
		auto &res = this->info->recent;
		ReleaseMutex(this->info->res_mutex);
		return res;
	}
	else{
		ReleaseMutex(this->info->res_mutex);
		//force return
		return this->info->recent;
	}

};

Multithreading::Thread::Thread(): info(new ThreadInfo()){
	
};
Multithreading::Thread::Thread(ThreadFunction f, void* data, bool autostart, size_t stack_size, size_t opts):
info(new ThreadInfo(f, NULL, data, 0, opts, stack_size, (HANDLE)CreateMutex(NULL, 0, NULL), TS_EMPTY,
					(HANDLE)CreateEvent(NULL, 1, 0, NULL),*new Event(EV_NONE),false, 0 ))
{
	this->info->thread = (HANDLE)_beginthreadex(NULL, stack_size, defThreadFunction,
						 (void*)this, autostart ? 0 : CREATE_SUSPENDED, &this->info->t_id);
	if (!this->info->thread){
		this->sendEvent(Event(Multithreading::EV_ERROR, errno, "Thread created unsuccessfully"));
		this->setStatus(Multithreading::TS_ERROR);
	}
	else this->setStatus(Multithreading::TS_CREATED);
};
Multithreading::Thread::Thread(const Multithreading::Thread &thread):info(thread.info){

}
#ifdef __HAS_C11
Multithreading::Thread::Thread(Multithreading::Thread &&thread):info(thread.info) {
	thread.info->status = TS_NONE;

}
#endif

Multithreading::Thread::~Thread(){
	//send stop signal, kill if thread is not responding
	//release all handles
	if (this->info.use_count() <= 1){
		if (this->getStatus() & (TS_CREATED | TS_WORKING | TS_PAUSED))
			if (this->stop(ThreadInfo::closeMaxDelay))
				this->kill();
	}
};

int Multithreading::Thread::assign(ThreadFunction f, void* data, size_t stack_size){
	size_t status = this->getStatus();
	if (status & (Multithreading::TS_EMPTY | Multithreading::TS_FINISHED | Multithreading::TS_CREATED )){
		
		//Well, if status is TS_CREATED, then we have to stop thread (or kill it), remove handle, and reset all variables
		if (status & (Multithreading::TS_FINISHED | Multithreading::TS_CREATED)){
			if (status == Multithreading::TS_CREATED)
				if (this->stop())
					this->kill();
		}
			

		//Just assign all values and create thread handle - easy
		HANDLE saved_mutex = this->info->res_mutex, saved_st_ev = this->info->st_ev;
		this->info->res_mutex = 0;
		this->info->st_ev = 0;
		this->info = *new ThreadInfo(f, NULL, data, 0, this->info->options, stack_size,
									 saved_mutex, TS_NONE, saved_st_ev, *new Event(EV_NONE), false, 0 );
		this->info->thread = (HANDLE)_beginthreadex(NULL, stack_size, defThreadFunction, (void*)this, CREATE_SUSPENDED, &this->info->t_id);
		if (!this->info->thread){
			this->sendEvent(Event(Multithreading::EV_ERROR, errno, "Thread created unsuccessfully"));
			this->setStatus(Multithreading::TS_ERROR);
		}
		else 
			this->setStatus(Multithreading::TS_CREATED);

		return 0;
	}	
	else 
		return 1;
};

int Multithreading::Thread::start(){
	switch (this->info->status){
	case Multithreading::TS_CREATED:
	{
		ResumeThread(this->info->thread);
		this->setStatus(TS_WORKING);
		return 0;

	}
		break;
	case Multithreading::TS_FINISHED:
	{
		CloseHandle(this->info->thread);
		setLockEvent(false);
		this->info->thread = (HANDLE)_beginthreadex(NULL, info->stack_size, defThreadFunction, (void*)this, 0, &this->info->t_id);
		if (!this->info->thread){
			this->sendEvent(*new Event(Multithreading::EV_ERROR, errno, "Thread created unsuccessfully"));
			this->setStatus(TS_ERROR);
		}
		return 0;
	}
		break;
	default:
		return this->info->status;
	}
};
int Multithreading::Thread::stop(size_t wait){
	if (this->info->status == Multithreading::TS_WORKING || this->info->status == Multithreading::TS_CREATED){
        sendEvent(Event(Multithreading::EV_SYSINFO, 1));
		setLockEvent(true);
//		if (this->info->status == Multithreading::TS_CREATED) ResumeThread(this->info->thread);

		return WaitForSingleObject(this->info->thread, wait);
	}
	else
		return this->info->status;
};
int Multithreading::Thread::pause(){
	if (this->info->status == Multithreading::TS_WORKING){
		SuspendThread(this->info->thread);
		this->setStatus(TS_PAUSED);
		return 0;
	}
	else return this->info->status;
};
int Multithreading::Thread::kill(){
    if(this->info->thread)
        TerminateThread(this->info->thread, Multithreading::TR_HALTED);
    this->setStatus(Multithreading::TS_CREATED);
    return 0;
};
int Multithreading::Thread::pauseUntil(DWORD miliseconds, const Multithreading::Thread *thread, Multithreading::ThreadStatus status){
	//1st option - only timeout
	//2nd option - timeout & status
	if (miliseconds){
		void* data = (void*) new char[sizeof(miliseconds)+2 * sizeof(thread)+sizeof(status)];
		int addr = (int)this;
		std::memcpy(data, &miliseconds, sizeof(miliseconds));
		std::memcpy((char*)data + sizeof(miliseconds), &thread, sizeof(thread));
		std::memcpy((char*)data + (sizeof(miliseconds)+sizeof(thread)), &addr, sizeof(thread));
		std::memcpy((char*)data + (sizeof(miliseconds)+2 * sizeof(thread)), &status, sizeof(status));
		HANDLE waiter = (HANDLE)_beginthreadex(NULL, 1024, Multithreading::defPauseUntilFunction, data, CREATE_SUSPENDED, NULL);
		if (!waiter)
			return 1;
		else
			ResumeThread(waiter);
		this->pause();

	}
	else if (thread &&status != TS_NONE){
		this->pause();
		Multithreading::ThreadStatus res = thread->getStatus();
		if (res != TS_NONE&&res != status)
			return 2;
	}
	return 0;
}

int Multithreading::Thread::join()const{
	//wait until thread closes
	DWORD res = WaitForSingleObject(this->info->thread, INFINITE);
	return (res == WAIT_OBJECT_0) ? 0 : 1;
}
int Multithreading::Thread::detach(){
	this->info->options |= Multithreading::ThreadOption::TO_DETACHED;
	this->info->thread = NULL;
	//remove thread handle in all references if it is not singleton instance
	return 0;
};


Multithreading::Event Multithreading::Thread::waitForEvent(DWORD milliseconds) const{
	DWORD res;
	const int epsilon = 100;
	Multithreading::ThreadStatus st;
	__time64_t begin = _time64(NULL), end; //this won't work in the year 2038 :D. No, seriously guys :)
	do{
		res = WaitForSingleObject(this->info->recent.getHandle(), milliseconds);
		end = _time64(NULL);
		if ((res == WAIT_ABANDONED || res == WAIT_OBJECT_0)){
			if (this->info->status == TS_NONE || ((st = this->getStatus()) != TS_NONE && st == this->info->status))
				return this->getEvent();
		}
		else if (res != WAIT_TIMEOUT)
			return Event(EV_NONE, 1);
	} while (abs(_difftime64(begin, end) * 1000 - milliseconds) < epsilon);

	return Event(EV_NONE, 2);
}
int Multithreading::Thread::waitForStatus(DWORD milliseconds, Multithreading::ThreadStatus status)const{
	DWORD res;
	Multithreading::ThreadStatus st;
	unsigned long long begin = GetTickCount(), end; //This is very dangerous solution for time count
	do{
		res = WaitForSingleObject(this->info->st_ev, milliseconds);
		st = status;
		ReleaseMutex(this->info->st_ev);
		end = GetTickCount();
		//check and fix time
		if (end < begin) {
			end = begin + end;
		}

		if ((res == WAIT_ABANDONED || res == WAIT_OBJECT_0)){
			if (st == TS_NONE || ((st = this->getStatus()) != TS_NONE && st & status))
				return 0;
		}
		else if (res != WAIT_TIMEOUT)
			return 1;
	} while ((end - begin)<milliseconds);

	return 2;
}

Multithreading::Thread& Multithreading::Thread::operator = (Multithreading::Thread &thread){
	if (this->getStatus() & (TS_CREATED | TS_WORKING | TS_PAUSED))
		if (this->stop(ThreadInfo::closeMaxDelay))
			this->kill();
	this->info = thread.info;
	return *this;
}
Multithreading::Thread& Multithreading::Thread::operator >> (const Multithreading::Thread &thread){
	size_t opts = this->getOptions(), opts_t = thread.getOptions();
	if (opts == -1 || opts == -1)
		throw EFAULT;
	else if ((opts | opts_t)&Multithreading::TO_DETACHED)
		throw EINVAL;
	else if (this == &thread)
		throw ECANCELED;
	//wait until finished
	this->waitForStatus(INFINITE, Multithreading::TS_FINISHED);
	return *this;
}
Multithreading::Thread& Multithreading::Thread::operator << (const Multithreading::Thread &thread) {
	size_t opts = this->getOptions(), opts_t = thread.getOptions();
	if (opts == -1 || opts == -1)
		throw EFAULT;
	else if ((opts | opts_t)&Multithreading::TO_DETACHED)
		throw EINVAL;
	else if (this == &thread)
		throw ECANCELED;
	thread.waitForStatus(INFINITE, Multithreading::TS_FINISHED);
	return *this;
}

/* DELETED: setOption() is redundant
size_t Multithreading::Thread::setOptions(size_t options){
DWORD r = WaitForSingleObject(this->res_mutex, this->readMaxDelay);
size_t old;
if (r == WAIT_ABANDONED || r == WAIT_OBJECT_0){
//if WAIT_ABANDONED, signal to thread management system
old = this->options;
this->options = options;
ReleaseMutex(this->res_mutex);
return old;
}
else return -1;
}
*/

size_t Multithreading::Thread::getOptions()const{
	DWORD r = WaitForSingleObject(this->info->res_mutex, ThreadInfo::readMaxDelay);
	if (r == WAIT_ABANDONED || r == WAIT_OBJECT_0){
		//if WAIT_ABANDONED, signal to thread management system
		auto res = this->info->options;
		ReleaseMutex(this->info->res_mutex);
		return res;
	}
	else return -1;
};
size_t Multithreading::Thread::getThreadId() const {
	DWORD r = WaitForSingleObject(this->info->res_mutex, ThreadInfo::readMaxDelay);
	if (r == WAIT_ABANDONED || r == WAIT_OBJECT_0){
		//if WAIT_ABANDONED, signal to thread management system
		auto res = this->info->t_id;
		ReleaseMutex(this->info->res_mutex);
		return res;

	}
	else return -1;
}
Multithreading::ThreadStatus Multithreading::Thread::getStatus()const{
	DWORD r = WaitForSingleObject(this->info->res_mutex, ThreadInfo::readMaxDelay);
	if (r == WAIT_ABANDONED || r == WAIT_OBJECT_0){
		//if WAIT_ABANDONED, signal to thread management system
		auto res = this->info->status;
		ReleaseMutex(this->info->res_mutex);
		return res;
	}
	else return Multithreading::TS_NONE;
};
int Multithreading::Thread::getLastError()const {
	int res = 0;
	DWORD r = WaitForSingleObject(this->info->res_mutex, ThreadInfo::readMaxDelay);
	if (r == WAIT_ABANDONED || r == WAIT_OBJECT_0){
		//if WAIT_ABANDONED, signal to thread management system
		auto res = this->info->th_errno;
		ReleaseMutex(this->info->res_mutex);
		return res;
	}
	return -1;
};

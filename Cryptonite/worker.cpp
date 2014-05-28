#include "worker.h"


Task::Task():
mode(true), eac(Task::DefEncryption), ht(Task::DefHashing), bcm(Task::DefBlockCipher)
{

}
Task::Task(const Task& t): 
path(t.path), out_path(t.out_path), mode(t.mode), key(t.key), salt(t.salt), eac(t.eac), ht(t.ht), bcm(t.bcm)
{
}
Task::Task(const std::string &path, const std::string &out_path, const bool mode, const std::string &key, const std::string &salt,
	Encrypter::EncryptionAlgorithm eac, Encrypter::HashingType ht, BlockCipher::Mode bcm):
	path(path), mode(mode), key(key), salt(salt), eac(eac), ht(ht), bcm(bcm), out_path(out_path)
{

}

void Worker::addLogEntry(const Multithreading::EventType type, const std::string &brief, const std::string &description){
	static std::map<Multithreading::EventType, std::string> map = {
		std::make_pair(Multithreading::EventType::EV_ERROR, "Error"),
		std::make_pair(Multithreading::EventType::EV_NONE, ""),
		std::make_pair(Multithreading::EventType::EV_SYSINFO, "System Info"),
		std::make_pair(Multithreading::EventType::EV_WARNING, "Warning"),
		std::make_pair(Multithreading::EventType::EV_TRACING, "Tracing")
	};
	std::stringstream ss;
	char str_time[256];
	struct tm *timeinfo;
	time_t now;
	time(&now);
	timeinfo = localtime(&now);
	WaitForSingleObject(this->mutex, INFINITE);
	strftime(str_time, 256, "%Y, %b %d - %H:%M:%S", timeinfo);
	ss << "[" << str_time << "] " << map.operator[](type) << ": " << brief << " - " << description;
	this->log.push_back(ss.str());
	ReleaseMutex(this->mutex);
}

Multithreading::ThreadResult __stdcall Worker::__workerThreadFunction(Multithreading::Thread *thread, void* worker_void){
	using namespace Multithreading;
	Worker *worker = reinterpret_cast<Worker*>(worker_void);
	Task* ts;
    size_t i, s, code;
    clock_t time, beg;
	Event thread_ev;
	Encrypter::EncryptedFile *ef = NULL;
	std::stringstream bufss;
	if (!worker){
		//void* argument, which is Worker, is not passed or not passed correctly - stopping work.
		thread->sendEvent(Event(EventType::EV_ERROR, 1, "Thread could not determine current worker - no work is done. Stopping..."));
		return ThreadResult::TR_ERROR;
	}
	s = worker->tasks.size();
	worker->addLogEntry(EventType::EV_TRACING, "Working Progress", "Initializing work loop...");
	for (i = 0; i < s; i++){
		try{
            beg = clock();
			//get info for the next task
			worker->addLogEntry(EventType::EV_TRACING, "Working Progress", "Loading task for new file...");
			ts = &worker->tasks[i];
			ef = new Encrypter::EncryptedFile(ts->path);
			//update task string by the event
			bufss << (ts->mode?"Encryption":"Decryption")<< " on file " << ts->path;
			worker->e.set(0xFF, EV_TRACING, 2, bufss.str());
			//clear streambuffer
			bufss.str(std::string());
			bufss.clear();
			worker->e.triggerEvent();
			

			worker->addLogEntry(EventType::EV_TRACING, "Working Progress", "File was parsed correctly. \r\n File path: " + ts->path);
			bufss << "Preparing for " << (ts->mode ? "encryption" : "decryption") << " process::" << std::endl
				<< "\tEncryption Algorithm: " << Encrypter::WrapEncryption::stringFromType(ts->eac) << std::endl
				<< "\tHashing Algorithm: " << Encrypter::WrapHashing::stringFromType(ts->ht) << std::endl
                << "\tBlockCipher Mode: " << Encrypter::WrapBlockCipher::stringFromType(ts->bcm);

 			worker->addLogEntry(EventType::EV_TRACING, "Working Progress", bufss.str());
			//clear streambuffer
			bufss.str(std::string());
			bufss.clear();
			//MAGIC! OLOLOLOLO!
			if(ts->mode)
                code = ef->encrypt(ts->key,IO::File(ts->out_path), Encrypter::WrapEncryption(ts->eac,true),
						Encrypter::WrapHashing(ts->ht),	Encrypter::WrapBlockCipher(ts->bcm));
			else
                code = ef->decrypt(ts->key, IO::File(ts->out_path));
            if(!code)
                worker->addLogEntry(EventType::EV_TRACING, "Working Progress", "Task was completed. \r\nOutput file path: " + ts->out_path);
            else {
                bufss<<"File was not processed. Error code : "<<code;
                worker->addLogEntry(EventType::EV_WARNING,"Working Progres", bufss.str());
                bufss.str(std::string());
                bufss.clear();
            }


            //update stats
            time = clock();
			WaitForSingleObject(worker->mutex, INFINITE);
			worker->progress = (i + 1) / s * 100;
            worker->speed = ef->getSize() / (size_t)(time - beg+18)*1000;
			ReleaseMutex(worker->mutex);
			worker->e.set(0xFF, EV_TRACING, 1);
			worker->e.triggerEvent();
			//check, if the stop signal was passed to this thread.
			thread_ev = thread->getEvent();
            if (thread_ev.getType() == EventType::EV_SYSINFO&&thread_ev.getCode() == 1){
				worker->addLogEntry(EventType::EV_WARNING, "Thread is stopping",
									"Caught StopWork Signal. All completed task are saved. Stopping...");
				break;
			}
			
		}
		catch (...){
			worker->addLogEntry(EventType::EV_ERROR, "Exception during work",
								"An exception was caught during the working process. Output file is probably corrupted.");
		};
		//safe delete Encrypted file
		try{
			delete ef;
		}
		catch (...){};
		
		
	}
    worker->e.set(0xFF, EV_TRACING, 3);
    worker->e.triggerEvent();
	worker->addLogEntry(EventType::EV_TRACING, "Working Progress", "All work is done. Finalising...");
	return ThreadResult::TR_SUCCESS;
}

Worker::Worker():
speed(0), progress(0), work_thread(Worker::__workerThreadFunction, this, false, 1024),
e(Multithreading::EV_NONE, 0), mutex((HANDLE)CreateMutex(NULL,FALSE,NULL))
{
	
}
Worker::Worker(const Task& task) :
speed(0), progress(0), work_thread(Worker::__workerThreadFunction, this, false, 1024),
e(Multithreading::EV_NONE, 0), mutex((HANDLE)CreateMutex(NULL, FALSE, NULL)), tasks(&task,&task+1)
{

}
Worker::Worker(const std::vector<Task> &tasks):
speed(0), progress(0), work_thread(Worker::__workerThreadFunction, this, false, 1024),
e(Multithreading::EV_NONE, 0), mutex((HANDLE)CreateMutex(NULL, FALSE, NULL)), tasks(tasks)
{

}


Worker::~Worker(){
	ReleaseMutex(this->mutex);
	CloseHandle(this->mutex);
}
int Worker::startWork(){
	this->work_thread.start();
	return 0;
}
int Worker::stopWork(const bool immediately){
	if (!immediately)
		return this->work_thread.stop();
	else
		this->work_thread.kill();
	return 0;
}

std::vector<std::string> Worker::getLog(){
	std::vector<std::string> res;
	WaitForSingleObject(this->mutex, INFINITE);
	res = this->log;
	ReleaseMutex(this->mutex);
	return res;
}
const Multithreading::Event& Worker::getEvent(){
	const Multithreading::Event* res;
	WaitForSingleObject(this->mutex, INFINITE);
	res = &this->e;
	ReleaseMutex(this->mutex);
	return *res;
}
unsigned char Worker::getProgress(){
	unsigned char res;
	WaitForSingleObject(this->mutex, INFINITE);
	res = this->progress;
	ReleaseMutex(this->mutex);
	return res;
}
size_t Worker::getSpeed(){
	size_t res;
	WaitForSingleObject(this->mutex, INFINITE);
	res = this->speed;
	ReleaseMutex(this->mutex);
	return res;
}
const Multithreading::Thread& Worker::getThread(){
	const Multithreading::Thread* res;
	WaitForSingleObject(this->mutex, INFINITE);
	res = &this->work_thread;
	ReleaseMutex(this->mutex);
	return *res;
};

#include "fileio.h"
#pragma warning(disable: 4996)


Multithreading::ThreadResult IO::File::fileThreadFunction(Multithreading::Thread* thr, void* fInstance){
	const int delay = 30;
	bool working = true;
	Multithreading::Event ev;
	IO::File *fileInstance = reinterpret_cast<IO::File*>(fInstance);
	while (working){
		ev = thr->waitForEvent(INFINITE);
		//parse event

	}
	return Multithreading::ThreadResult::TR_SUCCESS;
}

IO::File::File(const std::string &path, const size_t chunk_size, const bool async, const bool createIfNotExists) :
stream(path, std::ios::binary | std::ios::in | std::ios::out), asyncEvent(Multithreading::EventType::EV_SYSINFO, 0)
{
	this->chunk = chunk_size;
	this->fileHandle = NULL;
	this->path = path;
	this->c_pos = 0;
	this->worker = NULL;
	if (createIfNotExists && checkFile() & IO::IOF_NEXISTS)
		creat(this->path.c_str(), S_IWRITE | S_IREAD);
	this->stream = std::fstream(this->path, std::ios::binary | std::ios::in | std::ios::out);
	
	if (async){
		//create thread
		this->worker = new Multithreading::Thread(IO::File::fileThreadFunction, this, true, 2 * chunk_size);
		if (this->worker->getStatus() == Multithreading::ThreadStatus::TS_ERROR)
			throw EFAULT;
	}
};
IO::File::File(File &f){
	this->fileHandle = NULL;
	this->path = f.path;
	this->chunk = f.chunk;
	this->worker = NULL;
	this->c_pos = f.c_pos;
	this->stream = std::fstream(this->path.c_str(), std::ios::binary | std::ios::in | std::ios::out);
	if (f.isAsync()){
		//create thread
		this->worker = new Multithreading::Thread(IO::File::fileThreadFunction, this, true, 2 * this->chunk);
		if (this->worker->getStatus() == Multithreading::ThreadStatus::TS_ERROR)
			throw EFAULT;
	}
}

IO::File::~File(){
	/**
	* 1) If thread exists, send message to kill it
	* 2) Wait 500 ms for it
	* 3) Call kill() function
	* 4) Close stream
	* 5) Release all resources
	* 6) Go home.
	**/
	//1-3
	if (this->isAsync()){
		this->worker->stop();
		if (!this->worker->waitForStatus(500, Multithreading::ThreadStatus::TS_FINISHED))
			this->worker->kill();
	}
	//4
	this->stream.close();
	//5
	if (this->fileHandle)
		CloseHandle(this->fileHandle);
	//6
}

int IO::File::checkFile(){
	int state = IO::IOFileStatus::IOF_GOOD;
	if (this->worker&&!this->isCalledOnWorker()){
		//async
		state = -1;
	}
	else {
		//check if file exists
		//POSIX test (2nd best performance + crossplatform)
		struct _stat buffer;
		//check for errors (File not exists, cannot access, path is wrong)
		if (!_stat(this->path.c_str(), &buffer) && !open(this->path.c_str(), _O_RDONLY)){
			switch (errno){
			case EACCES:
				state = IO::IOFileStatus::IOF_NREAD;
				break;
			case ENOENT:
				state = IO::IOFileStatus::IOF_NEXISTS;
				break;
			case ENOTDIR:
				state = IO::IOFileStatus::IOF_WRONGPATH;
			}
		}
		else {
			//check if file is really opened
			
			//check if file is somewhat inaccessible for reading
			if (access(this->path.c_str(), 0x04))
				state |= IO::IOFileStatus::IOF_NREAD;
			//check if file is somewhat inaccessible for writing
			if (access(this->path.c_str(), 0x02))
				state |= IO::IOFileStatus::IOF_NWRITE;
			//check if file is bad
			if (this->stream.fail())
				state |= IO::IOFileStatus::IOF_BAD;
		}
	}
	return state;
}


bool IO::File::isCalledOnWorker(){
	if (this->worker&&this->worker->getThreadId() == GetCurrentThreadId())
		return true;
	else 
		return false;
}

//FILE IO

int IO::File::loadFile(std::string &buf, size_t offset, size_t length, bool movePosition){
	int state = 0;
	size_t num;
	size_t len = length == -1 ? this->getSize() - offset: length;
	if (this->worker&&!this->isCalledOnWorker()){
		//async
		state = -1;
	}
	else if(this->stream.good()){
		
		char *t_buf = new char[this->chunk];
		this->stream.seekp(offset,std::ios::beg);
		while (!this->stream.eof() && buf.length() < len){
			num = (len - buf.length() < this->chunk) ? len - buf.length() : this->chunk;
			this->stream.read(t_buf,num);
			if (!this->stream.good()){
				state = 1;
				break;
			}
			buf.append(t_buf,num);
		}
		if(!movePosition)
			this->stream.seekp(this->c_pos,std::ios::beg);
		delete[] t_buf;
	}
	else state = 1;
	return state;
}
int IO::File::loadChunk(std::string &buf,size_t offset, bool movePosition){
	int state = 0;
	if (this->worker&&!this->isCalledOnWorker()){
		//async
		state = -1;
	}
	else {
		char *t_buf = new char[this->chunk];
		this->stream.seekp(offset, std::ios::beg);
		this->stream.read(t_buf, this->chunk);
		buf.append(t_buf, this->chunk);
		if (!movePosition)
			this->stream.seekp(this->c_pos, std::ios::beg);

		delete[] t_buf;
	}
	return state;
}

int IO::File::overwrite(const std::string &str){
	int state = 0;
	if (this->worker&&!this->isCalledOnWorker()){
		//async
		state = -1;
	}
	else {
		this->stream.close();
		//this->clear();

		this->stream.open(this->path.c_str(),std::ios::binary|std::ios::out);
		if (!this->stream.good())
			state = 1;
		this->stream << str;
		this->stream.flush();
		if (!this->stream.good())
			state = 1;
		else
			this->c_pos = this->stream.seekp(std::streamoff(0), std::ios::end).tellp();
	}
	return state;
}
int IO::File::clear(){
	int state = 0;
	if (this->worker&&!this->isCalledOnWorker()){
		//async
		state = -1;
	}
	else {
		this->stream.close();
		this->stream.open(this->path, std::ios::binary|std::ios::out);
		if (!this->stream.good())
			state = 1;
		else{
			this->c_pos = 0;
			this->stream.close();
			this->stream.open(this->path, std::ios::binary | std::ios::out | std::ios::in);
		}
			
	}
	return state;
}

int IO::File::append(const std::string &str){
	int state = 0;
	if (this->worker&&!this->isCalledOnWorker()){
		//async
		state = -1;
	}
	else {
		this->stream.seekg(0,std::ofstream::end);
		this->stream.write(str.c_str(), str.length());

		if (!this->stream.good())
			state = 1;
		else{
			this->stream.seekg(0,std::ofstream::end);
			this->c_pos = this->stream.tellg();
		}
			
	}
	return state;
};
int IO::File::insert(const std::string &str){
	int state = 0;
	return state;
}
/* DELETED - unnecessary
int IO::File::createFile(const bool overwrite){
	int state = 0;
	if (this->worker&&!this->isCalledOnWorker()){
		//async
		state = -1;
	}
	else {
		//check if file exists
		state = this->checkFile();
		if (state & IO::IOFileStatus::IOF_NEXISTS || !state && overwrite)
		{
			this->stream.close();
			this->stream.open);
		}
		else if (!state) {

		}
		else return 1;

		state = this->stream.is_open()|this->stream.good();
	}
	return state;
};*/
int IO::File::deleteFile(){
	int state = 0;
	if (this->worker&&!this->isCalledOnWorker()){
		//async
		state = -1;
	}
	else {
		state = remove(this->path.c_str());
		if (!state)
			this->c_pos = 0;
	}
	return state;
}
int IO::File::close(){
	this->stream.close();
	return this->checkFile();
}

//GETTERS

std::string IO::File::getName(){
	return std::string(
		std::find_if(this->path.rbegin(), this->path.rend(),
		IO::PathSeparator()).base(),
		this->path.end() );
}
std::string IO::File::getPath(){
	return this->path;
}
size_t IO::File::getSize(){
	this->stream.seekg(0, std::ios::end);
	size_t result = this->stream.tellg();
	this->stream.seekg(0,this->c_pos);
	return result;
}
bool IO::File::isAsync(){
	if (this->worker != NULL) return true;
	else return false;
}
const Multithreading::Thread& IO::File::getThread(){
	return *this->worker;
}
const Multithreading::Event& IO::File::getEvent(){
	return this->asyncEvent;
}

//SETTERS

int IO::File::setPos(const size_t pos){
	int state = 0;
	if (this->worker&&!this->isCalledOnWorker()){
		//async
		state = -1;
	}
	else if (pos<this->getSize()){
		this->stream.seekp(pos, std::ios::beg);
		if (!this->stream.good())
			state = 1;
		else
			this->c_pos = pos;
	}
	else
		state = 1;
	return state;
};

//STATIC

//helper function
int fillEntry(const std::string &dir_path, DIR* dir, IO::DiskEntryProxy *dep){
	struct dirent *dent;
	int handle;
	dent = readdir(dir);
	if (!dent)
		return errno;
	//skip . and .. folders
	if (!strcmp(dent->d_name, "..") || !strcmp(dent->d_name, "."))
		return 0;
	dep->path = dir_path + std::string(&IO::PathSeparator::path_sp,0,1) + dent->d_name;
	dep->descriptor = 0;
	if (dent->d_type&DT_DIR)
		dep->descriptor |= IO::IOEntryFlags::IOE_DIR;
	//check file - get attributes 
	else {
		if ((handle = open(dep->path.c_str(), _O_RDONLY)) != -1){
			dep->descriptor |= IO::IOEntryFlags::IOE_READABLE;
			close(handle);
		}
		if ((handle = open(dep->path.c_str(), _O_WRONLY)) != -1){
			dep->descriptor |= IO::IOEntryFlags::IOE_WRITABLE;
			close(handle);
		}
	}
	
	return 0;
};
//main function
/*
*	@brief Lists content in the folder.
*	@returns 0 - if function ended successfully, errno code - if otherwise

*/
int IO::File::list(const std::string &folder_path, std::vector<struct IO::DiskEntryProxy> &vec){
	std::vector<struct DiskEntryProxy> res;
	int code = 0;
	std::string dp;
	const size_t pathL = 65536;
	static char path[pathL], fArgument[pathL];
	//open directory
	struct DIR *dir = opendir(folder_path.c_str());
	//if could not open
	if (!dir)
		return errno;
	//expand vector to fit minimum size of 1
	vec.resize(vec.size() + 1);
	while (!(code = fillEntry(folder_path,dir, &vec[vec.size()-1])) )
		vec.resize(vec.size() + 1);

	vec.resize(vec.size() - 1); //remove last null element
	closedir(dir);
	return code^EACCES;
};
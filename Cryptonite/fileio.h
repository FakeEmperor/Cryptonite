#pragma once
#include "utils.h"
#include "multithreading.h"
#ifdef __WINDOWS_FOREVAH
//external library, thanks to some site
#include "dirent.h"
#elif defined(__WINDOWS_MUSTDIE)
#include <dirent.h>
#endif
#include <iostream>
#include <fstream>
#include <io.h>

namespace IO{
	enum IOEntryFlags {
		IOE_DIR = 0x01, IOE_OPENABLE = 0x02, IOE_READABLE = 0x04,
		IOE_WRITABLE = 0x08, IOE_EXECUTABLE = 0x10
	};
	struct DiskEntryProxy {
		int descriptor;
		std::string path;
	};
	struct PathSeparator {
		static const char path_sp = '\\';
		static const char linux_path_sp = '/';

		bool operator()(char ch) const		{
#if defined(_WIN32) || defined(WIN32) || defined(WINDOWS)
			return ch == path_sp || ch == linux_path_sp;
#elif defined(_LINUX) || defined(LINUX)
			return ch == linux_path_sp;
#endif
		};
	};

	enum IOOperation { IOO_LOAD, IOO_APPEND, IOO_INSERT, IOO_GOTO, IOO_CREATE, IOO_OVERWRITE, IOO_CHECK };
	enum IOFileStatus { IOF_GOOD = 0x00, IOF_NEXISTS = 0x01, IOF_NWRITE = 0x02, IOF_NREAD = 0x04, IOF_BAD = 0x08, IOF_WRONGPATH = 0x10 };
	class File {
		//FILE descriptor; //used to describe file surroundings
		HANDLE fileHandle;//used for lock/unlock
		size_t c_pos; //holds the current position of 
		size_t chunk; //used for chunk operations
		Multithreading::Thread *worker; //used for asynchronious operations
		std::string path;
		Multithreading::Event asyncEvent;
	protected:
		std::fstream stream;//used for control operations


		bool isCalledOnWorker(); //checks whether function is called on a worker thread
	public:
		const size_t defChunkSize = 20;
		static Multithreading::ThreadResult _stdcall fileThreadFunction(Multithreading::Thread *thread, void* fileInstance);

		File(const std::string &path,const size_t chunk_size = 20,const bool asynced = false, const bool createIfNotExists = true);
		File(File &f); 

		~File();
		/********** CONTROLS ***********/
		/**
		* Lock file (make it unusable for the other programs until unlock() is called)
		* @note: There are many limitations to that function and several hacks/tricks to avoid that lock,
		*		  so it is not very useful technique to protect the file
		* @example: If this code running on a user-priveleged level,
		*			and other program which is running on an administrative level is to access locked file,
		*			the access will be granted.
		**/
		int lock();
		/**
		*	Unlock file (making it usable again in all other programs)
		**/
		int unlock();
		

		/*********** SYNCHRONISATION ************/
		/**
		*	Creates a thread, so any operations are being done through the eventing system 
		**/
		int asynchronise();
		/**
		*	Creates a thread, so any operations are being done through the eventing system
		**/
		//int beginOperation(IOOperation opcode, void* data);
		/**
		*	Waits until working operation is done, then safely removes a thread.
		*	Note: No other events will be passed/sent to/from that thread.
		**/
		int synchronise();

		/******** FILE IO *******/
		int loadFile(std::string &buf, size_t offset = 0, size_t length = -1, bool movePosition = false);
		int loadChunk(std::string &buf, size_t offset = 0, bool movePosition = false);
		
		int overwrite(const std::string &str);
		int clear();
		int close();

		int append(const std::string &str);
		int insert(const std::string &str);
		//DELETED - overkill and unnecessary
		//int createFile(const bool overwrite = false);
		int deleteFile();

		int checkFile(); //checks file by its path, 0 - good, >0 bad and error code, corresponding to certain error
		/******* GETTERS *******/
		std::string getName();
		std::string getPath();
		size_t getSize();
		bool isAsync();
		/**
		* Why const? To disable control features, like pause(), stop() and other nasty things which will probably mess up the file.
		**/
		const Multithreading::Thread& getThread();
		/**
		* Why const? To disable control features. You only need that to wait for response
		**/
		const Multithreading::Event& getEvent();

		/******* SETTERS *******/
		/**
		*	Set reading position to that position
		**/
		int setPos(const size_t pos);

		/****** STATIC CONTROL FUNCTIONS ********/
		static int list(const std::string &folder_path, std::vector<struct IO::DiskEntryProxy> &vec);

	};
};
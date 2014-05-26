#pragma once
#include "encrypter.h"
#include "utils.h"
#include <map>
#include "multithreading.h"



//this class contains all the required info about the file to encrypt/decrypt
struct Task{
	std::string path,out_path; //path to a file
	bool mode; // mode == true - encrypt, mode == false - decrypt
	std::string key, salt;

	Encrypter::EncryptionAlgorithm eac;
	Encrypter::HashingType ht;
	BlockCipher::Mode bcm;

	Task();
	Task(const Task& t);
	Task(const std::string &path, const std::string &out_path, const bool mode, const std::string &key, const std::string &salt,
		Encrypter::EncryptionAlgorithm eac, Encrypter::HashingType ht, BlockCipher::Mode bcm);

	static const Encrypter::EncryptionAlgorithm DefEncryption = Encrypter::EncryptionAlgorithm::EA_AES;
	static const Encrypter::HashingType DefHashing = Encrypter::HashingType::HT_SHA256;
	static const BlockCipher::Mode DefBlockCipher = BlockCipher::Mode::BC_CBC;
};


class Worker{
	std::vector<Task> tasks;
	Multithreading::Thread work_thread;
	Multithreading::Event e;
	HANDLE mutex;
	std::vector<std::string> log;
	unsigned char progress; // [0 - 100] in percents
	size_t speed; //in bytes
protected:
	void addLogEntry(const Multithreading::EventType, const std::string &brief, const std::string &description);
	
	static Multithreading::ThreadResult __stdcall __workerThreadFunction(Multithreading::Thread *thread, void* worker);
	
public:
	Worker();
	Worker(const Task& task);
	Worker(const std::vector<Task> &tasks);
	
	int startWork();
	int stopWork(const bool immediately = false);
	
	std::vector<std::string> getLog();
	const Multithreading::Event& getEvent();
	unsigned char getProgress();
	size_t getSpeed();
	const Multithreading::Thread& getThread();

	~Worker();
};
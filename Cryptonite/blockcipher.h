#pragma once
#include "Utils.h"
#include "multithreading.h"


namespace BlockCipher{
    enum Mode { BC_UNKNOWN = 0x00, BC_ECB = 0x01, BC_CBC = 0x02, BC_OFB = 0x03, BC_CFB = 0x04, BC_MAX = 0x04 };

	typedef std::string(*CryptoFunction)(const std::string&,const std::string&);
	class Cipher{
	protected:
		size_t block;
		Multithreading::Thread* thread;
		Multithreading::Event ev;
		std::string iv;
		std::string to_xor;

		virtual int EncryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) = 0;
		virtual int DecryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) = 0;

	public:		
		//virtual const Utils::Thread& EncryptAsync();
		virtual int Encrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) = 0;
		
		//virtual const Utils::Thread& DecryptAsync();
		virtual int Decrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) = 0;
	};

	class ECBEncrypter :public Cipher{
	protected:
		int EncryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) override final;
		int DecryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) override final;
	public:
		static const Mode Mode = BC_ECB;
		ECBEncrypter(const size_t &block, const std::string &initializationVector = "");
		
		int Encrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f)override final;
		int Decrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f)override final;
	};

	class CBCEncrypter :public Cipher{
	protected:
		int EncryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) override final;
		int DecryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) override final;
	public:
		static const Mode Mode = BC_CBC;
		CBCEncrypter(const size_t &block,const std::string &initializationVector = "");

		int Encrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f)override final;
		int Decrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f)override final;
	};

	class OFBEncrypter :public Cipher{
	protected:
		int EncryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) override final;
		int DecryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) override final;
	public:
		static const Mode Mode = BC_OFB;
		OFBEncrypter(const size_t &block, const std::string &initializationVector = "");

		int Encrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f)override final;
		int Decrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f)override final;
	};

	class CFBEncrypter :public Cipher{
	protected:
		int EncryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) override final;
		int DecryptBlock(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f) override final;
	public:
		static const Mode Mode = BC_CFB;
		CFBEncrypter(const size_t &block, const std::string &initializationVector = "");

		int Encrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f)override final;
		int Decrypt(std::string &buf, const std::string &data, const std::string &key, CryptoFunction f)override final;
	};
};

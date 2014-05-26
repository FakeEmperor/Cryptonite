#pragma once
//Block-cipher functions
#include "blockcipher.h"
#include "utils.h"
//Symmetric cipherings
#include "cryptopp\aes.h"
#include "cryptopp\blowfish.h"
#include "cryptopp\gost.h"
//Assymmetric ciphering
#include "cryptopp\rsa.h"
//Hashing functions
#include "cryptopp\md5.h"
#include "cryptopp\sha.h"
#include "cryptopp\crc.h"
//BASE encoding
#include "cryptopp\base64.h"
#include "cryptopp\pwdbased.h"
//ECB
#include "cryptopp\modes.h"
//FILE IO
#include "fileio.h"
#ifdef __HAS_C11
#include <type_traits>
#else 
#include <tr1/type_traits>
#endif;



namespace Encrypter{
	typedef std::string(*CryptoFunctionPointer)(const std::string &key, const std::string &data);
	typedef std::string(*BlockCipherFunctionPointer)(const std::string &key, const std::string &data, BlockCipher::CryptoFunction f);

	std::string genStr(const size_t &size);
	
	/******  TEMPLATE FUNCTIONS *******/
	template<typename hashingClass>
	size_t testPBKDF(const std::string &key, const std::string &salt, const size_t keyLength, const size_t testNumIterations = 200,const size_t seconds = 1){
		static int const cycles = 10;
		static int const minIterations = 100;

		size_t i = 0,res, tni = (testNumIterations < minIterations) ? minIterations : testNumIterations;
		u_llong beg,time;
		long double average = 0, k;
		if (keyLength < key.length() || !key.length())
			return -1;

		byte *buffer = new byte[keyLength + 1];
		CryptoPP::PKCS5_PBKDF2_HMAC<hashingClass> pbkdf;
		/**
			First you need to know is that this function pbkdf is O(x), so
			time = k*numberOfIterations;
			k - some coefficient (depends on your machine, keylength and saltlength;
		**/
		//first, count execution time
		beg = clock();
		for (i = 0; i < cycles; i++)	
			pbkdf.DeriveKey(buffer, keyLength, 0, (byte*)key.c_str(), key.length(), (byte*)salt.c_str(), salt.length(), tni, 0);				
		time = clock() - beg; //DANGEROUS (if clock resets to zero)
		//calculate average time
		average = time / (long double)CLOCKS_PER_SEC * 1000 / cycles;
		//calculate k
		k =  average / (long double) tni;
		//std::cout << "One call with the iterations=" << tni << " costs me averagely: " << average << "ms (coef = "<<k<<" )"<<std::endl;
		
		
		if (average < 0.05) average = 0.2;//if average time is too small
		res = (size_t)fabsl((long double)seconds * 1000 / k)+1;
		delete[] buffer;
		return res;
	};
	template<typename hashingClass>
	std::string genPBKDFString(const std::string &key, const std::string &salt, const size_t length, const size_t numIterations){
		std::string res;
		static CryptoPP::PKCS5_PBKDF2_HMAC<hashingClass> pbkdf;
		byte *buffer = new byte[length];
		pbkdf.DeriveKey(buffer, length, 0, (byte*)key.c_str(), key.length(), (byte*)salt.c_str(), salt.length(), numIterations, 1);
		res.append((char*)buffer, length);
		return res;
	};
	
	template<typename CryptoAlgorithm>
	std::string cryptoFunction(const std::string &data, const std::string &key){
		std::string crypt;
		CryptoAlgorithm e;
		byte buf[CryptoAlgorithm::BLOCKSIZE + 1];
		//Fill in buffer
		memcpy(buf, data.c_str(), CryptoAlgorithm::BLOCKSIZE);
		buf[CryptoAlgorithm::BLOCKSIZE] = '\0';
		//Initialize key
		e.SetKey((byte*)key.c_str(), key.length());
		//Process block
		e.ProcessBlock(buf);
		//Assign to the string
		crypt.assign((char*)buf, CryptoAlgorithm::BLOCKSIZE);

		return crypt;
	}

	template<typename encryption>
	std::string eBlockFunction(const std::string &data, const std::string &key){
		return cryptoFunction<encryption::Encryption>(key, data);
	};
	template<typename encryption>
	std::string dBlockFunction(const std::string &data, const std::string &key){
		return cryptoFunction<encryption::Decryption>(key, data);
	}

	
	
	static const char UUID[173] = "HpV0xTFACuQnqqys9BZpmFCzSHg6qLCaDKBt6Fx/hFQYeWyoBPZQJDNVx7CoN0gkuLCY4KjswiIooHha3HKnlIJaHiCv0KAeIm55qFTonJB8QDiinq5Avv4O9KueoHWw113UDUh2cq0Q5vqwjJvZ4Az2wTza+JVOKjwE0BCa6HA=";

    enum EncryptionAlgorithm { EA_UNKNOWN = 0x00, /*EA_RSA = 0x01,*/ EA_AES = 0x01, EA_BLOWFISH = 0x02, EA_GOST = 0x03, EA_MAX = 0x03 };
	enum HashingType { HT_UNKNOWN = 0x00, HT_MD5 = 0x01, HT_SHA = 0x02, HT_SHA256 = 0x03, HT_MAX = 0x03 };
	//ABSTRACT TEMPLATE CLASS FOR WRAPPERS

	//NEED TO RECODE THAT SHIT INTO CURIOUSLY RECURRING TEMPLATE (FFFUUUUUUUUUUUUUUUUUUU)

	class aWrapF{
	protected:
		int type;
		

		inline int getType();
	protected:
		static std::string stringFromType(const int c, std::map<int, const std::string> &string_rep);
		static int typeFromString(const std::string &str, std::map<int, const std::string> &string_rep);
	public:
		static std::string stringFromType(const int c);
		static int typeFromString(const std::string &str);
	};
	//WRAPPER CLASSES, WEEEEE!!
	class WrapEncryption:public aWrapF{
		bool encrypt;
		static int max, unknown;
	public:
		static std::map<int, const std::string> string_rep;
		const size_t DEFAULT_KEYLENGTH, BLOCKSIZE;
		WrapEncryption(const EncryptionAlgorithm type, const bool encrypt = true);

		static size_t getDefKeyLength(const EncryptionAlgorithm type);
		static size_t getBlockSize(const EncryptionAlgorithm type, const bool encrypt);
		
		CryptoFunctionPointer getCryptoFunction();
		inline EncryptionAlgorithm  getType();
		void setMethod(const bool encrypt = true);

		static std::string stringFromType(const int c);
		static int typeFromString(const std::string &str);
	};
	class WrapHashing:public aWrapF{
		static int max, unknown;
	public:
		static std::map<int, const std::string> string_rep;
		WrapHashing(const HashingType type);
		inline HashingType getType();
		size_t test(const std::string &key, const std::string &salt, const size_t keyLength, const size_t testNumIterations = 200, const size_t seconds = 1);
		std::string gen(const std::string &key, const std::string &salt, const size_t length, const size_t numIterations);

		static std::string stringFromType(const int c);
		static int typeFromString(const std::string &str);
	};
	class WrapBlockCipher:public aWrapF{
		static int max, unknown;
		BlockCipher::Cipher *cp;
	public:
		static std::map<int, const std::string> string_rep;
		WrapBlockCipher(const BlockCipher::Mode type);
		~WrapBlockCipher();
		inline BlockCipher::Mode getType();
		
		BlockCipher::Cipher* getBlockCipherPointer(const size_t block = 16, const std::string &iv = "");


		static std::string stringFromType(const int c);
		static int typeFromString(const std::string &str);
	};


	/** ENCRYPTED FILE STRUCTURE
	* [OPEN_PART (192 bytes)][ENCRPTD (9 bytes)]UUID(172 bytes)|[DESC](6 bytes)<ENCRYPTION_TYPE(1b)|BLOCKCIPHER_MODE(1b)|HASHING_METHOD(1b)|PBKDFROUNDS(2b)>[/OPEN_PART]
	* [CLOSED_PART >14 bytes][CLOSED (8 bytes)]SIZE (4 bytes)|FILENAME_LENGTH(2 byte)FILENAME[/CLOSED_PART]
	* [DATA]
	* [PADDING]
	**/
	class EncryptedFile:public IO::File{
		HashingType ht;
		BlockCipher::Mode ecm;
		EncryptionAlgorithm eac;
		unsigned short num_pbkdf;

		bool enc_status; // 0 - N/E, 1 - ENC

		std::string generateIV(size_t length);
		std::string generateKey(size_t length, std::string &key);
		bool checkHeader();
	protected:
		int writeOpenPart( File &to );
		std::string makeClosedPartHeader();
		void mainInit();

		int parseClosedPart(const std::string &str, std::string &data, std::string *filename = nullptr);
	public:
		static const std::string h1, h2, eh1;

		
		//INITIALIZER
		EncryptedFile(File& outFile);
		EncryptedFile(const std::string &path, const size_t chunk_size = 20, const bool asynced = false);
		//DECRYPT

		/** DECRYPTION DATA PROCESS
		*	1) Read file for tags.
		*	2) Create PBKDF key with the defined number of rounds.
		*	4) Read and decrypt data.
		*	5) Save decrypted data to the newly created file.
		**/
		//TEMPLATE FUNCTION
		/* [DELETED]: Bad style

		template<typename hashing, typename encryption, typename blockmode>
		int decrypt(const std::string &key, File &out, std::string *outString = nullptr);

		[ALTERNATIVE]: Non-template function

		*/
		int decrypt(const std::string &key, File &out, std::string *outString = nullptr);
		/** ENCRYPTION DATA PROCESS
		*	1) Create new empty file with prefered name.
		*	2) Make a structure, descripting encryption type, number of PBKDF rounds, prefered hashing method. And also a UUID of this program (it's a mark, that this file is probably encrypted).
		*	3) Append that ^ structure to the file.
		*	4) Make a structure, descripting filename, filesize.
		*	5) Make IV(initialization vector).
		*	6) Make string, containing filsesize,filename length, filename, padding so the string is 64 bytes.
		*	7) Load data, encrypt it with the defined algorithm.
		*	8) Save encrypted data to the newly created file.
		**/

		/* [DELETED]: Bad style

		template<typename hashing, typename encryption, typename blockmode>
		int encrypt(const std::string &key, File &out, std::string *outString = nullptr);
		
		   [ALTERNATIVE]: Non-template function
		
		*/
		int encrypt(const std::string &key, File &out, WrapEncryption &enc, WrapHashing &hash, WrapBlockCipher &wb, std::string *outString = nullptr);

		~EncryptedFile();
		//GETTERS
		bool isValid();
		bool isEncrypted();
		size_t getPNum();	
		HashingType getHashType();
		BlockCipher::Mode getBlockType();
		EncryptionAlgorithm getEncType();

		static int scan(const std::string &path, std::vector<EncryptedFile> &vec, const bool scan_subfolders = false);

	};
	
};


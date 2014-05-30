#include "encrypter.h"

int Encrypter::aWrapF::getType(){
	return this->type;
};
std::string Encrypter::aWrapF::stringFromType(const int c, std::map<int, const std::string> &string_rep) {
	auto it = string_rep.find(c);
	if (it == string_rep.end())
		return "";
	else
		return it->second;
}
int Encrypter::aWrapF::typeFromString(const std::string &str, std::map<int, const std::string> &string_rep){
	std::map<int, const std::string>::iterator it;
	for (it = string_rep.begin(); it != string_rep.end(); ++it){
		if (str == it->second)
			return it->first;
	}

	return -1;
}

//WRAP ENCRYPTION
#ifdef __HAS_C11
std::map<int, const std::string> Encrypter::WrapEncryption::string_rep = {
    { Encrypter::EA_GOST, "gost" }, { Encrypter::EA_BLOWFISH, "blowfish" }, { Encrypter::EA_AES, "aes" }
};
#else 
Encrypter::WrapEncryption::string_rep.insert(std::make_pair(Encrypter::EA_AES, std::string("aes")));
//Encrypter::WrapEncryption::string_rep.insert(std::make_pair(Encrypter::EA_RSA, std::string("rsa")));
Encrypter::WrapEncryption::string_rep.insert(std::make_pair(Encrypter::EA_BLOWFISH, std::string("blowfish")));
Encrypter::WrapEncryption::string_rep.insert(std::make_pair(Encrypter::EA_GOST, std::string("gost")));
#endif
int Encrypter::WrapEncryption::max = Encrypter::EncryptionAlgorithm::EA_MAX;
int Encrypter::WrapEncryption::unknown = Encrypter::EncryptionAlgorithm::EA_UNKNOWN;

size_t Encrypter::WrapEncryption::getDefKeyLength(const EncryptionAlgorithm type){
	using namespace CryptoPP;
	switch (type){
	case EA_AES:
		return AES::DEFAULT_KEYLENGTH;
		break;
	case EA_BLOWFISH:
		return Blowfish::DEFAULT_KEYLENGTH;
		break;
	case EA_GOST:
		return GOST::DEFAULT_KEYLENGTH;
		break;
	}
	return 0;
};

size_t Encrypter::WrapEncryption::getBlockSize(const EncryptionAlgorithm type,const bool encrypt){
	using namespace CryptoPP;
	switch (type){
	case EA_AES:
		return encrypt ? AES::Encryption::BLOCKSIZE : AES::Decryption::BLOCKSIZE;
		break;
	case EA_BLOWFISH:
		return encrypt ? Blowfish::Encryption::BLOCKSIZE : Blowfish::Decryption::BLOCKSIZE;
		break;
	case EA_GOST:
		return encrypt ? GOST::Encryption::BLOCKSIZE : GOST::Decryption::BLOCKSIZE;
		break;
	}
	return 0;
};

Encrypter::WrapEncryption::WrapEncryption(const EncryptionAlgorithm type,const bool encrypt)
:DEFAULT_KEYLENGTH(Encrypter::WrapEncryption::getDefKeyLength(type)), BLOCKSIZE(Encrypter::WrapEncryption::getBlockSize(type,encrypt))
{
	if (type<0 || type>EA_MAX)
		throw EINVAL;
	this->type = type;
	this->encrypt = encrypt;
}

Encrypter::CryptoFunctionPointer Encrypter::WrapEncryption::getCryptoFunction(){
	//Fill in buffer
	using namespace CryptoPP;
	switch (type){
	case EA_AES:
		return this->encrypt ? Encrypter::cryptoFunction<AES::Encryption> : Encrypter::cryptoFunction<AES::Decryption>;
		break;
	case EA_BLOWFISH:
		return this->encrypt ? Encrypter::cryptoFunction<Blowfish::Encryption> : Encrypter::cryptoFunction<Blowfish::Decryption>;
		break;
	case EA_GOST:
		return this->encrypt ? Encrypter::cryptoFunction<GOST::Encryption> : Encrypter::cryptoFunction<GOST::Decryption>;
		break;
	}
	return nullptr;
};

Encrypter::EncryptionAlgorithm Encrypter::WrapEncryption::getType(){
	return (Encrypter::EncryptionAlgorithm)this->type;
}

void Encrypter::WrapEncryption::setMethod(const bool encrypt){
	this->encrypt = encrypt;
}

std::string Encrypter::WrapEncryption::stringFromType(const int c) {
	if (c <= unknown || c > max)
		return "";
	return Encrypter::aWrapF::stringFromType(c, Encrypter::WrapEncryption::string_rep);
}
int Encrypter::WrapEncryption::typeFromString(const std::string &str){
	int res = Encrypter::aWrapF::typeFromString(str, Encrypter::WrapEncryption::string_rep);
	return res == -1 ? Encrypter::WrapEncryption::unknown : res;
}

//WRAP HASHING
#ifdef __HAS_C11
std::map<int, const std::string> Encrypter::WrapHashing::string_rep = { 
	{ Encrypter::HT_SHA, "sha1" }, { Encrypter::HT_SHA256, "sha256" }, { Encrypter::HT_MD5, "md5" }
};
#else 
Encrypter::WrapHashing::string_rep.insert(std::make_pair(Encrypter::HT_SHA, std::string("sha")));
Encrypter::WrapHashing::string_rep.insert(std::make_pair(Encrypter::EA_SHA256, std::string("sha256")));
Encrypter::WrapHashing::string_rep.insert(std::make_pair(Encrypter::HT_MD5, std::string("md5")));
#endif
int Encrypter::WrapHashing::max = Encrypter::HashingType::HT_MAX;
int Encrypter::WrapHashing::unknown = Encrypter::HashingType::HT_UNKNOWN;

Encrypter::WrapHashing::WrapHashing(const HashingType type){
	if (type<0 || type>HT_MAX)
		throw EINVAL;
	this->type = type;
}

size_t Encrypter::WrapHashing::test(const std::string &key, const std::string &salt, const size_t keyLength, const size_t testNumIterations, const size_t seconds){
	using namespace CryptoPP;
	switch (type){
	case HT_MD5:
		return  Encrypter::testPBKDF<MD5>(key, salt, keyLength, testNumIterations, seconds);
		break;
	case HT_SHA:
		return  Encrypter::testPBKDF<SHA>(key, salt, keyLength, testNumIterations,seconds);
		break;
	case HT_SHA256:
		return  Encrypter::testPBKDF<SHA256>(key, salt, keyLength, testNumIterations, seconds);
		break;
	}
	return HT_UNKNOWN;
}
std::string Encrypter::WrapHashing::gen(const std::string &key, const std::string &salt, const size_t length, size_t &numIterations){
	//Fill in buffer
	using namespace CryptoPP;
    std::string res;
	switch (type){
	case HT_MD5:
        numIterations = Encrypter::genPBKDFString<MD5>(res,key, salt,length,0);
		break;
	case HT_SHA:
        numIterations = Encrypter::genPBKDFString<SHA>(res, key, salt, length, 0);
		break;
	case HT_SHA256:
        numIterations = Encrypter::genPBKDFString<SHA256>(res, key, salt, length, 0);
		break;
    default:
        res = "";
        break;
	}
    return res;
}
std::string Encrypter::WrapHashing::genForce(const std::string &key, const std::string &salt, const size_t length,const size_t numIterations){
    //Fill in buffer
    using namespace CryptoPP;
    std::string res;
    switch (type){
    case HT_MD5:
        Encrypter::genPBKDFString<MD5>(res,key, salt,length,numIterations);
        break;
    case HT_SHA:
        Encrypter::genPBKDFString<SHA>(res, key, salt, length, numIterations);
        break;
    case HT_SHA256:
        Encrypter::genPBKDFString<SHA256>(res, key, salt, length, numIterations);
        break;
    default:
        res = "";
        break;
    }
    return res;
}

Encrypter::HashingType Encrypter::WrapHashing::getType(){
	return (Encrypter::HashingType)this->type;
};

std::string Encrypter::WrapHashing::stringFromType(const int c) {
	if (c <= unknown || c > max)
		return "";
	return Encrypter::aWrapF::stringFromType(c, Encrypter::WrapHashing::string_rep);
}
int Encrypter::WrapHashing::typeFromString(const std::string &str){
	int res = Encrypter::aWrapF::typeFromString(str, Encrypter::WrapHashing::string_rep);
	return res == -1 ? Encrypter::WrapHashing::unknown : res;
}

//WRAP BLOCKCIPHER
#ifdef __HAS_C11
std::map<int, const std::string> Encrypter::WrapBlockCipher::string_rep = { 
	{ BlockCipher::BC_ECB, "ecb" }, { BlockCipher::BC_CBC, "cbc" }, { BlockCipher::BC_CFB, "cfb" }, { BlockCipher::BC_OFB, "ofb" }
};
#else 
Encrypter::WrapBlockCipher::string_rep.insert(std::make_pair(BlockCipher::BC_ECB, std::string("ecb")));
Encrypter::WrapBlockCipher::string_rep.insert(std::make_pair(BlockCipher::BC_BCB, std::string("cbc")));
Encrypter::WrapBlockCipher::string_rep.insert(std::make_pair(BlockCipher::BC_CFB, std::string("cfb")));
Encrypter::WrapBlockCipher::string_rep.insert(std::make_pair(BlockCipher::BC_OFB, std::string("ofb")));
#endif
int Encrypter::WrapBlockCipher::max = BlockCipher::BC_MAX;
int Encrypter::WrapBlockCipher::unknown = BlockCipher::BC_UNKNOWN;

Encrypter::WrapBlockCipher::WrapBlockCipher(const BlockCipher::Mode type){
    if (type<0 || type>BlockCipher::BC_MAX)
		throw EINVAL;
	this->type = type;
	this->cp = nullptr;
	//Fill in buffer
	
};
Encrypter::WrapBlockCipher::~WrapBlockCipher(){
	delete this->cp;
}
BlockCipher::Cipher* Encrypter::WrapBlockCipher::getBlockCipherPointer(const size_t block, const std::string &iv){
	using namespace BlockCipher;
	switch (this->type){
	case BC_ECB:
		cp = new ECBEncrypter(block, iv);
		break;
	case BC_CBC:
		cp = new CBCEncrypter(block, iv);
		break;
	case BC_OFB:
		cp = new OFBEncrypter(block, iv);
		break;
	case BC_CFB:
		cp = new CFBEncrypter(block, iv);
		break;
	default:
		cp = nullptr;
		break;
	}
	return this->cp;
}
BlockCipher::Mode Encrypter::WrapBlockCipher::getType(){
	return (BlockCipher::Mode)this->type;
};

std::string Encrypter::WrapBlockCipher::stringFromType(const int c) {
	if (c <= unknown || c > max)
		return "";
	return Encrypter::aWrapF::stringFromType(c, Encrypter::WrapBlockCipher::string_rep);
}
int Encrypter::WrapBlockCipher::typeFromString(const std::string &str){
	int res = Encrypter::aWrapF::typeFromString(str, Encrypter::WrapBlockCipher::string_rep);
	return res == -1 ? Encrypter::WrapBlockCipher::unknown : res;
}

/*
std::string Encrypter::WrapHashing::stringFromType(const HashingType type){
	switch (type){
	case HT_MD5:
		return "md5";
	case HT_SHA:
		return "sha1";
	case HT_SHA256:
		return "sha256";
	}
	return "";
}
Encrypter::HashingType Encrypter::WrapHashing::typeFromString(const std::string &str){
	return HT_UNKNOWN;
}*/
//CLASS EncryptedFIle
const std::string Encrypter::EncryptedFile::h1 = "[ENCRPTD]";
const std::string Encrypter::EncryptedFile::h2 = "[DESC]";
const std::string Encrypter::EncryptedFile::eh1 = "[CLOSED]";

std::string Encrypter::genStr(const size_t &size){
	const int bufsize = 64;
	std::string res;
	size_t i;
	
	for (i = 0; i < size; i++)
		res+= (char)random(256);
	return res;
};

bool Encrypter::EncryptedFile::isEncrypted(){
	return this->enc_status;
}
Encrypter::EncryptionAlgorithm Encrypter::EncryptedFile::getEncType(){
	return this->eac;
}
Encrypter::HashingType Encrypter::EncryptedFile::getHashType(){
	return this->ht;
}
BlockCipher::Mode Encrypter::EncryptedFile::getBlockType(){
	return this->ecm;
}

int Encrypter::EncryptedFile::writeOpenPart(IO::File &file){
	file.clear();
	std::string str;
    char buf[Encrypter::EncryptedFile::HeaderSize+1];

    toChars<size_t>(buf+(EncryptedFile::HeaderSize-4),  ( this->eac<<24|this->ecm<<16|this->ht<<8|0));
    for(size_t i=0; i<sizeof(this->num_pbkdf);i++)
        buf[EncryptedFile::HeaderSize-4-i] = (unsigned char)(this->num_pbkdf>>(8*(sizeof(this->num_pbkdf)-i-1)) & 0xFF);

    buf[Encrypter::EncryptedFile::HeaderSize] = '\0';
    str= this->h1 + UUID + this->h2 + std::string(buf,buf+EncryptedFile::HeaderSize);
	if (!file.checkFile())
		file.append(str);
	else
		return 1;
	return 0;
}

std::string Encrypter::EncryptedFile::makeClosedPartHeader(){
	std::string res = eh1;
	std::string name = this->getName();
	char buf[4];
	toChars<size_t>(buf, this->getSize());
	res.append(buf, 4);
	toChars<short>(buf, (short)name.length());
	res.append(buf, 2);
	res.append(name);
	return res;
}
int Encrypter::EncryptedFile::parseClosedPart(const std::string &str, std::string &data, std::string *filename){
	size_t dataLen,pos = 0;
	short nameLen;
	//check for consistency
	if (str.compare(0, (pos+=eh1.length()), eh1))
		return 1;

	fromChars<size_t>(str.substr( pos, 4).c_str(), dataLen);
	fromChars<short>(str.substr( pos+=4, 2).c_str(), nameLen);
	pos += 2;
	if (filename)
		*filename = str.substr( pos,nameLen);
	data = str.substr( pos+=nameLen,dataLen);
	
	return 0;
}

bool Encrypter::EncryptedFile::checkHeader(){
	std::string buf;
	//1st - check size (not smaller than 192+13 = 320 bytes
	const static size_t minSize = 205;

	size_t sz = this->getSize();
	if (sz < minSize)
		return false;
	//load first 188 bytes (4 last bytes are not needed)
	if (this->loadFile(buf, 0, 188, false))
		return false;
	//2nd - check first 192 bytes
	size_t l1 = h1.length(), l2 = 172, l3 = h2.length();
	//check 10 bytes of starting signature, then check signature (172 bytes), then last mark (6 bytes)
	if (buf.compare(0, l1, h1) != 0 || buf.compare(l1, l2, Encrypter::UUID) != 0 || buf.compare(l1+l2, l3, h2) != 0)
		return false;
		
	//3rd - okay it is cool
	return true;
}

void Encrypter::EncryptedFile::mainInit(){
	//now check file
	if (!this->checkFile()){
		std::string buf;
		this->enc_status = this->checkHeader();
		if (enc_status){
			//okay, parse it
			size_t flags;
            if (this->loadFile(buf, 172+h1.length()+h2.length(), HeaderSize, false) != 0)
				throw EACCES;
			//1st byte - enc type
			//2nd byte - block-cipher mode
			//3rd byte - hashing type
			//4-5th bytes - num of PBKDF#2 calls (4th is the BIG_ENDIAN)
			fromChars(buf.c_str(), flags);
            this->eac = (EncryptionAlgorithm)buf[HeaderSize-1];
            this->ecm = (BlockCipher::Mode)buf[HeaderSize-2];
            this->ht = (HashingType)buf[HeaderSize-3];
            this->num_pbkdf = 0;
            for(size_t i=0; i<HeaderSize-3; i++){
                this->num_pbkdf |=( unsigned char)(buf[HeaderSize-4-i]&0xFF)<<(8*(3-i));
            }

			//if values are invalid
            if (this->eac > EncryptionAlgorithm::EA_MAX || this->eac <= EncryptionAlgorithm::EA_UNKNOWN ||
                this->ecm > BlockCipher::Mode::BC_MAX || this->ecm <= BlockCipher::Mode::BC_UNKNOWN ||
                this->ht > HashingType::HT_MAX||this->ht <= HashingType::HT_UNKNOWN)
				throw EINVAL;
		}

	}
	else
		this->enc_status = false;
}
Encrypter::EncryptedFile::EncryptedFile(IO::File &outFile):IO::File(outFile) {
	mainInit();
};
Encrypter::EncryptedFile::EncryptedFile(const std::string &path, const size_t chunk_size, const bool asynced) :IO::File(path, chunk_size, asynced){
	mainInit();
}

Encrypter::EncryptedFile::~EncryptedFile(){
	/**
	* 1) Send close message to the working thread on encryption
	* 2) Wait 500ms for it
	* 3) Send kill() message to the thread
	* 4) Release all associated resources
	* 5) Eat pie :3
	* 6) Call Daddy (Base class) to clean up (call Base class destructors)
	**/
	//1-4 - Not implemented

	//5 - done
	//6 - done automatically
}


int Encrypter::EncryptedFile::encrypt(const std::string &key, File &out, WrapEncryption &we, WrapHashing &wh, WrapBlockCipher &wb, std::string *outString){
	/*** DECLARATION ***/
	std::string iv, derived, salt;
	if (this->enc_status)
		return 2; //already encrypted file

	//generate IV and derived key
	salt = std::string(Encrypter::UUID, 60);
	//test pbkdf function on machine
	/* [DELETED]
	*	It's obsolete now, because genPBKDF2key is able to test the key itself

	do 	this->num_pbkdf = wh.test(key, salt, we.DEFAULT_KEYLENGTH);  //if num magically turns into zero
	while (this->num_pbkdf == 0);


	*/
	//define types
	this->ecm = wb.getType();
	this->eac = we.getType();
	this->ht = wh.getType();

    derived = wh.gen(key, salt, we.DEFAULT_KEYLENGTH, this->num_pbkdf);
    iv = (this->ecm == BlockCipher::BC_ECB) ? "" : wh.genForce(key, salt, we.DEFAULT_KEYLENGTH, this->num_pbkdf); //not needed if ECB
	salt = std::string(Encrypter::UUID, 60, 60);


	//save open scheme
	if (this->writeOpenPart(out))
		return 1;
	//make closed scheme
	std::string closed = makeClosedPartHeader();
	//encryption process on the way, BOOYAH!!	
	std::string padding = genStr(we.BLOCKSIZE - (this->getSize() + closed.length()) % we.BLOCKSIZE);
	//lock file

	//load file
	std::string data = closed;
	std::string buf;
	if (this->loadFile(buf))
		return 1;
	else{
		data.append(buf);
		buf.clear();
	}
	data.append(padding);
	//encrypt closed part, file and padding
	std::string outs;
	BlockCipher::Cipher *bm = wb.getBlockCipherPointer(we.BLOCKSIZE, iv);

	bm->Encrypt(outs, data, derived, we.getCryptoFunction());
	//unlock file
	//write to file encrypted part
	out.append(outs);
	return 0;
}
int Encrypter::EncryptedFile::decrypt(const std::string &key, File &out, std::string *outString){
	/*** DECLARATION ***/
	std::string iv, derived, salt, data, decrypted;
	WrapEncryption we(this->eac,false);
	WrapHashing wh(this->ht);
	//generate IV and derived key
	salt = std::string(Encrypter::UUID, 60);
    derived = wh.genForce(key, salt, we.DEFAULT_KEYLENGTH, this->num_pbkdf);
    iv = (this->ecm == BlockCipher::BC_ECB) ? "" : wh.genForce(key, salt, we.DEFAULT_KEYLENGTH, this->num_pbkdf); //not needed if ECB
	salt = std::string(Encrypter::UUID, 60, 60);


	//encrypt closed part, file and padding
	std::string outs;
	WrapBlockCipher *wbc = new WrapBlockCipher(this->ecm );
	BlockCipher::Cipher *bm = wbc->getBlockCipherPointer(we.BLOCKSIZE, iv);
    if (this->loadFile(data, 172 + h1.length() + h2.length() + HeaderSize))
		return 1;

	bm->Decrypt(outs, data, derived, we.getCryptoFunction() );
	delete wbc;
	if (parseClosedPart(outs, decrypted))
		return 1;
	out.overwrite(decrypted);
	return 0;
};

int Encrypter::EncryptedFile::scan(const std::string &path, std::vector<Encrypter::EncryptedFile> &vec, const bool scan_subfolders){
	std::vector<IO::DiskEntryProxy> entries;
	size_t s,i;
	if (IO::File::list(path,entries))
		return 1;
	s = entries.size();
	//check files first
	for (i = 0; i < s; i++)
		if (!(entries[i].descriptor&IO::IOE_DIR) && entries[i].descriptor&IO::IOE_READABLE){
			try{
				vec.push_back(EncryptedFile(entries[i].path));
			}
			catch (int err){
				//do something
				std::cout << "Invalid encrypted file: " << entries[i].path << std::endl;
			}
		}
	
	//check directories, if needed
	if (scan_subfolders)
		for ( i = 0; i < s; i++)
			if (entries[i].descriptor&IO::IOE_DIR)
				Encrypter::EncryptedFile::scan(entries[i].path, vec, scan_subfolders);
	
	return 0;
};

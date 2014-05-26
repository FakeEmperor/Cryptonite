#pragma once
#include "blockcipher.h"

BlockCipher::ECBEncrypter::ECBEncrypter(const size_t &block, const std::string &iv){
	this->block = block;
};
int  BlockCipher::ECBEncrypter::EncryptBlock(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f) {
	int status = 0;
	if (data.length()>this->block)
		return 1;
	buf.append(f(data, key));
	return status;
}
int BlockCipher::ECBEncrypter::DecryptBlock(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f) {
	return EncryptBlock(buf,data,key,f); //in ECB, algorithm is same as this
};
int BlockCipher::ECBEncrypter::Encrypt(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	size_t l = data.length(), n = l / this->block;
	if (l%this->block != 0)
		return 1;
	buf = "";
	for (size_t i = 0; i < n; i++)
		this->EncryptBlock(buf, data.substr(i*block,block), key, f);

	return 0;
}
int BlockCipher::ECBEncrypter::Decrypt(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	return this->Encrypt(buf,data,key,f); //same as the Encrypt function
}

BlockCipher::CBCEncrypter::CBCEncrypter(const size_t &block, const std::string &initializationVector){
	this->block = block;
	this->iv = initializationVector;
};
int  BlockCipher::CBCEncrypter::EncryptBlock(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	int status = 0;
	if (data.length()>this->block)
		return 1;
	buf.append(this->to_xor = f(xorStrings(this->to_xor, data), key));
	return status;
}
int BlockCipher::CBCEncrypter::DecryptBlock(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f) {
	int status = 0;
	if (data.length()>this->block)
		return 1;
	buf.append(xorStrings(to_xor,f(data, key) ));
	to_xor = data;
	return status;
}
int  BlockCipher::CBCEncrypter::Encrypt(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	size_t l = data.length(), n = l / this->block;
	
	if (l%this->block != 0)
		return 1;
	buf = "";
	this->to_xor = this->iv; //first round
	for (size_t i = 0; i < n; i++)
		this->EncryptBlock(buf, data.substr(block*i,block), key, f);
	return 0;
}
int  BlockCipher::CBCEncrypter::Decrypt(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	size_t l = data.length(), n = l / this->block;
	if (l%this->block != 0)
		return 1;
	this->to_xor = this->iv; //first round
	buf = "";
	for (size_t i = 0; i < n; i++)
		this->DecryptBlock(buf, data.substr(block*i, block), key, f);
	return 0;
}

BlockCipher::OFBEncrypter::OFBEncrypter(const size_t &block, const std::string &initializationVector){
	this->block = block;
	this->iv = initializationVector;
}
int  BlockCipher::OFBEncrypter::EncryptBlock(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	int status = 0;
	if (data.length()!=this->block)
		return 1;
	buf.append( xorStrings(  data,(this->to_xor = f(to_xor, key))  ) );
	return status;
}
int BlockCipher::OFBEncrypter::DecryptBlock(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f) {
	return EncryptBlock(buf, data, key, f);
}
int  BlockCipher::OFBEncrypter::Encrypt(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	size_t l = data.length(), n = l / this->block;
	if (l%this->block != 0)
		return 1;
	buf = "";
	this->to_xor = iv;  //first round
	for (size_t i = 0; i < n; i++)
		this->EncryptBlock(buf, data.substr(block*i, block), key, f);
	
		
	return 0;
}
int  BlockCipher::OFBEncrypter::Decrypt(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	return Encrypt(buf, data, key, f);
}


BlockCipher::CFBEncrypter::CFBEncrypter(const size_t &block, const std::string &initializationVector){
	this->block = block;
	this->iv = initializationVector;
}
int  BlockCipher::CFBEncrypter::EncryptBlock(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	int status = 0;
	if (data.length()!=this->block)
		return 1;
	buf.append( to_xor = xorStrings(data, f(to_xor, key)) );
	return status;
}
int BlockCipher::CFBEncrypter::DecryptBlock(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f) {
	int status = 0;
	if (data.length()!=this->block)
		return 1;
	buf.append(  xorStrings( f( to_xor,key), data ));
	to_xor = data;
	return status;
}
int  BlockCipher::CFBEncrypter::Encrypt(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	size_t l = data.length(), n = l / this->block;
	if (l%this->block != 0)
		return 1;
	this->to_xor = this->iv; //first round
	buf = "";
	for (size_t i = 0; i < n; i++)
		this->EncryptBlock(buf, data.substr(block*i, block), key, f);
	return 0;
}
int  BlockCipher::CFBEncrypter::Decrypt(std::string &buf, const std::string &data, const std::string &key, BlockCipher::CryptoFunction f){
	size_t l = data.length(), n = l / this->block;

	if (l%this->block != 0)
		return 1;
	buf = "";
	this->to_xor = this->iv; //first round
	for (size_t i = 0; i < n; i++)
		this->DecryptBlock(buf, data.substr(block*i, block), key, f);
	return 0;
}
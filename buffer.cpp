#include "buffer.h"

Buffer::Buffer() {

	_buffer.clear();
	_buffer.reserve(1024);
}


void Buffer::AddByte(const uint8_t& byte) {

	AddBytes((char*)&byte,1);
}

void Buffer::AddString(const std::string& val) {

	AddBytes(val.c_str(), val.length());
	char ch = 10;
	AddBytes(&ch,1);
}

void Buffer::writeWordA(int i) {

	char buffer[2];

	buffer[0] = (char)(i >> 8);
	buffer[1] = (char)(i + 128);

	AddBytes(buffer,2);
}

void Buffer::AddBytes(const char* bytes, const size_t& size) {

	for (size_t i = 0; i < size; i++) {

		_buffer.emplace_back(bytes[i]);
	}
}
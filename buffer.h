#pragma once
#include <vector>
#include <string>

//a buffer that collect data to send them
class Buffer {
public:
	Buffer();

	size_t Size() { return _buffer.size(); }
	char* Data() { return _buffer.data(); }

	template<class T>
	void AddVariable(const T& value) {

		T temp = ReverseBytes(value);
		AddBytes((char*)(&temp), sizeof(T));
	}

	void AddByte(const uint8_t& byte);
	void AddNegativeByte(const char& byte) {
		AddByte(byte*-1);
	}
	void AddByteA(const char& byte) {
		AddByte(byte +128);
	}

	void AddString(const std::string& val);

	void writeWordA(int i);

protected:
	template<class T>
	T ReverseBytes(const T& num) {
		T newNum;
		const int byteCount = sizeof(T);

		char buffer[byteCount];
		memset(buffer, 0, byteCount);

		for (int i = 0; i < byteCount; i++) {
			buffer[byteCount - i - 1] = num >> i * 8 & 0xFF;
		}

		memcpy(&newNum, &buffer[0], byteCount);
		return newNum;
	}

	void AddBytes(const char* bytes, const size_t& size);

private:
	std::vector<char>  _buffer;
};
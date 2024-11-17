#pragma once

#include <string>
#include <type_traits>
typedef unsigned char byte;

byte FloatToByte(float value);

byte IntToByte(int value);

byte UnsignedIntToByte(unsigned int value);

byte BoolToByte(bool value);

byte DoubleToByte(double value);

byte CharToByte(char value);

byte UnsignedCharToByte(unsigned char value);

byte ShortToByte(short value);

byte UnsignedShortToByte(unsigned short value);

byte LongToByte(long value);

byte UnsignedLongToByte(unsigned long value);

byte LongLongToByte(long long value);

byte UnsignedLongLongToByte(unsigned long long value);

byte StringToByte(const std::string& value);

template<typename T>
byte ConvertToByte(T value) {
	if constexpr (std::is_same<T, float>::value) {
		return FloatToByte(value);
	} else if constexpr (std::is_same<T, int>::value) {
		return IntToByte(value);
	} else if constexpr (std::is_same<T, unsigned int>::value) {
		return UnsignedIntToByte(value);
	} else if constexpr (std::is_same<T, bool>::value) {
		return BoolToByte(value);
	} else if constexpr (std::is_same<T, double>::value) {
		return DoubleToByte(value);
	} else if constexpr (std::is_same<T, char>::value) {
		return CharToByte(value);
	} else if constexpr (std::is_same<T, unsigned char>::value) {
		return UnsignedCharToByte(value);
	} else if constexpr (std::is_same<T, short>::value) {
		return ShortToByte(value);
	} else if constexpr (std::is_same<T, unsigned short>::value) {
		return UnsignedShortToByte(value);
	} else if constexpr (std::is_same<T, long>::value) {
		return LongToByte(value);
	} else if constexpr (std::is_same<T, unsigned long>::value) {
		return UnsignedLongToByte(value);
	} else if constexpr (std::is_same<T, long long>::value) {
		return LongLongToByte(value);
	} else if constexpr (std::is_same<T, unsigned long long>::value) {
		return UnsignedLongLongToByte(value);
	} else if constexpr (std::is_same<T, std::string>::value) {
		return StringToByte(value);
	} else if constexpr (std::is_same<T, const char*>::value) {
		return StringToByte(std::string(value));
	} else {
		return static_cast<byte>(value);
	}
}
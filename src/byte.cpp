#include "byte.h"

byte FloatToByte(float value) {
	if (value < 0.0f) {
		value = 0.0f;
	} else if (value > 1.0f) {
		value = 1.0f;
	}
	return static_cast<byte>(value * 255.0f);
}

byte IntToByte(int value) {
	if (value < 0) {
		return 0;
	} else if (value > 255) {
		return 255;
	} else {
		return static_cast<byte>(value);
	}
}

byte UnsignedIntToByte(unsigned int value) {
	if (value > 255) {
		return 255;
	} else {
		return static_cast<byte>(value);
	}
}

byte BoolToByte(bool value) {
	return value ? 0xFF : 0x00;
}

byte DoubleToByte(double value) {
	if (value < 0.0) {
		value = 0.0;
	} else if (value > 1.0) {
		value = 1.0;
	}
	return static_cast<byte>(value * 255.0);
}

byte CharToByte(char value) {
	return static_cast<byte>(value);
}

byte UnsignedCharToByte(unsigned char value) {
	return value;
}

byte ShortToByte(short value) {
	return IntToByte(value);
}

byte UnsignedShortToByte(unsigned short value) {
	return UnsignedIntToByte(value);
}

byte LongToByte(long value) {
	return IntToByte(static_cast<int>(value));
}

byte UnsignedLongToByte(unsigned long value) {
	return UnsignedIntToByte(static_cast<unsigned int>(value));
}

byte LongLongToByte(long long value) {
	return IntToByte(static_cast<int>(value));
}

byte UnsignedLongLongToByte(unsigned long long value) {
	return UnsignedIntToByte(static_cast<unsigned int>(value));
}

byte StringToByte(const std::string& value) {
	if (!value.empty()) {
		return CharToByte(value[0]);
	}
	return 0;
}
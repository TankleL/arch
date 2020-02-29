#include "vuint.hpp"
#include <cassert>

VUInt::VUInt() noexcept
	: _data({0})
	, _esize(0)
	, _dsize(0)
	, _digst(DS_Idle)
	, _value(0)
{
	value(0);
}

VUInt::VUInt(std::uint32_t val) noexcept
	: _data({0})
	, _esize(0)
	, _dsize(0)
	, _digst(DS_Idle)
	, _value(0)
{
	value(val);
}

VUInt::VUInt(const VUInt& rhs) noexcept
	: _data(rhs._data)
	, _esize(rhs._esize)
	, _dsize(rhs._dsize)
	, _digst(rhs._digst)
	, _value(rhs._value)
{
}

VUInt::VUInt(VUInt&& rhs) noexcept
	: _data(std::move(rhs._data))
	, _esize(rhs._esize)
	, _dsize(rhs._dsize)
	, _digst(rhs._digst)
	, _value(rhs._value)
{}

VUInt& VUInt::operator=(const VUInt& rhs) noexcept
{
	_data = rhs._data;
	_esize = rhs._esize;
	_dsize = rhs._dsize;
	_digst = rhs._digst;
	_value = rhs._value;
	return *this;
}

VUInt& VUInt::operator=(VUInt&& rhs) noexcept
{
	_data = std::move(rhs._data);
	_esize = rhs._esize;
	_dsize = rhs._dsize;
	_digst = rhs._digst;
	_value = rhs._value;
	return *this;
}

VUInt::~VUInt()
{}

std::uint32_t VUInt::value() const noexcept
{
	return _value;
}

int VUInt::decoded_size() const noexcept
{
	return _dsize;
}

void VUInt::value(std::uint32_t val) noexcept
{
	memset(_data.data(), 0, _data.size());

	if (val < 0x80)  // 0 ~ 7 bits
	{
		// [byte 0] 0xxx xxxx

		_dsize = 1;
		_esize = 1;
		_data[0] = val;
	}
	else if (val < 0x4000) // 8 ~ 14 bits
	{
		// [byte 0] 10xx xxxx
		// [byte 1] xxxx xxxx

		_dsize = 2;
		_esize = 2;
		_data[0] = 0x80 | (val & 0x3f);
		_data[1] = (std::uint8_t) (val >> 6);
	}
	else if (val < 0x200000) // 15 ~ 21 bits
	{
		// [byte 0] 110x xxxx
		// [byte 1] xxxx xxxx
		// [byte 2] xxxx xxxx

		_dsize = 3;
		_esize = 3;
		_data[0] = 0xc0 | (val & 0x1f);
		_data[1] = (std::uint8_t)((val & 0x1fe0) >> 5);
		_data[2] = (std::uint8_t)(val >> 13);
	}
	else if(val < 0x10000000) // 22 ~ 28 bits
	{
		// [byte 0] 1110 xxxx
		// [byte 1] xxxx xxxx
		// [byte 2] xxxx xxxx
		// [byte 3] xxxx xxxx

		_dsize = 4;
		_esize = 4;
		_data[0] = 0xe0 | (val & 0x0f);
		_data[1] = (val & 0xff0) >> 4;
		_data[2] = (val & 0xff000) >> 12;
		_data[3] = val >> 20;
	}
	else // 29 ~ 32 bits
	{
		// [byte 0] 1111 0xxx
		// [byte 1] xxxx xxxx
		// [byte 2] xxxx xxxx
		// [byte 3] xxxx xxxx
		// [byte 4] xxxx xxxx

		_dsize = 4;
		_esize = 5;
		_data[0] = 0xf0 | (val & 0x7);
		_data[1] = (val & 0x7f8) >> 3;
		_data[2] = (val & 0x7f800) >> 11;
		_data[3] = (val & 0x7f80000) >> 19;
		_data[4] = val >> 27;
	}

	_value = val;
}

VUInt::DigestStatus VUInt::digest(const std::uint8_t& byte) noexcept
{
	switch (_digst)
	{
	case DS_Idle:
		memset(_data.data(), 0, _data.size());
		_data[0] = byte;
		if ((byte & 0x80) == 0) // 0 ~ 7 bits
		{
			_value = byte & 0x7f;
			_esize = 1;
			_dsize = 1;
		}
		else if((byte & 0xc0) == 0x80) // 8 ~ 14 bits
		{
			_digst = DS_Decode_0;
			_value = byte & 0x3f;
			_esize = 2;
			_dsize = 2;
		}
		else if((byte & 0xe0) == 0xc0) // 15 ~ 21 bits
		{
			_digst = DS_Decode_0;
			_value = byte & 0x1f;
			_esize = 3;
			_dsize = 3;
		}
		else if((byte & 0xf0) == 0xe0) // 22 ~ 28 bits
		{
			_digst = DS_Decode_0;
			_value = byte & 0xf;
			_esize = 4;
			_dsize = 4;
		}
		else if ((byte & 0xf8) == 0xf0) // 29 ~ 32 bits
		{
			_digst = DS_Decode_0;
			_value = byte & 0x7;
			_esize = 5;
			_dsize = 4;
		}
		else
		{ // bad data
			_digst = DS_Bad;
		}
		break;

	case DS_Decode_0:
		_data[1] = byte;
		switch (_esize)
		{
		case 2:
			_digst = DS_Idle;
			_value |= byte << 6;
			break;

		case 3:
			_digst = DS_Decode_1;
			_value |= byte << 5;
			break;

		case 4:
			_digst = DS_Decode_1;
			_value |= byte << 4;
			break;

		case 5:
			_digst = DS_Decode_1;
			_value |= byte << 3;
			break;

		default:
			_digst = DS_Bad;
		}
		break;

	case DS_Decode_1:
		_data[2] = byte;
		switch (_esize)
		{
		case 3:
			_digst = DS_Idle;
			_value |= byte << 13;
			break;

		case 4:
			_digst = DS_Decode_2;
			_value |= byte << 12;
			break;

		case 5:
			_digst = DS_Decode_2;
			_value |= byte << 11;
			break;

		default:
			_digst = DS_Bad;
		}
		break;

	case DS_Decode_2:
		_data[3] = byte;
		switch(_esize)
		{
		case 4:
			_digst = DS_Idle;
			_value |= byte << 21;
			break;

		case 5:
			_digst = DS_Decode_3;
			_value |= byte << 20;
			break;

		default:
			_digst = DS_Bad;
		}
		break;

	case DS_Decode_3:
		_data[4] = byte;
		if(_esize == 5)
		{
			_digst = DS_Idle;
			_value |= byte << 28;
		}
		else
		{
			_digst = DS_Bad;
		}
		break;

	default:
		_digst = DS_Bad;
	}

	return _digst;
}

int VUInt::encoded_size() const noexcept
{
	return _esize;
}

const std::uint8_t* VUInt::encoded_data() const noexcept
{
	return _data.data();
}

std::vector<uint8_t>&
operator<<(std::vector<uint8_t>& dest, const VUInt& vuint)
{
	dest.insert(
		dest.end(),
		vuint.encoded_data(),
		vuint.encoded_data() + vuint.encoded_size());
	return dest;
}





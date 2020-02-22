#include "asvc-vint.hpp"

VInt::VInt() noexcept
{}

VInt::VInt(std::int32_t val) noexcept
	: VUInt(_zz_map(val))
{}

VInt::~VInt()
{}

std::int32_t VInt::value() const noexcept
{
	return _zz_unmap(VUInt::value());
}

int VInt::decoded_size() const noexcept
{
	return VUInt::decoded_size();
}

void VInt::value(std::int32_t val) noexcept
{
	VUInt::value(_zz_map(val));
}

VInt::DigestStatus VInt::digest(const std::uint8_t& byte) noexcept
{
	return VUInt::digest(byte);
}

int VInt::encoded_size() const noexcept
{
	return VUInt::encoded_size();
}

const std::uint8_t* VInt::encoded_data() const noexcept
{
	return VUInt::encoded_data();
}

std::uint32_t VInt::_zz_map(std::int32_t n) noexcept
{
	return (n << 1) ^ (n >> 31);
}

std::int32_t VInt::_zz_unmap(std::uint32_t n) noexcept
{
	return (n >> 1) ^ -static_cast<int32_t>(n & 1);
}

std::vector<uint8_t>&
operator<<(std::vector<uint8_t>& dest, const VInt& vint)
{
	dest.insert(
		dest.end(),
		vint.encoded_data(),
		vint.encoded_data() + vint.encoded_size());
	return dest;
}






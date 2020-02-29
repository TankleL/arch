#pragma once

#include "asvc-vuint.hpp"

class VInt : private VUInt
{
public:
	VInt() noexcept;
	VInt(std::int32_t val) noexcept;
	VInt(const VInt& rhs) noexcept;
	VInt(VInt&& rhs) noexcept;
	VInt& operator=(const VInt& rhs) noexcept;
	VInt& operator=(VInt&& rhs) noexcept;
	~VInt() noexcept;

public:
	std::int32_t	value() const noexcept;
	int				decoded_size() const noexcept;
	
	void value(std::int32_t val) noexcept;

	DigestStatus	digest(const std::uint8_t& byte) noexcept;
	int				encoded_size() const noexcept;
	const std::uint8_t*	encoded_data() const noexcept;

private:
	static std::uint32_t	_zz_map(std::int32_t n) noexcept;
	static std::int32_t		_zz_unmap(std::uint32_t n) noexcept;
};

std::vector<uint8_t>& operator<<(std::vector<uint8_t>& dest, const VInt& vuint);




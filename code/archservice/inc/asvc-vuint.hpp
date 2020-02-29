#pragma once
#include <cstdint>
#include <array>
#include <vector>


class VUInt
{
public:
	enum DigestStatus : int
	{
		DS_Idle = 0,
		DS_Decode_0,
		DS_Decode_1,
		DS_Decode_2,
		DS_Decode_3,
		DS_Bad
	};

public:
	VUInt() noexcept;
	VUInt(std::uint32_t val) noexcept;
	VUInt(const VUInt& rhs) noexcept;
	VUInt(VUInt&& rhs) noexcept;
	VUInt& operator=(const VUInt& rhs) noexcept;
	VUInt& operator=(VUInt&& rhs) noexcept;
	~VUInt() noexcept;

public:
	std::uint32_t	value() const noexcept;
	int				decoded_size() const noexcept;
	
	void value(std::uint32_t val) noexcept;

	DigestStatus	digest(const std::uint8_t& byte) noexcept;
	int				encoded_size() const noexcept;
	const std::uint8_t*	encoded_data() const noexcept;

private:
	std::array<std::uint8_t, 6>	_data;
	std::uint8_t				_esize;		// encoded size
	std::uint8_t				_dsize;		// decoded size
	DigestStatus				_digst;
	std::uint32_t				_value;
};

std::vector<uint8_t>& operator<<(std::vector<uint8_t>& dest, const VUInt& vuint);



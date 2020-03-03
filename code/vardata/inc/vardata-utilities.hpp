#pragma once

#include <vector>
#include <cstdint>

std::vector<uint8_t>& operator<<(
	std::vector<uint8_t>& left,
	const std::vector<uint8_t>& right)
{
	left.insert(
		left.end(),
		right.cbegin(),
		right.cend());
	return left;
}



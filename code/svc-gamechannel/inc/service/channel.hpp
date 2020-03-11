#pragma once

#include "pre-req.hpp"
#include "character.hpp"

class Channel
{
public:
	typedef uint32_t	id_t;

public:


private:
	id_t							_id;

	std::unordered_map<
		Character::id_t,
		std::unique_ptr<Character>> _chars;
};



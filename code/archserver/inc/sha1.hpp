/*
    sha1.hpp - header of

    ============
    SHA-1 in C++
    ============

    100% Public Domain.

    Original C Code
        -- Steve Reid <steve@edmweb.com>
    Small changes to fit into bglibs
        -- Bruce Guenter <bruce@untroubled.org>
    Translation to simpler C++ Code
        -- Volker Diels-Grabsch <v@njh.eu>
    Safety fixes
        -- Eugene Hopkinson <slowriot at voxelstorm dot com>
*/

#pragma once

#include "archserver-prereq.hpp"


namespace arch
{

		class SHA1
		{
		public:
			SHA1();
			void update(const std::string& s);
			void update(std::istream& is);
			std::string final();
			std::string final_base64();
			static std::string from_file(const std::string& filename);

		private:
			static void _reset(uint32_t digest[], std::string& buffer, uint64_t& transforms);
			static uint32_t _rol(const uint32_t value, const size_t bits);
			static uint32_t _blk(const uint32_t* block, const size_t i);
			static void _R0(const uint32_t* block, const uint32_t v, uint32_t& w, const uint32_t x, const uint32_t y, uint32_t& z, const size_t i);
			static void _R1(uint32_t* block, const uint32_t v, uint32_t& w, const uint32_t x, const uint32_t y, uint32_t& z, const size_t i);
			static void _R2(uint32_t* block, const uint32_t v, uint32_t& w, const uint32_t x, const uint32_t y, uint32_t& z, const size_t i);
			static void _R3(uint32_t* block, const uint32_t v, uint32_t& w, const uint32_t x, const uint32_t y, uint32_t& z, const size_t i);
			static void _R4(uint32_t* block, const uint32_t v, uint32_t& w, const uint32_t x, const uint32_t y, uint32_t& z, const size_t i);
			static void _transform(uint32_t digest[], uint32_t block[], uint64_t& transforms);
			static void _buffer_to_block(const std::string& buffer, uint32_t* block);

		private:
			static const size_t BLOCK_INTS;  /* number of 32bit integers per SHA1 block */
			static const size_t BLOCK_BYTES;

		private:
			uint32_t digest[5];
			std::string buffer;
			uint64_t transforms;
		};

}

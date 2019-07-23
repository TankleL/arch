/*
    sha1.cpp - source code of

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

#include "sha1.hpp"
#include "base64.hpp"

using namespace std;
using namespace arch;

const size_t SHA1::BLOCK_INTS = 16;  /* number of 32bit integers per SHA1 block */
const size_t SHA1::BLOCK_BYTES = BLOCK_INTS * 4;

void SHA1::_reset(uint32_t digest[], std::string &buffer, uint64_t &transforms)
{
    /* SHA1 initialization constants */
    digest[0] = 0x67452301;
    digest[1] = 0xefcdab89;
    digest[2] = 0x98badcfe;
    digest[3] = 0x10325476;
    digest[4] = 0xc3d2e1f0;

    /* Reset counters */
    buffer = "";
    transforms = 0;
}


uint32_t SHA1::_rol(const uint32_t value, const size_t bits)
{
    return (value << bits) | (value >> (32 - bits));
}


uint32_t SHA1::_blk(const uint32_t* block, const size_t i)
{
    return _rol(block[(i+13)&15] ^ block[(i+8)&15] ^ block[(i+2)&15] ^ block[i], 1);
}


/*
 * (_R0+_R1), _R2, _R3, _R4 are the different operations used in SHA1
 */

void SHA1::_R0(const uint32_t* block, const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
    z += ((w&(x^y))^y) + block[i] + 0x5a827999 + _rol(v, 5);
    w = _rol(w, 30);
}


void SHA1::_R1(uint32_t* block, const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
    block[i] = _blk(block, i);
    z += ((w&(x^y))^y) + block[i] + 0x5a827999 + _rol(v, 5);
    w = _rol(w, 30);
}


void SHA1::_R2(uint32_t* block, const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
    block[i] = _blk(block, i);
    z += (w^x^y) + block[i] + 0x6ed9eba1 + _rol(v, 5);
    w = _rol(w, 30);
}


void SHA1::_R3(uint32_t* block, const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
    block[i] = _blk(block, i);
    z += (((w|x)&y)|(w&x)) + block[i] + 0x8f1bbcdc + _rol(v, 5);
    w = _rol(w, 30);
}


void SHA1::_R4(uint32_t* block, const uint32_t v, uint32_t &w, const uint32_t x, const uint32_t y, uint32_t &z, const size_t i)
{
    block[i] = _blk(block, i);
    z += (w^x^y) + block[i] + 0xca62c1d6 + _rol(v, 5);
    w = _rol(w, 30);
}


/*
 * Hash a single 512-bit block. This is the core of the algorithm.
 */

void SHA1::_transform(uint32_t digest[], uint32_t block[BLOCK_INTS], uint64_t &transforms)
{
    /* Copy digest[] to working vars */
    uint32_t a = digest[0];
    uint32_t b = digest[1];
    uint32_t c = digest[2];
    uint32_t d = digest[3];
    uint32_t e = digest[4];

    /* 4 rounds of 20 operations each. Loop unrolled. */
    _R0(block, a, b, c, d, e,  0);
    _R0(block, e, a, b, c, d,  1);
    _R0(block, d, e, a, b, c,  2);
    _R0(block, c, d, e, a, b,  3);
    _R0(block, b, c, d, e, a,  4);
    _R0(block, a, b, c, d, e,  5);
    _R0(block, e, a, b, c, d,  6);
    _R0(block, d, e, a, b, c,  7);
    _R0(block, c, d, e, a, b,  8);
    _R0(block, b, c, d, e, a,  9);
    _R0(block, a, b, c, d, e, 10);
    _R0(block, e, a, b, c, d, 11);
    _R0(block, d, e, a, b, c, 12);
    _R0(block, c, d, e, a, b, 13);
    _R0(block, b, c, d, e, a, 14);
    _R0(block, a, b, c, d, e, 15);
    _R1(block, e, a, b, c, d,  0);
    _R1(block, d, e, a, b, c,  1);
    _R1(block, c, d, e, a, b,  2);
    _R1(block, b, c, d, e, a,  3);
    _R2(block, a, b, c, d, e,  4);
    _R2(block, e, a, b, c, d,  5);
    _R2(block, d, e, a, b, c,  6);
    _R2(block, c, d, e, a, b,  7);
    _R2(block, b, c, d, e, a,  8);
    _R2(block, a, b, c, d, e,  9);
    _R2(block, e, a, b, c, d, 10);
    _R2(block, d, e, a, b, c, 11);
    _R2(block, c, d, e, a, b, 12);
    _R2(block, b, c, d, e, a, 13);
    _R2(block, a, b, c, d, e, 14);
    _R2(block, e, a, b, c, d, 15);
    _R2(block, d, e, a, b, c,  0);
    _R2(block, c, d, e, a, b,  1);
    _R2(block, b, c, d, e, a,  2);
    _R2(block, a, b, c, d, e,  3);
    _R2(block, e, a, b, c, d,  4);
    _R2(block, d, e, a, b, c,  5);
    _R2(block, c, d, e, a, b,  6);
    _R2(block, b, c, d, e, a,  7);
    _R3(block, a, b, c, d, e,  8);
    _R3(block, e, a, b, c, d,  9);
    _R3(block, d, e, a, b, c, 10);
    _R3(block, c, d, e, a, b, 11);
    _R3(block, b, c, d, e, a, 12);
    _R3(block, a, b, c, d, e, 13);
    _R3(block, e, a, b, c, d, 14);
    _R3(block, d, e, a, b, c, 15);
    _R3(block, c, d, e, a, b,  0);
    _R3(block, b, c, d, e, a,  1);
    _R3(block, a, b, c, d, e,  2);
    _R3(block, e, a, b, c, d,  3);
    _R3(block, d, e, a, b, c,  4);
    _R3(block, c, d, e, a, b,  5);
    _R3(block, b, c, d, e, a,  6);
    _R3(block, a, b, c, d, e,  7);
    _R3(block, e, a, b, c, d,  8);
    _R3(block, d, e, a, b, c,  9);
    _R3(block, c, d, e, a, b, 10);
    _R3(block, b, c, d, e, a, 11);
    _R4(block, a, b, c, d, e, 12);
    _R4(block, e, a, b, c, d, 13);
    _R4(block, d, e, a, b, c, 14);
    _R4(block, c, d, e, a, b, 15);
    _R4(block, b, c, d, e, a,  0);
    _R4(block, a, b, c, d, e,  1);
    _R4(block, e, a, b, c, d,  2);
    _R4(block, d, e, a, b, c,  3);
    _R4(block, c, d, e, a, b,  4);
    _R4(block, b, c, d, e, a,  5);
    _R4(block, a, b, c, d, e,  6);
    _R4(block, e, a, b, c, d,  7);
    _R4(block, d, e, a, b, c,  8);
    _R4(block, c, d, e, a, b,  9);
    _R4(block, b, c, d, e, a, 10);
    _R4(block, a, b, c, d, e, 11);
    _R4(block, e, a, b, c, d, 12);
    _R4(block, d, e, a, b, c, 13);
    _R4(block, c, d, e, a, b, 14);
    _R4(block, b, c, d, e, a, 15);

    /* Add the working vars back into digest[] */
    digest[0] += a;
    digest[1] += b;
    digest[2] += c;
    digest[3] += d;
    digest[4] += e;

    /* Count the number of transformations */
    transforms++;
}


void SHA1::_buffer_to_block(const std::string &buffer, uint32_t block[BLOCK_INTS])
{
    /* Convert the std::string (byte buffer) to a uint32_t array (MSB) */
    for (size_t i = 0; i < BLOCK_INTS; i++)
    {
        block[i] = (buffer[4*i+3] & 0xff)
                   | (buffer[4*i+2] & 0xff)<<8
                   | (buffer[4*i+1] & 0xff)<<16
                   | (buffer[4*i+0] & 0xff)<<24;
    }
}


SHA1::SHA1()
{
    _reset(digest, buffer, transforms);
}


void SHA1::update(const std::string &s)
{
    std::istringstream is(s);
    update(is);
}


void SHA1::update(std::istream &is)
{
    while (true)
    {
        char sbuf[BLOCK_BYTES];
        is.read(sbuf, BLOCK_BYTES - buffer.size());
        buffer.append(sbuf, (std::size_t)is.gcount());
        if (buffer.size() != BLOCK_BYTES)
        {
            return;
        }
        uint32_t block[BLOCK_INTS];
        _buffer_to_block(buffer, block);
        _transform(digest, block, transforms);
        buffer.clear();
    }
}


/*
 * Add padding and return the message digest.
 */

std::string SHA1::final()
{
    /* Total number of hashed bits */
    uint64_t total_bits = (transforms*BLOCK_BYTES + buffer.size()) * 8;

    /* Padding */
    buffer += (char)0x80;
    size_t orig_size = buffer.size();
    while (buffer.size() < BLOCK_BYTES)
    {
        buffer += (char)0x00;
    }

    uint32_t block[BLOCK_INTS];
    _buffer_to_block(buffer, block);

    if (orig_size > BLOCK_BYTES - 8)
    {
        _transform(digest, block, transforms);
        for (size_t i = 0; i < BLOCK_INTS - 2; i++)
        {
            block[i] = 0;
        }
    }

    /* Append total_bits, split this uint64_t into two uint32_t */
    block[BLOCK_INTS - 1] = (uint32_t)total_bits;
    block[BLOCK_INTS - 2] = (uint32_t)(total_bits >> 32);
    _transform(digest, block, transforms);

    /* Hex std::string */
    std::ostringstream result;
    for (size_t i = 0; i < sizeof(digest) / sizeof(digest[0]); i++)
    {
        result << std::hex << std::setfill('0') << std::setw(8);
        result << digest[i];
    }

    /* Reset for next run */
    _reset(digest, buffer, transforms);

    return result.str();
}

std::string SHA1::final_base64()
{
	/* Total number of hashed bits */
	uint64_t total_bits = (transforms*BLOCK_BYTES + buffer.size()) * 8;

	/* Padding */
	buffer += (char)0x80;
	size_t orig_size = buffer.size();
	while (buffer.size() < BLOCK_BYTES)
	{
		buffer += (char)0x00;
	}

	uint32_t block[BLOCK_INTS];
	_buffer_to_block(buffer, block);

	if (orig_size > BLOCK_BYTES - 8)
	{
		_transform(digest, block, transforms);
		for (size_t i = 0; i < BLOCK_INTS - 2; i++)
		{
			block[i] = 0;
		}
	}

	/* Append total_bits, split this uint64_t into two uint32_t */
	block[BLOCK_INTS - 1] = (uint32_t)total_bits;
	block[BLOCK_INTS - 2] = (uint32_t)(total_bits >> 32);
	_transform(digest, block, transforms);


	char result_bytes[sizeof(digest)];
	for (size_t i = 0; i < sizeof(digest) / sizeof(digest[0]); ++i)
	{
		result_bytes[i * 4 + 3] = (char)(digest[i] & 0xff);
		result_bytes[i * 4 + 2] = (char)((digest[i] & 0xff00) >> 8);
		result_bytes[i * 4 + 1] = (char)((digest[i] & 0xff0000) >> 16);
		result_bytes[i * 4] = (char)((digest[i] & 0xff000000) >> 24);
	}
	std::string result = Base64Util::encode((unsigned char*)result_bytes, sizeof(digest));;

	/* Reset for next run */
	_reset(digest, buffer, transforms);

	return result;
}


std::string SHA1::from_file(const std::string &filename)
{
    std::ifstream stream(filename.c_str(), std::ios::binary);
    SHA1 checksum;
    checksum.update(stream);
    return checksum.final();
}
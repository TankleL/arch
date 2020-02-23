#pragma once

#include <vector>
#include <cstdint>

namespace archsvc
{

	class RawSvcDataHandler
	{
	public:
		enum ParsingPhase : int
		{
			PP_Idle,
			PP_ConnIDH8,
			PP_ConnIDL8,
			PP_High8,
			PP_Low8,
			PP_Data,

			PP_Bad
		};

		union payload_u16_u
		{
			 payload_u16_u()
				: value(0)
			{}

			uint16_t value;

			struct
			{
				uint8_t low;
				uint8_t high;
			};
		};

	public:
		RawSvcDataHandler();
		~RawSvcDataHandler();

	public:
		ParsingPhase deserialize(
			const std::uint8_t* data,
			size_t length,
			size_t& procbytes);
		
		void serialiaze(
			std::vector<uint8_t>& odata,
			const uint16_t& conn_id,
			const std::vector<uint8_t>& idata);

		void get_deserialized(std::vector<uint8_t>& data);

	private:
		static uint8_t	_high8(uint16_t n);
		static uint8_t	_low8(uint16_t n);

	private:
		std::vector<uint8_t>	_temp;
		ParsingPhase			_pp;
		payload_u16_u			_len;
		payload_u16_u			_conn_id;
	};

}



#pragma once

#include "archserver-prereq.hpp"
#include "module.hpp"

namespace arch
{

	class BuiltinModule_Http
		: public IModule
	{
	public:
		virtual bool	init() override;
		virtual void	uninit() override;
		virtual void	process(ArchMessage& onode, const ArchMessage& inode) override;
		virtual void	dispose() noexcept override;
	};


	class ModuleManager;
	class BuiltinModule_WebSocket
		: public IModule
	{
	public:
		BuiltinModule_WebSocket(const ModuleManager* mgr) noexcept;

	public:
		virtual bool	init() override;
		virtual void	uninit() override;
		virtual void	process(ArchMessage& onode, const ArchMessage& inode) override;
		virtual void	dispose() noexcept override;

	private:
		typedef struct Header
		{
			/*
			/ ----------------------------------------------------------------------------------------------------- \
			|| 0 1 2 3 , 4 5 6 7 , 8 9 10 11 , 12 13 14 15 , 16 17 18 19 , 20 21 22 23 , 24 25 26 27 , 28 29 30 31 ||
			|| F|  V   | Z|                                |                                                       ||
			|| I|  E   | I|         Reserved               |                     Protocol Code                     ||
			|| N|  R   | P|                                |                                                       ||
			\ ----------------------------------------------------------------------------------------------------- /
			*/
			union
			{
				struct
				{
					std::uint8_t	ctrl_b0;
					std::uint8_t	ctrl_b2;
					std::uint16_t	proto_code;
				};

				std::uint32_t		dw0;
			};

			Header() noexcept;

			bool			get_fin() const;
			int				get_version() const;
			bool			get_zip() const;

			void	set_fin(bool fin);
			void	set_version(int version);
			void	set_zip(bool zip);

			void	digest(const byte_t* data);

		} header_t;

	private:
		const ModuleManager* _mdl_mgr;
	};

}

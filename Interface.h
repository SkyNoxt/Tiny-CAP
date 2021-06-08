
#pragma once

#include <pcap.h>

namespace TinyCAP
{
	typedef pcap_if_t Interface;

	class Interfaces
	{
	  public:
		unsigned int count = 0;

		Interfaces();

		Interface& operator[](int index);

		~Interfaces();

	  private:
		Interface **interfaces = nullptr, *devices = nullptr;
	};
}
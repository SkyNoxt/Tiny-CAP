
#pragma once

#include <condition_variable>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>

#include "Interface.h"

namespace TinyCAP
{
	class PCAP
	{
	  public:
		struct Packet
		{
			const pcap_pkthdr header;
			const unsigned char data[];
		};

		PCAP(const Interface& device, bool promiscuous = true);
		PCAP(const char* file);

		void start(std::function<void(const Packet*)> callback, int count = 0);
		void stop();

		unsigned int inject(const void* data, unsigned int size);

		~PCAP();

	  private:
		pcap_t* handle = nullptr;

		std::mutex mutex;
		std::condition_variable condition;
		std::queue<Packet*> queue;

		std::thread producer;
		std::thread consumer;

		bool finished = true;

		void produce(int count);
		void consume(std::function<void(const Packet*)> callback);
	};
}
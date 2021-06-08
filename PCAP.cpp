
#include <cstring>

#include "PCAP.h"

using TinyCAP::PCAP;

PCAP::PCAP(const Interface& device, bool promiscuous)
{
	char buffer[PCAP_ERRBUF_SIZE];
	handle = pcap_open_live(device.name, BUFSIZ, promiscuous, 1, buffer);
}

PCAP::PCAP(const char* file)
{
	char buffer[PCAP_ERRBUF_SIZE];
	handle = pcap_open_offline(file, buffer);
}

void PCAP::start(std::function<void(const Packet*)> callback, int count)
{
	finished = false;
	producer = std::thread(&PCAP::produce, this, count);
	consumer = std::thread(&PCAP::consume, this, callback);
}

void PCAP::stop()
{
	finished = true;

	pcap_breakloop(handle);

	if(producer.joinable())
		producer.join();
	if(consumer.joinable())
		consumer.join();
}

unsigned int PCAP::inject(const void* data, unsigned int size)
{
	return pcap_inject(handle, data, size);
}

PCAP::~PCAP()
{
	if(!finished)
		stop();
	pcap_close(handle);
}

void PCAP::produce(int count)
{
	pcap_loop(
		handle, count, [](unsigned char* object, const pcap_pkthdr* header, const unsigned char* data)
		{
			PCAP* pcap = (PCAP*)object;

			void* buffer = std::malloc(sizeof(pcap_pkthdr) + header->caplen);
			Packet* packet = (Packet*)buffer;

			std::memcpy((void*)&packet->header, header, sizeof(pcap_pkthdr));
			std::memcpy((void*)&packet->data, data, header->caplen);

			{
				std::lock_guard<std::mutex> lock(pcap->mutex);
				pcap->queue.push(packet);
			}
			pcap->condition.notify_all();
		},
		(unsigned char*)this);
	{
		std::lock_guard<std::mutex> lock(mutex);
		finished = true;
	}
	condition.notify_all();
}

void PCAP::consume(std::function<void(const Packet*)> callback)
{
	while(true)
	{
		std::unique_lock<std::mutex> lock(mutex);
		condition.wait(lock, [this]
					   { return this->finished || !this->queue.empty(); });
		while(!queue.empty())
		{
			callback(queue.front());
			delete queue.front();
			queue.pop();
		}
		if(finished)
			break;
	}
}
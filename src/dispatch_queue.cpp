#include "dispatch_queue.hpp"

#include <list>
#include <iostream>
#include <sstream>

#include <cyanid.hpp>

using namespace cyanid;
using namespace std;

DispatchQueue::DispatchQueue(cyanid::device& dev, const string& gateway_ip)
    : listener(dev), gateway_ip(gateway_ip)
{
    init_filter();
}

void DispatchQueue::init_filter()
{
    const string mac(utils::mac_to_str(get_device().get_mac()));

    stringstream filter;
    filter << "arp and not (ether src host " << mac << ")";
    apply_filter(filter.str());
}

void DispatchQueue::operator()()
{
    run();
}

void DispatchQueue::handle_packet(const cyanid::raw_packet& pkt)
{
    const size_t pkt_size = pkt.packet_header()->len;
    const size_t arp_size = pkt_size - builder::ethernet::header_size;

    if (arp_size < builder::arp::header_size)
        return;

    builder::ethernet eth(pkt.payload(), pkt_size);
    builder::arp      arp(eth.payload(), arp_size);

    if (arp.oper() == builder::arp::REPLY) {
        cout << "** Got an ARP reply from " << arp.spa() << ": "
            << arp.sha() << endl;

        arp_cache[arp.spa()] = arp.sha();

        cout << "Trying to dispatch packets. Packets in queue: " << queue.size() << endl;
        std::list<raw_packet> packets;
        while (!queue.empty()) {
            packets.push_back(queue.front());
            queue.pop_front();
        }

        cout << " ** -- Dispatching packages..." << endl;
        for (auto it = packets.begin(); it != packets.end(); ++it) {
            cout << " ** -- --Trying to dispatch packet..." << endl;
            dispatch(*it);
        }
    }
}

void DispatchQueue::dispatch(const raw_packet& pkt)
{
    static const string prefix = "192.168";

    const size_t pkt_size = pkt.packet_header()->len;
    const size_t ip_size  = pkt_size - builder::ip::header_size;

    builder::ethernet eth(pkt.payload(), pkt_size);
    builder::ip        ip(eth.payload(), ip_size);

    string target_ip = utils::addr4_to_str(ip.dst_ip());

    string tpa;
    if (target_ip.substr(0, prefix.size()) == prefix) {
        tpa = target_ip;
    } else {
        tpa = gateway_ip;
    }

    cout << "Sending packet to " << tpa;
    if (arp_cache.find(tpa) != arp_cache.end()) {
        cout << ". Target HW addr is in cache" << endl;
        packet new_pkt = build_packet(eth, pkt_size - builder::ethernet::header_size, tpa);
        new_pkt.dispatch();
    } else {
        cout << ". Target HW addr is NOT in cache... ";
        queue.push_back(pkt);
        cout << "Adding packet to queue. New queue size: " << queue.size() << endl;
        send_arp_request(tpa);
    }
}

void DispatchQueue::send_arp_request(const string& target_ip)
{
    // TODO: Load this from a config file
    static const string prefix = "192.168";

    string tpa;
    if (target_ip.substr(0, prefix.size()) == prefix) {
        tpa = target_ip;
    } else {
        tpa = gateway_ip;
    }

    const cyanid::device& dev(get_device());

    const string source_mac(cyanid::utils::mac_to_str(dev.get_mac()));
    const string source_ip(cyanid::utils::addr4_to_str(dev.get_ip()));

    packet pkt(get_device());

    pkt.build<cyanid::builder::arp>()(
            cyanid::builder::arp::REQUEST,
            source_mac,
            source_ip,
            "00:00:00:00:00:00",
            tpa);

    pkt.build<cyanid::builder::ethernet>()(
            "ff:ff:ff:ff:ff:ff",
            cyanid::builder::arp::ETHER_TYPE);

    pkt.dispatch();
}

packet DispatchQueue::build_packet(builder::ethernet& eth, size_t pkt_size,
        const string& target_ip)
{
    const device& dev(get_device());

    packet new_pkt(dev);

    new_pkt.build<builder::ethernet>()(
            eth.source_mac(),
            arp_cache[target_ip],
            eth.ether_type(),
            pkt_size,
            eth.payload());

    return new_pkt;
}

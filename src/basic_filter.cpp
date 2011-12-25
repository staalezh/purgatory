#include "basic_filter.hpp"

#include <cyanid.hpp>

#include <list>
#include <string>
#include <algorithm>
#include <iostream>

using namespace std;
using namespace cyanid;

BasicFilter::BasicFilter(const list<string>& hosts) : hosts(hosts)
{ 
}

bool BasicFilter::validate(const cyanid::raw_packet& pkt)
{
    /*
    const size_t pkt_size = pkt.packet_header()->len;
    const size_t ip_size = pkt_size - builder::ethernet::header_size;

    builder::ethernet eth(pkt.payload(), pkt_size);
    builder::ip        ip(eth.payload(), ip_size);

    string src_ip(utils::addr4_to_str(ip.src_ip()));
    string dst_ip(utils::addr4_to_str(ip.dst_ip()));

    if (find(hosts.begin(), hosts.end(), src_ip) == hosts.end() &&
            find(hosts.begin(), hosts.end(), dst_ip) == hosts.end())
        return false;

    cout << "Routing packet from " << src_ip << " to " << dst_ip << endl;
    */

    return true;
}

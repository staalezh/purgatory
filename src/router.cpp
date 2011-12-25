#include "router.hpp"

#include <cyanid.hpp>

#include <iostream>
#include <sstream>

using namespace std;
using namespace cyanid;

Router::Router(device& dev, Filter& filter, DispatchQueue& dq)
: listener(dev), filter(filter), dispatch_queue(dq)
{
    init_filter();
}

Router::Router(const Router&& r)
: listener(r.get_device()), filter(r.filter), dispatch_queue(r.dispatch_queue)
{
    init_filter();
}

void Router::handle_packet(const raw_packet& pkt)
{
    if (filter.validate(pkt)) {
        dispatch_queue.dispatch(pkt);
    }
}

void Router::operator()()
{
    run();
}

void Router::init_filter()
{
    string ip(utils::addr4_to_str(get_device().get_ip()));
    string mac(utils::mac_to_str(get_device().get_mac()));

    stringstream ss;
    ss << "tcp and not (dst net " << ip << ") and not (src net "
       << ip << ")" << " and not (ether src host " << mac << ")";

    apply_filter(ss.str());
}

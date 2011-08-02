#include "router.hpp"

#include <cyanid.hpp>

#include <iostream>
#include <sstream>

using namespace std;
using namespace cyanid;

Router::Router(device& dev, Filter& filter) : listener(dev), filter(filter)
{
    init_filter();
}

Router::Router(const Router&& r) : listener(r.get_device()), filter(r.filter)
{
    init_filter();
}

void Router::handle_packet(const raw_packet& pkt)
{
    if (filter.validate(pkt)) {
        cout << "Routing packet... ";
        size_t bytes_written = get_device().dispatch_raw(pkt);
        cout << "Done. Wrote " << bytes_written << " bytes to the network\n";
    }
}

void Router::operator()()
{
    run();
}

void Router::init_filter()
{
    string ip(utils::addr4_to_str(get_device().get_ip()));

    stringstream ss;
    ss << "ip and not (net " << ip << ")";
    apply_filter(ss.str());
}

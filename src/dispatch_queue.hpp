#ifndef _dispatch_queue_hpp_
#define _dispatch_queue_hpp_

#include <cyanid.hpp>

#include <thread>
#include <list>
#include <map>

class DispatchQueue : public cyanid::listener {
public:
    DispatchQueue(cyanid::device&, const std::string&);

    void handle_packet(const cyanid::raw_packet&);
    void operator()();

    void dispatch(const cyanid::raw_packet&);

private:
    void operator=(const DispatchQueue&);

    void init_filter();
    void send_arp_request(const std::string&);
    cyanid::packet build_packet(cyanid::builder::ethernet&, size_t,
            const std::string&);

    std::string gateway_ip;
    std::map<std::string, std::string> arp_cache;
    std::list<cyanid::raw_packet> queue;
};

#endif // _dispatch_queue_hpp_

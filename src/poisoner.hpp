#ifndef _poisoner_hpp_
#define _poisoner_hpp_

#include <cyanid.hpp>

#include <vector>
#include <string>

class Poisoner : public cyanid::listener {
public:
    Poisoner(cyanid::device& device,
            const std::vector<std::string>& hosts,
            const std::string& gateway_ip,
            const std::string& gateway_mac);

    Poisoner(Poisoner&&);

    void handle_packet(const cyanid::raw_packet&);
    void operator()();

private:
    void operator=(const Poisoner&);

    void poison_target(const std::string& spa,
            const std::string& tpa,
            const std::string& tha);

    void dump_arp_packet(const cyanid::builder::arp&);
    void init(const cyanid::device&);
    bool protect_host(const std::string&);

    const std::vector<std::string> hosts;
    const std::string gateway_ip;
    const std::string gateway_mac;

    std::string mac_addr;
    std::string ip_addr;
};

#endif // _poisoner_hpp_

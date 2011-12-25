#ifndef _dumb_poisoner_hpp_
#define _dumb_poisoner_hpp_

class DumbPoisoner {
public:
    DumbPoisoner(const cyanid::device&,
            const std::vector<std::string>& hosts,
            const std::string& gateway_ip,
            const std::string& gateway_mac);

    DumbPoisoner(Poisoner&&);

    void operator()();

    const std::vector<std::string> hosts;
    const std::string gateway_ip;
    const std::string gateway_mac;
};

#endif

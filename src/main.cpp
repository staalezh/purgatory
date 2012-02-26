#include "basic_filter.hpp"
#include "poisoner.hpp"
#include "router.hpp"

#include <cyanid.hpp>

#include <algorithm>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <string>
#include <thread>

using namespace std;
using namespace cyanid;

void trigger_poisoning(cyanid::device&, const list<string>&);
void read_hosts(list<string>&, const string&);
void dump_hosts(const list<string>&);
map<string, string> read_config(const string&);

int main(int argc, char* argv[])
{
    using namespace std;
    using namespace cyanid;

    map<string, string> config(read_config("purgatory.conf"));

    const string iface(config["interface"]);
    const string gateway_ip(config["gateway-ip"]);
    const string gateway_mac(config["gateway-mac"]);

    list<string> hosts;
    read_hosts(hosts, config["hosts-file"]);

    cout << "Starting ARP poisoning and routing on " << iface << "... ";

    device dev(iface);

    BasicFilter filter(hosts);
    DispatchQueue dq(dev, gateway_ip);

    thread dispatch_queue([&dq]() { dq.run(); });
    thread router(Router(dev, filter, dq));
    thread poisoner(Poisoner(dev, hosts, gateway_ip, gateway_mac));

    cout << "Done. Firewalling hosts: " << endl;

    dump_hosts(hosts);

    cout << "Requesting MAC addresses... \n" << endl;

    trigger_poisoning(dev, hosts);

    dispatch_queue.join();
    router.join();
    poisoner.join();

    return 0;
}

void trigger_poisoning(cyanid::device& dev, const list<string>& hosts)
{
    const std::string source_mac(cyanid::utils::mac_to_str(dev.get_mac()));
    const std::string source_ip(cyanid::utils::addr4_to_str(dev.get_ip()));

    for (auto it = hosts.begin(); it != hosts.end(); ++it) {
        const string target_ip(*it);

        cyanid::packet packet(dev);

        packet.build<cyanid::builder::arp>()(
                cyanid::builder::arp::REQUEST,
                source_mac,
                source_ip,
                "00:00:00:00:00:00",
                target_ip);

        packet.build<cyanid::builder::ethernet>()(
                "ff:ff:ff:ff:ff:ff",
                cyanid::builder::arp::ETHER_TYPE);

        packet.dispatch();

        cout << "Requested MAC address for " << *it << "\n\n";
    }
}

void read_hosts(list<string>& hosts, const string& file)
{
    string line;

    fstream infile(file);
    while (getline(infile, line)) {
        if (line[0] == '#') continue;
        hosts.push_back(line);
    }
}

void dump_hosts(const list<string>& hosts)
{
    for (auto it = hosts.begin(); it != hosts.end(); ++it) {
        cout << "<*> " << *it << endl;
    }

    cout << endl;
}

map<string, string> read_config(const string& file)
{
    ifstream is(file);

    map<string, string> config;
    string line;
    while (getline(is, line)) {
        auto it = find(line.begin(), line.end(), ':');
        string key(line.begin(), it);
        string value(it + 1, line.end());
        config[key] = value;

        cerr << key << ":" << value << endl;
    }

    return config;
}

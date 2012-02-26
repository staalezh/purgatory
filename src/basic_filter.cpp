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
    return true;
}

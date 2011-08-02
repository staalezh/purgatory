#ifndef _router_hpp_
#define _router_hpp_

#include "filter.hpp"
#include <cyanid.hpp>

class Router : public cyanid::listener {
public:
    Router(cyanid::device&, Filter&);
    Router(const Router&&);

    void handle_packet(const cyanid::raw_packet&);
    void operator()();

private:
    void operator=(const Router&);

    void init_filter();

    Filter& filter;
};

#endif

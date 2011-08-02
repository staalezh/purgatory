#ifndef _filter_hpp_
#define _filter_hpp_

namespace cyanid { class raw_packet; }

class Filter {
public:
    virtual bool validate(const cyanid::raw_packet&) = 0;
};

#endif // _filter_hpp_

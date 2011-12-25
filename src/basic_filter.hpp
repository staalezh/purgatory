#ifndef _basic_filter_hpp_
#define _basic_filter_hpp_

#include "filter.hpp"
#include <list>
#include <string>

namespace cyanid { class raw_packet; }

class BasicFilter : public Filter {
public:
    BasicFilter(const std::list<std::string>&);

    bool validate(const cyanid::raw_packet&);
private:
    const std::list<std::string>& hosts;
};

#endif // _basic_filter_hpp_

#include "EventFlags.hpp"



etl::EventGroup< MemberType > EventFlags_t::operator[](uint32_t idx){
    if ( idx  > commandsNum ) return etl::nullopt ;

    return pEventGropu.*(memberMap[idx]);
};
#include "etl/optional.h"
#include "etl/array.h"

template < 
    class T ,
    class N, 
> 
class EventGroup{
private:
    GroupType  &eventGroup_t  ;
    const etl::array< MembersType GroupType::* , 32 > memberMap; 

public:
    using GroupType  = T ; 
    using MembersType = N ;
    //etl::array< MemberType T::* , 32 > mm
    static etl::optional< EventGroup > create(T &p_eventGroup ,  uint32_t bits ){
        return etl::optional< EventGroup >{ etl::in_place }  ;
    }

    etl::optional< MembersType > operator[](uint32_t idx){
        if ( idx  > commandsNum ) return etl::nullopt ;

        return pEventGropu.*(memberMap[idx]);
    };

    bool bitsWait(   uint32_t message , uint32_t delay) ; 
    
};


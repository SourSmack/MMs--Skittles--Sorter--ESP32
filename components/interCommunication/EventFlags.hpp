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
    
    static etl::optional< EventGroup > create(T &p_eventGroup ,  uint32_t bits ){
        return etl::optional< EventGroup >{ etl::in_place }  ;
    }

    EventGroup( T &p_eventGroup ,  uint32_t bits ): eventGroup_t(p_eventGroup) ,  {

    }

    etl::optional< MembersType > operator[](uint32_t idx){
        if ( idx  > commandsNum ) return etl::nullopt ;

        return pEventGropu.*(memberMap[idx]);
    };

    bool waitBits(   uint32_t message , uint32_t delay) {

    } 


    static bool setBits( uint32_t message ){

    }
    
};


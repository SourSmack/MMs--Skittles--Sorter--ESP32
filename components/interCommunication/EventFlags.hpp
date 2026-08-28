

template < 
    class T ,
    class MemberType, 
    uint16_t commandsNum 
> 
struct EventFlags_t{
    T  &pEventGroup  ;
    const etl::array< MemberType T::* , commandsNum> memberMap; 


    EventFlags_t( T & eventGroup, etl::array< MemberType T::* , commandsNum> mm ): pEventGroup( eventGroup ) , memberMap( mm ) {}
    
    etl::optional< MemberType > operator[](uint32_t idx);
};

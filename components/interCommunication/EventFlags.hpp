

template < 
    class T ,
    class MemberType, 
> 
class EventGroup{
private:
    T  &eventGroup_t  ;
    const etl::array< MemberType T::* , 32 > memberMap; 

public:

    //etl::array< MemberType T::* , 32 > mm
    static etl::optional< EventGroup > create(T &p_eventGroup ,  uint32_t bits );

    etl::optional< MemberType > operator[](uint32_t idx);

    bool bitsWait(   uint32_t message , uint32_t delay) ; 
    
};

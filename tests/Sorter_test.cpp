#include "../main/Sorter.hpp"
#include "../main/ConceptsConfig.hpp"
#include "/home/sourish/c++_libs/googletest/googlemock/include/gmock"

#include "/home/sourish/c++_libs/googletest/googletest/include/gtest"


template < class GroupType  , class MemberType > 
class  EventFlagsMOCK {
public:
    etl::optional< EventFlagsMOCK > create( GroupType & p_eventGroup , uint32_t bits){ return EventFlagsMOCK{} ;}


    
    
    MOCK_METHOD( etl::optional<  MemberType > , operator[] , ( uint32_t idx ) ) ;
    MOCK_METHOD( bool , bitsWait , ( uint32_t message, uint32_t delay));
 
};
struct DummyGroup{} ;
struct DummyMember{} ;
static_assert( EventFlagsConcept < EventFlagsMock< DummyGroup , DummyMember > > ) ;

class TaskMOCK {

};

class SlideEngMOCK {

};

class SlideSensorMOCK {

};
class DisksEngMOCK {

};
class DisksSensorMOCK {

};


int main(){
    


}
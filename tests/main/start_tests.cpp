#include "gmock/gmock.h"
#include "gtest/gtest.h"


extern "C" void app_main(){

    printf("Uruchamianie testow jednostkowych na systemie Linux...\n");
    
    // Inicjalizacja GMock/GTest (tworzymy sztuczne argumenty argc/argv)
    int argc = 1;
    char* argv[] = {(char*)"test_runner", nullptr};
    ::testing::InitGoogleMock(&argc, argv);
    
    // Uruchomienie wszystkich testów
    int result = RUN_ALL_TESTS();
    
    // Zakończenie programu (ważne na targecie Linux, by zwrócić kod błędu do terminala)
    exit(result);

}
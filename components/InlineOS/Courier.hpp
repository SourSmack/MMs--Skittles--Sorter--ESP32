#pragma once

#include <cstdint>
#include "Mission.hpp"

template<
    class Self,
    bool (*TransportData)(Self&),
    bool (*DestinationAvailable)(Self&) = nullptr,
    bool (*SourceAvailable)(Self&) = nullptr
>
class Courier {
public:
    enum : uint32_t {
        sourceExhaustion      = 1 << 1,
        destinationExhaustion = 1 << 2,
        transferExhaustion    = 1 << 3
    };

private:
    Mission &task;

    static void transfer(void * context);

public:
    explicit Courier(Mission &p_task);

    bool pauseTransfer();
    bool stopTransfer();
    bool startTransfer();

    uint32_t verifyStatus(uint32_t status = 0);
};


#include "Courier.cpp"

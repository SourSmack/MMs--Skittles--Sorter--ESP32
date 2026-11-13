#include "Courier.hpp"

template<
    class Self,
    bool (*TransportData)(Self&),
    bool (*DestinationAvailable)(Self&),
    bool (*SourceAvailable)(Self&)
>
Courier<Self, TransportData, DestinationAvailable, SourceAvailable>::Courier(Mission &p_task)
    : task(p_task) {}

template<
    class Self,
    bool (*TransportData)(Self&),
    bool (*DestinationAvailable)(Self&),
    bool (*SourceAvailable)(Self&)
>
void Courier<Self, TransportData, DestinationAvailable, SourceAvailable>::transfer(void * context) {
    auto& self = *static_cast<Self*>(context);

    while (!Mission::waitForNotify(Mission::event.stop, 0)) {
        if (Mission::waitForNotify(Mission::event.pause, 0)) {
            while (!Mission::waitForNotify(Mission::event.resume, Mission::MAX_DELAY)) {}
        }

        if constexpr (SourceAvailable != nullptr) {
            while (!SourceAvailable(self)) {
                Mission::notify(sourceExhaustion);
                while (!Mission::waitForNotify(Mission::event.resume, Mission::MAX_DELAY)) {}
            }
        }

        if constexpr (DestinationAvailable != nullptr) {
            while (!DestinationAvailable(self)) {
                Mission::notify(destinationExhaustion);
                while (!Mission::waitForNotify(Mission::event.resume, Mission::MAX_DELAY)) {}
            }
        }

        if constexpr (TransportData != nullptr) {
            if (!TransportData(self)) {
                Mission::notify(transferExhaustion);
                while (!Mission::waitForNotify(Mission::event.resume, Mission::MAX_DELAY)) {}
            }
        }
    }
}

template<
    class Self,
    bool (*TransportData)(Self&),
    bool (*DestinationAvailable)(Self&),
    bool (*SourceAvailable)(Self&)
>
bool Courier<Self, TransportData, DestinationAvailable, SourceAvailable>::pauseTransfer() {
    return false;
}

template<
    class Self,
    bool (*TransportData)(Self&),
    bool (*DestinationAvailable)(Self&),
    bool (*SourceAvailable)(Self&)
>
bool Courier<Self, TransportData, DestinationAvailable, SourceAvailable>::stopTransfer() {
    return false;
}

template<
    class Self,
    bool (*TransportData)(Self&),
    bool (*DestinationAvailable)(Self&),
    bool (*SourceAvailable)(Self&)
>
bool Courier<Self, TransportData, DestinationAvailable, SourceAvailable>::startTransfer() {
    return false;
}

template<
    class Self,
    bool (*TransportData)(Self&),
    bool (*DestinationAvailable)(Self&),
    bool (*SourceAvailable)(Self&)
>
uint32_t Courier<Self, TransportData, DestinationAvailable, SourceAvailable>::verifyStatus(uint32_t status) {
    if (status) {
        return Mission::waitForNotify(status, 0);
    }
    return Mission::checkOnFlags();
}

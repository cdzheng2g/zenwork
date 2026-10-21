#pragma once

#include <QtGlobal>

namespace ModbusRegisters {
constexpr int Trigger = 0x0100;
constexpr int ResultStart = 0x0300;
constexpr int ResultCount = 12;
enum ResultOffset {
    State,
    Verdict,
    SequenceLow,
    SequenceHigh,
    ElapsedLow,
    ElapsedHigh,
    CompletedLow,
    CompletedHigh,
    RejectedLow,
    RejectedHigh,
    TriggerReply,
    Version
};
enum ExecutionState : quint16 { NotReady = 0, Ready = 1, Busy = 2 };
enum TriggerResponse : quint16 { None = 0, Accepted = 1, BusyRejected = 2, NotReadyRejected = 3 };
}

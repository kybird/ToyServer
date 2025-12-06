#pragma once
#include <vector>
#include <cstdint>

namespace GameServer::Network {

/*
    [RW][Data][...][...]
    [..][RW][Data][...]
*/

class RecvBuffer {
public:
    RecvBuffer(size_t bufferSize);
    ~RecvBuffer();

    void Clean();
    bool OnRead(size_t numOfBytes);
    bool OnWrite(size_t numOfBytes);

    uint8_t* ReadPos() { return &_buffer[_readPos]; }
    uint8_t* WritePos() { return &_buffer[_writePos]; }
    size_t DataSize() { return _writePos - _readPos; }
    size_t FreeSize() { return _capacity - _writePos; }

private:
    size_t _capacity = 0;
    size_t _bufferSize = 0;
    size_t _readPos = 0;
    size_t _writePos = 0;
    std::vector<uint8_t> _buffer;
};

}

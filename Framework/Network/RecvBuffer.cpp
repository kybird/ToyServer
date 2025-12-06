#include "Network/RecvBuffer.h"
#include <cstring>
#include <algorithm>

namespace GameServer::Network {

RecvBuffer::RecvBuffer(size_t bufferSize) : _bufferSize(bufferSize) {
    _capacity = bufferSize * 10;
    _buffer.resize(_capacity);
}

RecvBuffer::~RecvBuffer() {
}

void RecvBuffer::Clean() {
    size_t dataSize = DataSize();

    if (dataSize == 0) {
        // 남은 데이터가 없으면 커서 초기화
        _readPos = _writePos = 0;
    } else {
        // 여유 공간이 버퍼 크기보다 작으면 데이터를 앞으로 당김
        if (FreeSize() < _bufferSize) {
            std::copy(_buffer.begin() + _readPos, _buffer.begin() + _writePos, _buffer.begin());
            _readPos = 0;
            _writePos = dataSize;
        }
    }
}

bool RecvBuffer::OnRead(size_t numOfBytes) {
    if (numOfBytes > DataSize())
        return false;

    _readPos += numOfBytes;
    return true;
}

bool RecvBuffer::OnWrite(size_t numOfBytes) {
    if (numOfBytes > FreeSize())
        return false;

    _writePos += numOfBytes;
    return true;
}

}

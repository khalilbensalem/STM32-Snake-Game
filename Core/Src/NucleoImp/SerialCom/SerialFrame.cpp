/**
 * @file SerialFrame.cpp
 * @date June 2025
 * @brief The file implements the SerialFrame class that represents a serial data frame.
 *
 * Ajout de getSnakePositionMessage() pour PolySnake.
 * 
 * @defgroup ELE3312
 * @{
 *
 * @authors
 * - Inès Tazi Riffi (2216169)
 * - Zakaria Chikri  (2216665)
 */
#include "NucleoImp/SerialCom/SerialFrame.h"
#include "NucleoImp/SerialCom/COBS.h"

using namespace ELE3312;

/**
 * @brief Structure that describes the header of the data frame.
 */
typedef struct {
    uint16_t size;
    uint16_t type;
    uint32_t crc;
} uart_header_t;

/**
 * @brief Default constructor for the SerialFrame class.
 */
SerialFrame::SerialFrame() : msg{nullptr} {
}

/**
 * @brief Constructor for the SerialFrame class.
 * @param [in] msg A shared pointer to a Message object to encapsulate into the data frame.
 */
SerialFrame::SerialFrame(Message *msg): msg{msg} {
}

/**
 * @brief Setup routine for the SerialFrame class
 */
void SerialFrame::setup(uint8_t *p_encodeBuffer, size_t encodeSize, 
                        uint8_t *p_tempBuffer, size_t outputSize) {
    encodeBuffer = p_encodeBuffer;
    tempBuffer = p_tempBuffer;
    encodeBufferSize = encodeSize;
    tempBufferSize = outputSize;
}

// Getters

/**
 * @brief Returns an array of bytes with the data contained in the frame.
 */
std::tuple<uint8_t*, size_t> SerialFrame::getByteArray() {
    return msgToByteArray(msg);
}

/**
 * @brief Returns the type of the message.
 */
MessageType SerialFrame::getMessageType() {
    return messageType;
}

// Setters

/**
 * @brief Sets the the message contained in the data frame.
 */
void SerialFrame::setMessage(uint8_t* byteArray, size_t arraySize) {
    msgBuffer = byteArray;
    msgSize = *(uint16_t*)msgBuffer;
    if (msgSize > arraySize) {
        msgSize = 0;
        msgBuffer = nullptr;
        messageType = MessageType::Unknown;
        return;
    }
    messageType = (MessageType)(*((uint16_t*)(msgBuffer + 2)));
}

/**
 * @brief Returns a SnakePositionMessage.
 * @return Interpretation of the internal buffer as a SnakePositionMessage.
 */
SnakePositionMessage SerialFrame::getSnakePositionMessage() {
    SnakePositionMessage msg;
    
    if (messageType == MessageType::Position) {
        // Le payload commence après le header (8 octets)
        msg.setData(msgBuffer + sizeof(uart_header_t), msgSize);
        msg.setValid(true);
    }
    
    return msg;
}

// Private Helper Methods

/**
 * @brief The function serializes the specified message to an array of bytes.
 */
std::tuple<uint8_t*, size_t> SerialFrame::msgToByteArray(Message *message) {
    if (message->getSize() > tempBufferSize - sizeof(uart_header_t)) {
        return {nullptr, 0};
    }
    
    uart_header_t header = {0, 0, 0};
    header.size = message->getSize();
    header.type = (uint16_t) message->getType();
    header.crc  = 0; // TODO: implement CRC via hardware
    
    size_t idx = 0;
    uint8_t *pHeader = (uint8_t *) &header;
    
    // Copy header into tempBuffer
    for (; idx < sizeof(uart_header_t) && idx < tempBufferSize; ++idx) {
        tempBuffer[idx] = *pHeader;
        pHeader += 1;
    }
    
    const uint8_t *pMessage = message->getData();
    for (size_t j = 0; j < message->getSize(); ++j) {
        tempBuffer[idx] = pMessage[j];
        idx += 1;
    }

    size_t len = cobsEncode(tempBuffer, idx, encodeBuffer);
    
    return {encodeBuffer, len};
}

/**
 * @}
 */ // End of documentation group ELE3312

/*
   Copyright 2021 Scott Bezek and the splitflap contributors

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/
#include "../core/common.h"

#include "../proto_gen/splitflap.pb.h"

#include "crc32.h"

#include "pb_encode.h"
#include "serial_proto_protocol.h"

static SerialProtoProtocol* singleton_for_packet_serial = 0;


SerialProtoProtocol::SerialProtoProtocol(SplitflapTask& splitflap_task) : SerialProtocol(splitflap_task) {
    packet_serial_.setStream(&Serial);

    // Note: not threadsafe or instance safe!! but PacketSerial requires a legacy function pointer, so we can't
    // use a member, std::function, or lambda with captures
    assert(singleton_for_packet_serial == 0);
    singleton_for_packet_serial = this;

    packet_serial_.setPacketHandler([](const uint8_t* buffer, size_t size) {
         singleton_for_packet_serial->handlePacket(buffer, size);
    });
}

void SerialProtoProtocol::handleState(const SplitflapState& old_state, const SplitflapState& new_state) {
    pb_tx_buffer_ = {};
    pb_tx_buffer_.which_payload = PB_FromSplitflap_splitflap_state_tag;
    pb_tx_buffer_.payload.splitflap_state.modules_count = NUM_MODULES;
    for (uint8_t i = 0; i < NUM_MODULES; i++) {
        pb_tx_buffer_.payload.splitflap_state.modules[i] = {
            .state = (PB_SplitflapState_ModuleState_State) new_state.modules[i].state,
            .flap_index = new_state.modules[i].flap_index,
            .moving = new_state.modules[i].moving,
            .home_state = new_state.modules[i].home_state,
            .count_unexpected_home = new_state.modules[i].count_unexpected_home,
            .count_missed_home = new_state.modules[i].count_missed_home,
        };
    }

    sendPbTxBuffer();
}

void SerialProtoProtocol::handleRx(int b) {
}

void SerialProtoProtocol::log(const char* msg) {
    pb_tx_buffer_ = {};
    pb_tx_buffer_.which_payload = PB_FromSplitflap_log_tag;

    strncpy(pb_tx_buffer_.payload.log.msg, msg, sizeof(pb_tx_buffer_.payload.log.msg));

    sendPbTxBuffer();
}

void SerialProtoProtocol::loop() {
    packet_serial_.update();
}

void SerialProtoProtocol::handlePacket(const uint8_t* buffer, size_t size) {
    // TODO: Check CRC
    // TODO: decode proto
}

void SerialProtoProtocol::sendPbTxBuffer() {
    // Encode protobuf message to byte buffer
    pb_ostream_t stream = pb_ostream_from_buffer(tx_buffer_, sizeof(tx_buffer_));
    if (!pb_encode(&stream, PB_FromSplitflap_fields, &pb_tx_buffer_)) {
        Serial.println(stream.errmsg);
        Serial.flush();
        assert(false);
    }

    // Compute and append little-endian CRC32
    uint32_t crc = 0;
    crc32(tx_buffer_, stream.bytes_written, &crc);
    tx_buffer_[stream.bytes_written + 0] = (crc >> 0)  & 0xFF;
    tx_buffer_[stream.bytes_written + 1] = (crc >> 8)  & 0xFF;
    tx_buffer_[stream.bytes_written + 2] = (crc >> 16) & 0xFF;
    tx_buffer_[stream.bytes_written + 3] = (crc >> 24) & 0xFF;

    // Encode and send proto+CRC as a COBS packet
    packet_serial_.send(tx_buffer_, stream.bytes_written + 4);
}

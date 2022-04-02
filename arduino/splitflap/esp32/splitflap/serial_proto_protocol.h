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
#pragma once

#include "PacketSerial.h"

#include "serial_protocol.h"
#include "../proto_gen/splitflap.pb.h"

class SerialProtoProtocol : public SerialProtocol {
    public:
        SerialProtoProtocol(SplitflapTask& splitflap_task, Stream& stream);
        ~SerialProtoProtocol() {}
        void log(const char* msg) override;
        void loop() override;
        void handleState(const SplitflapState& old_state, const SplitflapState& new_state) override;
        void sendSupervisorState(PB_SupervisorState& supervisor_state) override;

        void init();
    
    private:
        Stream& stream_;
        PB_FromSplitflap pb_tx_buffer_;
        PB_ToSplitflap pb_rx_buffer_;

        uint8_t tx_buffer_[PB_FromSplitflap_size + 4]; // Max message size + CRC32

        PacketSerial_<COBS, 0, (PB_ToSplitflap_size + 4) * 2 + 10> packet_serial_;

        uint32_t last_nonce_;

        SplitflapState latest_state_ = {};
        SplitflapState last_sent_state_ = {};
        uint32_t last_sent_state_millis_ = 0;

        bool state_requested_;

        void sendPbTxBuffer();
        void handlePacket(const uint8_t* buffer, size_t size);
        void ack(uint32_t nonce);
};

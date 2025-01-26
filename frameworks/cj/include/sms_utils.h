/*
 * Copyright (c) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef SMS_UTILS_H
#define SMS_UTILS_H

#include <cstdint>

#include "cj_common_ffi.h"

namespace OHOS {
namespace Telephony {
const int32_t MESSAGE_PARAMETER_NOT_MATCH = 0;
const int32_t TEXT_MESSAGE_PARAMETER_MATCH = 1;
const int32_t RAW_DATA_MESSAGE_PARAMETER_MATCH = 2;
extern "C" {
struct CJShortMessage {
    bool hasReplyPath;
    bool isReplaceMessage;
    bool isSmsStatusReportMessage;
    int32_t messageClass;
    CArrI32 pdu;
    int32_t protocolId;
    char* scAddress;
    int64_t scTimestamp;
    int32_t status;
    char* visibleMessageBody;
    char* visibleRawAddress;
    int32_t errCode;
};

struct CJSendMessageOptions {
    int32_t messageType = MESSAGE_PARAMETER_NOT_MATCH;
    bool isPersist = true;
    int32_t slotId;
    char* destinationHost;
    char* serviceCenter;
    uint16_t destinationPort = 0;
    int64_t sendCallback;
    int64_t deliveryCallback;
    char* textContent;
    CArrUI8 rawDataContent;
};
}
char* MallocCString(const std::string& origin);
int32_t NativeSendMessage(CJSendMessageOptions& options);
int32_t ConverErrorCodeForCj(int32_t errorCode);
int32_t ConverErrorCodeWithPermissionForCj(int32_t errorCode);
} // namespace Telephony
} // namespace OHOS

#endif

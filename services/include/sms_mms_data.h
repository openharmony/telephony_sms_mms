/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef SMS_MMS_DATA_H
#define SMS_MMS_DATA_H

namespace OHOS {
namespace Telephony {
class SmsMmsData {
public:
    static constexpr const char *ID = "id";
    static constexpr const char *SLOT_ID = "slot_id";
    static constexpr const char *SMS_SUBSECTION_ID = "sms_subsection_id";
    static constexpr const char *RECEIVER_NUMBER = "receiver_number";
    static constexpr const char *SENDER_NUMBER = "sender_number";
    static constexpr const char *IS_SENDER = "is_sender";
    static constexpr const char *START_TIME = "start_time";
    static constexpr const char *END_TIME = "end_time";
    static constexpr const char *RAW_PUD = "raw_pdu";
    static constexpr const char *FORMAT = "format";
    static constexpr const char *DEST_PORT = "dest_port";
    static constexpr const char *SUBSECTION_INDEX = "subsection_index";
    static constexpr const char *SIZE = "size";
};
} // namespace Telephony
} // namespace OHOS
#endif // SMS_MMS_DATA_H


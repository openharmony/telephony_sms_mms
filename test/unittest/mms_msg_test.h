/*
 * Copyright (C) 2021 Huawei Device Co., Ltd.
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
#ifndef MMS_MSG_TEST_H
#define MMS_MSG_TEST_H

#include "mms_msg.h"

namespace OHOS {
namespace Telephony {
class MmsMsgTest {
public:
    MmsMsgTest();
    ~MmsMsgTest();
    void ProcessTest() const;
    void ProcessDecodeInput(int inputCMD) const;
    void ProcessEncodeInput(int inputCMD) const;
    void MmsDecodeTest(std::string strPath) const;
    void MmsSendReqEncodeTest() const;
    void MmsSendConfEncodeTest() const;
    void MmsNotificationIndEncodeTest() const;
    void MmsNotifyRespIndEncodeTest() const;
    void MmsRetrieveConfEncodeTest() const;
    void MmsAcknowledgeIndEncodeTest() const;
    void MmsDeliveryIndEncodeTest() const;
    void MmsReadRecIndEncodeTest() const;
    void MmsReadOrigIndEncodeTest() const;

private:
    void MmsSendReqDecodeTest(MmsMsg &decodeMsg) const;
    void MmsSendConfDecodeTest(MmsMsg &decodeMsg) const;
    void MmsNotificationIndDecodeTest(MmsMsg &decodeMsg) const;
    void MmsNotifyRespIndDecodeTest(MmsMsg &decodeMsg) const;
    void MmsRetrieveConfDecodeTest(MmsMsg &decodeMsg) const;
    void MmsAcknowledgeIndDecodeTest(MmsMsg &decodeMsg) const;
    void MmsDeliveryIndDecodeTest(MmsMsg &decodeMsg) const;
    void MmsReadRecIndDecodeTest(MmsMsg &decodeMsg) const;
    void MmsReadOrigIndDecodeTest(MmsMsg &decodeMsg) const;
    bool MmsAddAttachment(
        MmsMsg &msg, std::string pathName, std::string contentId, std::string contenType, bool isSmil) const;
    std::string BuffToHex(const std::unique_ptr<char[]> &buff, uint32_t len) const;
    bool WriteBufferToFile(const std::unique_ptr<char[]> &buff, uint32_t len, const std::string &strPathName) const;
};
} // namespace Telephony
} // namespace OHOS
#endif

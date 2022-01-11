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

#ifndef MMS_HEADER_H
#define MMS_HEADER_H

#include <map>
#include <vector>

#include "mms_header_categ.h"
#include "mms_content_type.h"
#include "mms_decode_buffer.h"
#include "mms_encode_string.h"
#include "mms_address.h"

namespace OHOS {
namespace Telephony {
class MmsHeader {
public:
    MmsHeader();
    void DumpMmsHeader();
    bool DecodeMmsHeader(MmsDecodeBuffer &decodeBuffer);
    bool EncodeMmsHeader(MmsEncodeBuffer &encodeBuffer);
    bool GetOctetValue(uint8_t fieldId, uint8_t &value) const;
    bool SetOctetValue(uint8_t fieldId, uint8_t value);
    bool GetLongValue(uint8_t fieldId, int64_t &value) const;
    bool SetLongValue(uint8_t fieldId, int64_t value);
    bool GetTextValue(uint8_t fieldId, std::string &value) const;
    bool SetTextValue(uint8_t fieldId, std::string value);
    bool GetEncodeStringValue(uint8_t fieldId, MmsEncodeString &value) const;
    bool SetEncodeStringValue(uint8_t fieldId, uint32_t charset, const std::string &value);
    bool AddHeaderAddressValue(uint8_t fieldId, MmsAddress &address);
    bool GetHeaderAllAddressValue(uint8_t fieldId, std::vector<MmsAddress> &addressValue);
    bool GetStringValue(uint8_t fieldId, std::string &value) const;
    MmsContentType &GetHeaderContentType();
    bool GetSmilFileName(std::string &smileFileName);
    bool IsHaveBody();
private:
    void InitOctetHandleFun();
    void InitLongHandleFun();
    void InitStringHandleFun();
    void InitTextStringHandleFun();
    std::string MakeTransactionId(uint32_t len);
    bool FindHeaderFieldName(const uint8_t fieldId, std::string &fieldName);

    bool DecodeMmsMsgType(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldAddressModelValue(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldOctetValue(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldLongValue(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldIntegerValue(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldTextStringValue(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldEncodedStringValue(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFromValue(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeMmsMsgUnKnownField(MmsDecodeBuffer &decodeBuffer);
    bool DecodeMmsContentType(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldDate(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldPreviouslySentDate(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldMBox(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    bool DecodeFieldMMFlag(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);

    // encode value length feild
    bool EncodeFieldExpriyValue(MmsEncodeBuffer &buff, int64_t value);
    bool EncodeFieldFromValue(MmsEncodeBuffer &buff, std::vector<MmsAddress> &addr);
    bool EcondeFieldMessageClassValue(MmsEncodeBuffer &buff);
    // end

    bool EncodeCommontFieldValue(MmsEncodeBuffer &buff);
    bool EncodeMultipleAddressValue(MmsEncodeBuffer &buff, uint8_t fieldId, std::vector<MmsAddress> &addrs);

    bool EncodeMmsSendReq(MmsEncodeBuffer &buff);
    bool EncodeMmsSendConf(MmsEncodeBuffer &buff);
    bool EncodeMmsNotificationInd(MmsEncodeBuffer &buff);
    bool EnocdeMmsNotifyRespInd(MmsEncodeBuffer &buff);
    bool EnocdeMmsRetrieveConf(MmsEncodeBuffer &buff);
    bool EnocdeMmsAcknowledgeInd(MmsEncodeBuffer &buff);
    bool EnocdeMmsDeliveryInd(MmsEncodeBuffer &buff);
    bool EncodeMmsReadRecInd(MmsEncodeBuffer &buff);
    bool EncodeMmsReadOrigInd(MmsEncodeBuffer &buff);

    void TrimString(std::string &str);  // trim string two side <>
    bool EncodeOctetValue(MmsEncodeBuffer &buff, uint8_t fieldId, uint8_t value);
    bool EncodeTextStringValue(MmsEncodeBuffer &buff, uint8_t fieldId, std::string value);
    bool EncodeEncodeStringValue(MmsEncodeBuffer &buff, uint8_t fieldId, MmsEncodeString value);
    bool EncodeShortIntegerValue(MmsEncodeBuffer &buff, uint8_t fieldId, int64_t value);
    bool EncodeLongIntergerValue(MmsEncodeBuffer &buff, uint8_t fieldId, int64_t value);

    bool EncodeOctetValueFromMap(MmsEncodeBuffer &buff, uint8_t fieldId);
    bool EncodeTextStringValueFromMap(MmsEncodeBuffer &buff, uint8_t fieldId);
    bool EnocdeEncodeStringValueFromMap(MmsEncodeBuffer &buff, uint8_t fieldId);
    bool EnocdeShortIntegerValueFromMap(MmsEncodeBuffer &buff, uint8_t fieldId);
    bool EncodeLongIntergerValueFromMap(MmsEncodeBuffer &buff, uint8_t fieldId);

    bool IsHaveTransactionId(uint8_t messageType);
    bool CheckResponseStatus(uint8_t value);
    bool CheckRetrieveStatus(uint8_t value);
    bool CheckStoreStatus(uint8_t value);
    bool CheckBooleanValue(uint8_t fieldId, uint8_t value);

    using Func = bool (MmsHeader::*)(uint8_t fieldId, MmsDecodeBuffer &buff, int32_t &len);
    std::map<uint8_t, Func> memberFuncMap_;

    std::map<uint8_t, uint8_t> octetValueMap_;
    std::map<uint8_t, int64_t> longValueMap_;
    std::map<uint8_t, std::string> textValueMap_;
    std::map<uint8_t, MmsEncodeString> encodeStringsMap_;
    std::map<uint8_t, std::vector<MmsAddress>> addressMap_;
private:
    MmsContentType mmsContentType_;
    MmsHeaderCateg mmsHaderCateg_;
};
} // namespace Telephony
} // namespace OHOS
#endif

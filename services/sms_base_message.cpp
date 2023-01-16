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

#include "sms_base_message.h"

#include "msg_text_convert.h"
#include "sms_mms_errors.h"
#include "sms_service_manager_client.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace std;
static constexpr uint8_t PID_87 = 0xc0;
static constexpr uint8_t PID_7 = 0x40;
static constexpr uint8_t PID_10_LOW = 0x3f;
static constexpr int16_t WAP_PUSH_PORT = 2948;
static constexpr uint8_t MAX_GSM_7BIT_DATA_LEN = 160;
static constexpr uint8_t MAX_UCS2_DATA_LEN = 140;
static constexpr uint8_t BYTE_BITS = 8;
static constexpr uint8_t MAX_ADD_PARAM_LEN = 12;
static constexpr uint8_t GSM_BEAR_DATA_LEN = 140;
static constexpr uint8_t CHARSET_7BIT_BITS = 7;
static constexpr uint16_t TAPI_TEXT_SIZE_MAX = 520;

string SmsBaseMessage::GetSmscAddr() const
{
    return scAddress_;
}

void SmsBaseMessage::SetSmscAddr(const string &address)
{
    scAddress_ = address;
}

string SmsBaseMessage::GetOriginatingAddress() const
{
    return originatingAddress_;
}

string SmsBaseMessage::GetVisibleOriginatingAddress() const
{
    return originatingAddress_;
}

enum SmsMessageClass SmsBaseMessage::GetMessageClass() const
{
    return msgClass_;
}

string SmsBaseMessage::GetVisibleMessageBody() const
{
    return visibleMessageBody_;
}

std::vector<uint8_t> SmsBaseMessage::GetRawPdu() const
{
    return rawPdu_;
}

std::string SmsBaseMessage::GetRawUserData() const
{
    return rawUserData_;
}

long SmsBaseMessage::GetScTimestamp() const
{
    return scTimestamp_;
}

// 3GPP TS 23.040 V5.1.0 9.2.3.9 TP Protocol Identifier (TP PID)
bool SmsBaseMessage::IsReplaceMessage()
{
    uint8_t temp = static_cast<uint8_t>(protocolId_);
    uint8_t tempPid = temp & PID_10_LOW;
    bReplaceMessage_ = ((temp & PID_87) == PID_7) && (tempPid > 0) && (tempPid < MAX_REPLY_PID);
    return bReplaceMessage_;
}

// Message Waiting Indication Status storage on the USIM
bool SmsBaseMessage::IsCphsMwi() const
{
    return bCphsMwi_;
}

// 3GPP TS 23.040 V5.1.0 3.2.6 Messages Waiting
bool SmsBaseMessage::IsMwiClear() const
{
    return bMwiClear_;
}

// 3GPP TS 23.040 V5.1.0 3.2.6 Messages Waiting
bool SmsBaseMessage::IsMwiSet() const
{
    return bMwiSet_;
}

// 3GPP TS 23.040 V5.1.0 3.2.6 Messages Waiting
bool SmsBaseMessage::IsMwiNotStore() const
{
    return bMwiNotStore_;
}

int SmsBaseMessage::GetStatus() const
{
    return status_;
}

bool SmsBaseMessage::IsSmsStatusReportMessage() const
{
    return bStatusReportMessage_;
}

bool SmsBaseMessage::HasReplyPath() const
{
    return hasReplyPath_;
}

int SmsBaseMessage::GetProtocolId() const
{
    return protocolId_;
}

std::shared_ptr<SmsConcat> SmsBaseMessage::GetConcatMsg()
{
    smsConcat_ = nullptr;
    for (int i = 0; i < smsUserData_.headerCnt; i++) {
        if (smsUserData_.header[i].udhType == SMS_UDH_CONCAT_8BIT) {
            smsConcat_ = std::make_shared<SmsConcat>();
            if (smsConcat_ == nullptr) {
                TELEPHONY_LOGE("smsConcat is nullptr.");
                break;
            }
            smsConcat_->is8Bits = true;
            smsConcat_->totalSeg = smsUserData_.header[i].udh.concat8bit.totalSeg;
            smsConcat_->seqNum = smsUserData_.header[i].udh.concat8bit.seqNum;
            smsConcat_->msgRef = smsUserData_.header[i].udh.concat8bit.msgRef;
            break;
        } else if (smsUserData_.header[i].udhType == SMS_UDH_CONCAT_16BIT) {
            smsConcat_ = std::make_shared<SmsConcat>();
            if (smsConcat_ == nullptr) {
                TELEPHONY_LOGE("smsConcat is nullptr.");
                break;
            }
            smsConcat_->is8Bits = false;
            smsConcat_->totalSeg = smsUserData_.header[i].udh.concat16bit.totalSeg;
            smsConcat_->seqNum = smsUserData_.header[i].udh.concat16bit.seqNum;
            smsConcat_->msgRef = smsUserData_.header[i].udh.concat16bit.msgRef;
            break;
        }
    }
    return smsConcat_;
}

std::shared_ptr<SmsAppPortAddr> SmsBaseMessage::GetPortAddress()
{
    portAddress_ = nullptr;
    for (int i = 0; i < smsUserData_.headerCnt; i++) {
        if (smsUserData_.header[i].udhType == SMS_UDH_APP_PORT_8BIT) {
            portAddress_ = std::make_shared<SmsAppPortAddr>();
            if (portAddress_ == nullptr) {
                TELEPHONY_LOGE("portAddress_ is nullptr.");
                break;
            }
            portAddress_->is8Bits = true;
            portAddress_->destPort = smsUserData_.header[i].udh.appPort8bit.destPort;
            portAddress_->originPort = smsUserData_.header[i].udh.appPort8bit.originPort;
            break;
        } else if (smsUserData_.header[i].udhType == SMS_UDH_APP_PORT_16BIT) {
            portAddress_ = std::make_shared<SmsAppPortAddr>();
            if (portAddress_ == nullptr) {
                TELEPHONY_LOGE("portAddress_ is nullptr.");
                break;
            }
            portAddress_->is8Bits = false;
            portAddress_->destPort = smsUserData_.header[i].udh.appPort16bit.destPort;
            portAddress_->originPort = smsUserData_.header[i].udh.appPort16bit.originPort;
            break;
        }
    }
    return portAddress_;
}

std::shared_ptr<SpecialSmsIndication> SmsBaseMessage::GetSpecialSmsInd()
{
    specialSmsInd_ = nullptr;
    for (int i = 0; i < smsUserData_.headerCnt; i++) {
        if (smsUserData_.header[i].udhType == SMS_UDH_SPECIAL_SMS) {
            specialSmsInd_ = std::make_shared<SpecialSmsIndication>();
            if (specialSmsInd_ == nullptr) {
                TELEPHONY_LOGE("specialSmsInd_ is nullptr.");
                break;
            }
            specialSmsInd_->bStore = smsUserData_.header[i].udh.specialInd.bStore;
            specialSmsInd_->msgInd = smsUserData_.header[i].udh.specialInd.msgInd;
            specialSmsInd_->waitMsgNum = smsUserData_.header[i].udh.specialInd.waitMsgNum;
            break;
        }
    }
    return specialSmsInd_;
}

bool SmsBaseMessage::IsConcatMsg()
{
    return (GetConcatMsg() == nullptr) ? false : true;
}

bool SmsBaseMessage::IsWapPushMsg()
{
    std::shared_ptr<SmsAppPortAddr> portAddress = GetPortAddress();
    if (portAddress != nullptr && !portAddress->is8Bits) {
        return portAddress->destPort == WAP_PUSH_PORT;
    }
    return false;
}

void SmsBaseMessage::ConvertMessageClass(enum SmsMessageClass msgClass)
{
    switch (msgClass) {
        case SMS_SIM_MESSAGE:
            msgClass_ = SmsMessageClass::SMS_SIM_MESSAGE;
            break;
        case SMS_INSTANT_MESSAGE:
            msgClass_ = SmsMessageClass::SMS_INSTANT_MESSAGE;
            break;
        case SMS_OPTIONAL_MESSAGE:
            msgClass_ = SmsMessageClass::SMS_OPTIONAL_MESSAGE;
            break;
        case SMS_FORWARD_MESSAGE:
            msgClass_ = SmsMessageClass::SMS_FORWARD_MESSAGE;
            break;
        default:
            msgClass_ = SmsMessageClass::SMS_CLASS_UNKNOWN;
            break;
    }
}

int SmsBaseMessage::GetMsgRef()
{
    return msgRef_;
}

int SmsBaseMessage::GetSegmentSize(
    SmsCodingScheme &codingScheme, int dataLen, bool bPortNum, MSG_LANGUAGE_ID_T &langId, int replyAddrLen) const
{
    const int headerLen = 1;
    const int concat = 5;
    const int port = 6;
    const int lang = 3;
    const int reply = 2;
    int headerSize = 0;
    int segSize = 0;
    int maxSize = 0;
    if (codingScheme == SMS_CODING_7BIT || codingScheme == SMS_CODING_ASCII7BIT) {
        maxSize = MAX_GSM_7BIT_DATA_LEN;
    } else if (codingScheme == SMS_CODING_8BIT || codingScheme == SMS_CODING_UCS2) {
        maxSize = MAX_UCS2_DATA_LEN;
    }

    if (bPortNum == true) {
        headerSize += port;
    }

    if (langId != MSG_ID_RESERVED_LANG) {
        headerSize += lang;
    }

    if (replyAddrLen > 0) {
        headerSize += reply;
        headerSize += replyAddrLen;
    }

    if (codingScheme == SMS_CODING_7BIT || codingScheme == SMS_CODING_ASCII7BIT) {
        if ((dataLen + headerSize) > maxSize) {
            segSize =
                ((GSM_BEAR_DATA_LEN * BYTE_BITS) - ((headerLen + concat + headerSize) * BYTE_BITS)) / CHARSET_7BIT_BITS;
        } else {
            segSize = dataLen;
        }
    } else if (codingScheme == SMS_CODING_8BIT || codingScheme == SMS_CODING_UCS2) {
        if ((dataLen + headerSize) > maxSize) {
            segSize = GSM_BEAR_DATA_LEN - (headerLen + concat + headerSize);
        } else {
            segSize = dataLen;
        }
    }

    return segSize;
}

int SmsBaseMessage::GetMaxSegmentSize(
    SmsCodingScheme &codingScheme, int dataLen, bool bPortNum, MSG_LANGUAGE_ID_T &langId, int replyAddrLen) const
{
    const int headerLen = 1;
    const int concat = 5;
    const int port = 6;
    const int lang = 3;
    const int reply = 2;
    int headerSize = 0;
    int segSize = 0;
    int maxSize = 0;
    if (codingScheme == SMS_CODING_7BIT || codingScheme == SMS_CODING_ASCII7BIT) {
        maxSize = MAX_GSM_7BIT_DATA_LEN;
    } else if (codingScheme == SMS_CODING_8BIT || codingScheme == SMS_CODING_UCS2) {
        maxSize = MAX_UCS2_DATA_LEN;
    }
    if (bPortNum) {
        headerSize += port;
    }
    if (langId != MSG_ID_RESERVED_LANG) {
        headerSize += lang;
    }
    if (replyAddrLen > 0) {
        headerSize += reply;
        headerSize += replyAddrLen;
    }
    if (codingScheme == SMS_CODING_7BIT || codingScheme == SMS_CODING_ASCII7BIT) {
        if ((dataLen + headerSize) > maxSize) {
            segSize =
                ((GSM_BEAR_DATA_LEN * BYTE_BITS) - ((headerLen + concat + headerSize) * BYTE_BITS)) / CHARSET_7BIT_BITS;
        } else {
            segSize = maxSize - headerSize;
        }
    } else if (codingScheme == SMS_CODING_8BIT || codingScheme == SMS_CODING_UCS2) {
        if ((dataLen + headerSize) > maxSize) {
            segSize = GSM_BEAR_DATA_LEN - (headerLen + concat + headerSize);
        } else {
            segSize = maxSize - headerSize;
        }
    }
    return segSize;
}

void SmsBaseMessage::ConvertSpiltToUtf8(SplitInfo &split, const SmsCodingScheme &codingType)
{
    MsgTextConvert *textCvt = MsgTextConvert::Instance();
    if (textCvt == nullptr || split.encodeData.size() <= 0) {
        TELEPHONY_LOGE("MsgTextConvert Instance is nullptr");
        return;
    }

    int dataSize = 0;
    unsigned char buff[MAX_MSG_TEXT_LEN + 1] = { 0 };
    switch (codingType) {
        case SMS_CODING_7BIT: {
            MsgLangInfo langInfo = {
                0,
            };
            langInfo.bSingleShift = false;
            langInfo.bLockingShift = false;
            dataSize = textCvt->ConvertGSM7bitToUTF8(
                buff, MAX_MSG_TEXT_LEN, split.encodeData.data(), split.encodeData.size(), &langInfo);
            break;
        }
        case SMS_CODING_UCS2: {
            dataSize =
                textCvt->ConvertUCS2ToUTF8(buff, MAX_MSG_TEXT_LEN, split.encodeData.data(), split.encodeData.size());
            break;
        }
        default: {
            if (split.encodeData.size() > sizeof(buff)) {
                TELEPHONY_LOGE("AnalsisDeliverMsg data length invalid.");
                return;
            }
            if (memcpy_s(buff, sizeof(buff), split.encodeData.data(), split.encodeData.size()) != EOK) {
                TELEPHONY_LOGE("AnalsisDeliverMsg memcpy_s fail.");
                return;
            }
            dataSize = static_cast<int>(split.encodeData.size());
            buff[dataSize] = '\0';
            break;
        }
    }

    split.text.insert(0, reinterpret_cast<char *>(buff), dataSize);
    TELEPHONY_LOGI("split text");
}

void SmsBaseMessage::SplitMessage(std::vector<struct SplitInfo> &splitResult, const std::string &text,
    bool force7BitCode, SmsCodingScheme &codingType, bool bPortNum)
{
    std::string msgText(text);
    unsigned char decodeData[(MAX_GSM_7BIT_DATA_LEN * MAX_SEGMENT_NUM) + 1];
    if (memset_s(decodeData, sizeof(decodeData), 0x00, sizeof(decodeData)) != EOK) {
        TELEPHONY_LOGE("SplitMessage memset_s error!");
        return;
    }

    int encodeLen = 0;
    bool bAbnormal = false;
    MSG_LANGUAGE_ID_T langId = MSG_ID_RESERVED_LANG;
    codingType = force7BitCode ? SMS_CODING_7BIT : SMS_CODING_AUTO;
    encodeLen = DecodeMessage(decodeData, sizeof(decodeData), codingType, msgText, bAbnormal, langId);
    if (encodeLen <= 0) {
        TELEPHONY_LOGE("encodeLen Less than or equal to 0");
        return;
    }

    int index = 0;
    int segSize = 0;
    int segCount = 0;
    segSize = GetSegmentSize(codingType, encodeLen, bPortNum, langId, MAX_ADD_PARAM_LEN);
    if (segSize > 0) {
        segCount = ceil((double)encodeLen / (double)segSize);
    }

    for (int i = 0; i < segCount; i++) {
        int userDataLen = 0;
        struct SplitInfo splitInfo;
        splitInfo.langId = langId;
        splitInfo.encodeType = codingType;
        uint8_t textData[TAPI_TEXT_SIZE_MAX + 1];
        (void)memset_s(textData, sizeof(textData), 0x00, sizeof(textData));
        if ((i + 1) == segCount) {
            userDataLen = encodeLen - (i * segSize);
        } else {
            userDataLen = segSize;
        }
        splitInfo.encodeData = std::vector<uint8_t>(&decodeData[index], &decodeData[index] + userDataLen);
        ConvertSpiltToUtf8(splitInfo, codingType);
        splitResult.push_back(splitInfo);
        index += segSize;
    }
}

int32_t SmsBaseMessage::GetSmsSegmentsInfo(const std::string &message, bool force7BitCode, LengthInfo &lenInfo)
{
    unsigned char decodeData[(MAX_GSM_7BIT_DATA_LEN * MAX_SEGMENT_NUM) + 1];
    if (memset_s(decodeData, sizeof(decodeData), 0x00, sizeof(decodeData)) != EOK) {
        TELEPHONY_LOGE("SplitMessage memset_s error!");
        return TELEPHONY_ERR_MEMSET_FAIL;
    }
    const uint8_t smsEncodingUnkown = 0;
    const uint8_t smsEncoding7Bit = 1;
    const uint8_t smsEncoding8Bit = 2;
    const uint8_t smsEncoding16Bit = 3;
    int encodeLen = 0;
    bool bAbnormal = false;
    MSG_LANGUAGE_ID_T langId = MSG_ID_RESERVED_LANG;
    SmsCodingScheme codingType = force7BitCode ? SMS_CODING_7BIT : SMS_CODING_AUTO;
    encodeLen = DecodeMessage(decodeData, sizeof(decodeData), codingType, message, bAbnormal, langId);
    if (encodeLen <= 0) {
        TELEPHONY_LOGE("encodeLen Less than or equal to 0");
        return SMS_MMS_DECODE_DATA_EMPTY;
    }
    int segSize = GetMaxSegmentSize(codingType, encodeLen, false, langId, MAX_ADD_PARAM_LEN);
    TELEPHONY_LOGI("segSize = %{public}d", segSize);
    lenInfo.msgEncodeCount = static_cast<uint16_t>(encodeLen);
    if (codingType == SMS_CODING_7BIT || codingType == SMS_CODING_ASCII7BIT) {
        lenInfo.dcs = smsEncoding7Bit;
    } else if (codingType == SMS_CODING_UCS2) {
        lenInfo.dcs = smsEncoding16Bit;
    } else if (codingType == SMS_CODING_8BIT) {
        lenInfo.dcs = smsEncoding8Bit;
    } else {
        lenInfo.dcs = smsEncodingUnkown;
    }
    if (lenInfo.dcs == smsEncoding16Bit) {
        lenInfo.msgEncodeCount = lenInfo.msgEncodeCount / 2;
        segSize = segSize / 2;
    }
    if (segSize != 0) {
        lenInfo.msgRemainCount = static_cast<uint8_t>(((segSize - (lenInfo.msgEncodeCount % segSize))) % segSize);
        lenInfo.msgSegCount = ceil(static_cast<double>(lenInfo.msgEncodeCount) / static_cast<double>(segSize));
    }
    return TELEPHONY_ERR_SUCCESS;
}

int32_t SmsBaseMessage::GetIndexOnSim() const
{
    return indexOnSim_;
}

void SmsBaseMessage::SetIndexOnSim(int32_t index)
{
    indexOnSim_ = index;
}
} // namespace Telephony
} // namespace OHOS

/*
 * Copyright (C) 2023 Huawei Device Co., Ltd.
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

#include "gsm_sms_param_encode.h"

#include "gsm_pdu_hex_value.h"
#include "gsm_sms_common_utils.h"
#include "gsm_user_data_pdu.h"
#include "securec.h"
#include "sms_common_utils.h"
#include "telephony_log_wrapper.h"
#include "text_coder.h"

namespace OHOS {
namespace Telephony {
static constexpr uint8_t MAX_ADD_PARAM_LEN = 12;
static constexpr uint8_t MAX_SMSC_LEN = 20;
static constexpr uint8_t SLIDE_DATA_STEP = 2;
static constexpr uint8_t DIGIT_TEN = 10;
static constexpr uint8_t REL_TIME_LEN = 1;
static constexpr uint8_t MIN_SMSC_LEN = 2;

bool GsmSmsParamEncode::EncodeAddressPdu(const struct AddressNumber *num, std::string &resultNum)
{
    if (num == nullptr) {
        TELEPHONY_LOGE("nullptr error!");
        return false;
    }
    const char *temp = static_cast<const char *>(num->address);
    size_t tempLen = strlen(temp);
    if (tempLen > MAX_SMSC_LEN) {
        TELEPHONY_LOGE("temp over size!");
        return false;
    }
    unsigned char ton = 0;
    if (temp[0] == '+') {
        resultNum.push_back(tempLen - 1);
        temp++;
        tempLen--;
        ton = TYPE_INTERNATIONAL;
    } else {
        resultNum.push_back(tempLen);
        ton = num->ton;
    }

    /* Set TON, NPI */
    resultNum.push_back(HEX_VALUE_80 + (ton << HEX_VALUE_04) + num->npi);
    GsmSmsCommonUtils utils;
    uint8_t length = 0;
    uint8_t bcd[MAX_ADD_PARAM_LEN] = { 0 };
    if (!utils.DigitToBcd(temp, tempLen, bcd, MAX_ADD_PARAM_LEN, length)) {
        TELEPHONY_LOGE("DigitToBcd fail!");
        return false;
    }
    if (resultNum.size() + length > MAX_ADD_PARAM_LEN) {
        TELEPHONY_LOGE("length over size.");
        return false;
    }
    for (uint8_t index = 0; index < length; index++) {
        resultNum.push_back(static_cast<char>(bcd[index]));
    }
    return true;
}

uint8_t GsmSmsParamEncode::EncodeSmscPdu(const char *num, uint8_t *resultNum)
{
    if (num == nullptr || resultNum == nullptr) {
        TELEPHONY_LOGE("Address or EncodeAddr is null.");
        return 0;
    }

    char newNum[MAX_SMSC_LEN + 1];
    if (memset_s(newNum, sizeof(newNum), 0x00, sizeof(newNum)) != EOK) {
        TELEPHONY_LOGE("memset_s error!");
        return 0;
    }
    int32_t ret = 0;
    if (num[0] == '+') {
        ret = strncpy_s(newNum, sizeof(newNum), num + 1, MAX_SMSC_LEN);
    } else {
        ret = strncpy_s(newNum, sizeof(newNum), num, MAX_SMSC_LEN);
    }
    if (ret != EOK) {
        TELEPHONY_LOGE("EncodeSmscPdu error!");
        return 0;
    }
    /* Set Address */
    uint8_t encodeLen = 0;
    GsmSmsCommonUtils utils;
    if (!utils.DigitToBcd(newNum, strlen(newNum), resultNum, MAX_SMSC_LEN, encodeLen)) {
        TELEPHONY_LOGE("digit to bcd error!");
        return 0;
    }
    if (encodeLen == 0 || encodeLen >= MAX_SMSC_LEN) {
        TELEPHONY_LOGE("encodeLen error!");
        return 0;
    }
    resultNum[encodeLen] = '\0';
    return encodeLen;
}

uint8_t GsmSmsParamEncode::EncodeSmscPdu(const struct AddressNumber *num, uint8_t *smscNum, uint8_t smscLen)
{
    if (num == nullptr || smscNum == nullptr) {
        TELEPHONY_LOGE("nullptr error.");
        return 0;
    }

    char newNum[MAX_SMSC_LEN + 1];
    if (memset_s(newNum, sizeof(newNum), 0x00, sizeof(newNum)) != EOK) {
        TELEPHONY_LOGE("memset_s error!");
        return 0;
    }

    int32_t ret = 0;
    if (num->address[0] == '+') {
        ret = memcpy_s(newNum, sizeof(newNum), num->address + 1, strlen(num->address) - 1);
    } else {
        ret = memcpy_s(newNum, sizeof(newNum), num->address, sizeof(newNum));
    }
    if (ret != EOK) {
        TELEPHONY_LOGE("memory copy error!");
        return 0;
    }
    uint8_t addrLen = strlen(newNum);
    uint8_t dataSize = 0;
    if (addrLen % SLIDE_DATA_STEP == 0) {
        dataSize = SLIDE_DATA_STEP + (addrLen / SLIDE_DATA_STEP);
    } else {
        dataSize = SLIDE_DATA_STEP + (addrLen / SLIDE_DATA_STEP) + 1;
    }
    if (dataSize > MAX_SMSC_LEN || dataSize >= smscLen || dataSize <= 1 || smscLen <= HEX_VALUE_02) {
        TELEPHONY_LOGE("DataSize error!");
        return 0;
    }

    smscNum[0] = static_cast<char>(dataSize - 1);
    smscNum[1] = HEX_VALUE_80 + (static_cast<unsigned char>(num->ton << HEX_VALUE_04)) + num->npi;
    GsmSmsCommonUtils utils;
    uint8_t len = 0;
    if (smscLen > MIN_SMSC_LEN &&
        !utils.DigitToBcd(newNum, addrLen, &(smscNum[MIN_SMSC_LEN]), smscLen - MIN_SMSC_LEN, len)) {
        TELEPHONY_LOGE("digit to bcd error!");
        return 0;
    }
    smscNum[dataSize] = '\0';
    return dataSize;
}

void GsmSmsParamEncode::EncodeDCS(const struct SmsDcs *dcsData, std::string &returnValue)
{
    if (dcsData == nullptr) {
        TELEPHONY_LOGE("dcsData is null.");
        return;
    }
    switch (dcsData->codingGroup) {
        case CODING_DELETION_GROUP:
        case CODING_DISCARD_GROUP:
        case CODING_STORE_GROUP:
            /* not supported */
            break;
        case CODING_GENERAL_GROUP:
            if (dcsData->msgClass != SMS_CLASS_UNKNOWN) {
                returnValue.push_back(HEX_VALUE_10 + dcsData->msgClass);
            }
            if (dcsData->bCompressed) {
                char value = returnValue.front();
                returnValue.pop_back();
                returnValue.push_back(value | HEX_VALUE_20);
            }
            break;
        case SMS_CLASS_GROUP:
            returnValue.push_back(HEX_VALUE_F0 + dcsData->msgClass);
            break;
        default:
            return;
    }

    char value = returnValue.front();
    if (!returnValue.empty()) {
        returnValue.pop_back();
    }
    switch (dcsData->codingScheme) {
        case DATA_CODING_7BIT:
            break;
        case DATA_CODING_UCS2:
            returnValue.push_back(value | HEX_VALUE_08);
            break;
        case DATA_CODING_8BIT:
            returnValue.push_back(value | HEX_VALUE_04);
            break;
        default:
            break;
    }
}

void GsmSmsParamEncode::EncodeTimePdu(const struct SmsTimeStamp *timeStamp, std::string &resultValue)
{
    if (timeStamp == nullptr) {
        TELEPHONY_LOGE("TimeStamp is null.");
        return;
    }
    if (timeStamp->format == SMS_TIME_ABSOLUTE) {
        int32_t timeZone = timeStamp->time.absolute.timeZone;
        resultValue.push_back(((timeStamp->time.absolute.year % DIGIT_TEN) << HEX_VALUE_04) +
                              (timeStamp->time.absolute.year / DIGIT_TEN));
        resultValue.push_back(((timeStamp->time.absolute.month % DIGIT_TEN) << HEX_VALUE_04) +
                              (timeStamp->time.absolute.month / DIGIT_TEN));
        resultValue.push_back(
            ((timeStamp->time.absolute.day % DIGIT_TEN) << HEX_VALUE_04) + (timeStamp->time.absolute.day / DIGIT_TEN));
        resultValue.push_back(((timeStamp->time.absolute.hour % DIGIT_TEN) << HEX_VALUE_04) +
                              (timeStamp->time.absolute.hour / DIGIT_TEN));
        resultValue.push_back(((timeStamp->time.absolute.minute % DIGIT_TEN) << HEX_VALUE_04) +
                              (timeStamp->time.absolute.minute / DIGIT_TEN));
        resultValue.push_back(((timeStamp->time.absolute.second % DIGIT_TEN) << HEX_VALUE_04) +
                              (timeStamp->time.absolute.second / DIGIT_TEN));

        if (timeZone < 0) {
            resultValue.push_back(HEX_VALUE_08);
        }
        char value = (static_cast<uint8_t>(timeStamp->time.absolute.timeZone % DIGIT_TEN) << HEX_VALUE_04) +
                     (timeStamp->time.absolute.timeZone / DIGIT_TEN) + resultValue.front();
        resultValue.pop_back();
        resultValue.push_back(value);
        return;
    } else if (timeStamp->format == SMS_TIME_RELATIVE) {
        std::string tempStr(REL_TIME_LEN + 1, '\0');
        int ret = memcpy_s(tempStr.data(), REL_TIME_LEN + 1, &(timeStamp->time.relative.time), REL_TIME_LEN);
        if (ret != EOK) {
            TELEPHONY_LOGE("memcpy_s error!");
        }
        resultValue = tempStr;
    }
}
} // namespace Telephony
} // namespace OHOS

/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "gsmsmsparamcodec_fuzzer.h"

#define private public
#include "addsmstoken_fuzzer.h"
#include "core_manager_inner.h"
#include "i_sms_service_interface.h"
#include "sms_service.h"

using namespace OHOS::Telephony;
namespace OHOS {
static bool g_isInited = false;
static constexpr int32_t UINT8_COUNT = 256;
static constexpr int32_t CODE_SCHEME_SIZE = 6;
static constexpr int32_t TIME_FORMAT_SIZE = 3;
constexpr int32_t SLEEP_TIME_SECONDS = 1;

bool IsServiceInited()
{
    if (!g_isInited) {
        CoreManagerInner::GetInstance().isInitAllObj_ = true;
        DelayedSingleton<SmsService>::GetInstance()->registerToService_ = true;
        DelayedSingleton<SmsService>::GetInstance()->WaitCoreServiceToInit();
        DelayedSingleton<SmsService>::GetInstance()->OnStart();
        if (DelayedSingleton<SmsService>::GetInstance()->GetServiceRunningState() ==
            static_cast<int32_t>(Telephony::ServiceRunningState::STATE_RUNNING)) {
            g_isInited = true;
        }
    }
    return g_isInited;
}

void T7BitSubmitSms()
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector("21010B818176251308F4000002C130");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void Ucs2SubmitSms()
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector("21020B818176251308F40008046D4B8BD5");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void T7BitDeliverySms()
{
    std::vector<unsigned char> pdu =
        StringUtils::HexToByteVector("0891683108200075F4240D91688129562983F600001240800102142302C130");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void Ucs2DeliverySms()
{
    std::vector<unsigned char> pdu =
        StringUtils::HexToByteVector("0891683110206005F0240DA1688176251308F4000832500381459323044F60597D");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void StatusReportSms()
{
    std::vector<unsigned char> pdu =
        StringUtils::HexToByteVector("0891683110808805F006510B818176251308F4325013113382233250131143802300");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void MultiPageUcs2Sms()
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0891683110205005F06005A00110F00008325052214182238C050003D3030200310030002E0063006E002F007100410053004B00380020"
        "FF0C4EE5514D6B216708521D6263966476F851738D397528540E5F7154CD60A876846B635E384F7F752830024E2D56FD"
        "8054901A0041005000507545723D65B04F539A8CFF0C70B951FB0020002000680074007400700073003A002F002F0075002E0031003000"
        "3000310030002E");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void WapPushSms()
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0891683110205005F0640BA10156455102F1000432109261715023880605040B8423F04C06246170706C69636174696F6E2F766E642E77"
        "61702E6D6D732D6D65737361676500B487AF848C829850765030303031365A645430008D9089178031363630373532313930382F545950"
        "453D504C4D4E008A808E040001298D"
        "8805810303F47B83687474703A2F2F31302E3132332E31382E38303A3138302F76564F455F3000");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void DataSmsDeliverySms()
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0891683110808805F0640D91686106571209F80000327030021205231306050400640000E8329BFD06DDDF723619");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void Smsc00DeliverySms()
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector("00240D91689141468496F600001270721142432302B319");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void MultiPage7bitSms()
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector(
        "0891683110206005F0640DA1688176251308F4000032806190051123A00500030F020162B1582C168BC562B1582C168BC562B2198D369B"
        "CD68B5582C269BCD62B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C16"
        "8BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC562B1582C168BC540B1582C168BC562B1582C168BC56231D98C"
        "469BCD66");

    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void T7BitOtherSubmitSms()
{
    std::vector<unsigned char> pdu = StringUtils::HexToByteVector("00010005910110F0000003E17018");
    ShortMessage *message = new ShortMessage();
    if (message == nullptr) {
        return;
    }
    ShortMessage::CreateMessage(pdu, u"3gpp", *message);
    delete message;
}

void EncodePduItemsTest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    auto codec = std::make_shared<GsmSmsParamCodec>();
    AddressNumber *pAddress = new AddressNumber();
    std::string addrParam(reinterpret_cast<const char *>(data), size);
    codec->EncodeAddressPdu(pAddress, addrParam);

    uint8_t decodeData[UINT8_COUNT + 1];
    uint16_t len = UINT8_COUNT < size ? UINT8_COUNT : size;
    if (memcpy_s(decodeData, len, data, len) != EOK) {
        return;
    }
    codec->EncodeSmscPdu(addrParam.c_str(), decodeData);
    pAddress->address[0] = data[0];
    codec->EncodeAddressPdu(pAddress, addrParam);
    codec->EncodeSmscPdu(pAddress, decodeData, len);
    codec->EncodeAddressPdu(nullptr, addrParam);
    codec->EncodeSmscPdu(nullptr, decodeData);
    codec->EncodeSmscPdu(nullptr, decodeData, len);

    std::string timeParam(reinterpret_cast<const char *>(data), size);
    codec->EncodeTimePdu(nullptr, timeParam);
    SmsTimeStamp *stamp = new SmsTimeStamp();
    stamp->format = static_cast<SmsTimeFormat>(size % TIME_FORMAT_SIZE);
    stamp->time.absolute.timeZone = size;
    codec->EncodeTimePdu(stamp, timeParam);

    SmsDcs *dcs = new SmsDcs();
    std::string dcsParam(reinterpret_cast<const char *>(data), size);
    codec->EncodeDCS(nullptr, dcsParam);
    dcs->codingGroup = static_cast<PduSchemeGroup>(size % CODE_SCHEME_SIZE);
    dcs->codingScheme = static_cast<DataCodingScheme>(size % CODE_SCHEME_SIZE);
    codec->EncodeDCS(dcs, dcsParam);
}

void DecodePduItemsTest(const uint8_t *data, size_t size)
{
    if (!IsServiceInited()) {
        return;
    }
    T7BitSubmitSms();
    Ucs2SubmitSms();
    T7BitDeliverySms();
    Ucs2DeliverySms();
    StatusReportSms();
    MultiPageUcs2Sms();
    WapPushSms();
    DataSmsDeliverySms();
    Smsc00DeliverySms();
    MultiPage7bitSms();
    T7BitOtherSubmitSms();
    auto codec = std::make_shared<GsmSmsParamCodec>();
    std::string pdu(reinterpret_cast<const char *>(data), size);
    SmsReadBuffer buffer(pdu);
    AddressNumber *address = new AddressNumber();
    codec->DecodeAddressPdu(buffer, address);

    SmsTimeStamp *stamp = new SmsTimeStamp();
    codec->DecodeTimePdu(buffer, stamp);

    SmsDcs *dcs = new SmsDcs();
    codec->DecodeDcsPdu(buffer, dcs);

    int32_t setType[UINT8_COUNT] = { 0 };
    int32_t indType[UINT8_COUNT] { 0 };
    codec->CheckVoicemail(buffer, setType, indType);
}

void DoSomethingInterestingWithMyAPI(const uint8_t *data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }

    EncodePduItemsTest(data, size);
    DecodePduItemsTest(data, size);
    sleep(SLEEP_TIME_SECONDS);
    DelayedSingleton<SmsService>::DestroyInstance();
}
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;
    OHOS::DoSomethingInterestingWithMyAPI(data, size);
    return 0;
}

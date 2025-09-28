/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
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

#ifdef FUZZTEST
#define private public
#define protected public
#endif

#include "unittest_fuzzer.h"
#include "addsmstoken_fuzzer.h"
#include "mms_apn_info.h"
#include "mms_network_client.h"
#include "mms_persist_helper.h"
#include "mms_conn_callback_stub.h"
#include "cdma_sms_sender.h"
#include "cdma_sms_receive_handler.h"
#include "cdma_sms_message.h"

namespace OHOS {
using namespace Telephony;

class Fuzzer {
public:
    Fuzzer(const uint8_t* data, size_t size):g_data(data), g_size(size) {}
    virtual ~Fuzzer() {};
    virtual void StartFuzzerTest() = 0;

protected:
    std::string GetString(size_t size = 0)
    {
        std::string str(reinterpret_cast<const char*>(g_data), g_size);
        if (size != 0 && size < str.size()) {
            str.resize(size);
        }
        return str;
    }
    size_t GetPort(int nRangeStart = 0, int nRangeEnd = 0)
    {
        nRangeStart = nRangeStart != 0 ? nRangeStart : m_nPortRangeStartDef;
        nRangeEnd = nRangeEnd != 0 ? nRangeEnd : m_nPortRangeEndDef;
        nRangeEnd = nRangeEnd > nRangeStart ? nRangeEnd : nRangeStart + 1;

        return g_size % (nRangeEnd - nRangeStart) + nRangeStart;
    }
    template<typename T>
    T GetEnumRandom(std::vector<T> vect)
    {
        size_t nItem = g_size % vect.size();
        return vect[nItem];
    }

    void GetDataRange(char* szBuf, uint8_t& nSize, int nRangeMin, int nRangeMax)
    {
        nSize = g_size % (nRangeMax-nRangeMin) + nRangeMin;
        if (memset_s(szBuf, nSize, 0x00, nSize) != EOK
        || memcpy_s(szBuf, nSize, g_data, nSize) != EOK) {
            return;
        }
    }
    std::vector<uint8_t> GetPduVect()
    {
        std::string strData = GetString(30);
        std::vector<uint8_t> vectUint8(strData.begin(), strData.end());

        static std::vector<int> vectPduType {0, 1, 2, 3};
        vectUint8[0] = GetEnumRandom(vectPduType);
        return vectUint8;
    }
protected:
    SmsEncodingType GetEnumRandom_SmsEncodingType()
    {
        static std::vector<SmsEncodingType> vect{
            SmsEncodingType::OCTET, SmsEncodingType::EPM, SmsEncodingType::ASCII_7BIT,
            SmsEncodingType::IA5, SmsEncodingType::UNICODE, SmsEncodingType::SHIFT_JIS,
            SmsEncodingType::KOREAN, SmsEncodingType::LATIN_HEBREW, SmsEncodingType::LATIN,
            SmsEncodingType::GSM7BIT, SmsEncodingType::GSMDCS, SmsEncodingType::EUCKR,
            SmsEncodingType::RESERVED
        };
        return GetEnumRandom(vect);
    }
    CdmaTransportMsgType GetEnumRandom_CdmaTransportMsgType()
    {
        static std::vector<CdmaTransportMsgType> vect{
            CdmaTransportMsgType::P2P,
            CdmaTransportMsgType::BROADCAST,
            CdmaTransportMsgType::ACK
        };
        return GetEnumRandom(vect);
    }
    DataCodingScheme GetEnumRandom_DataCodingScheme()
    {
        static std::vector<DataCodingScheme> vect{
            DataCodingScheme::DATA_CODING_7BIT,
            DataCodingScheme::DATA_CODING_8BIT,
            DataCodingScheme::DATA_CODING_UCS2,
            DataCodingScheme::DATA_CODING_AUTO,
            DataCodingScheme::DATA_CODING_EUCKR,
            DataCodingScheme::DATA_CODING_ASCII7BIT
        };
        return GetEnumRandom(vect);
    }
    SmsTransTelsvcId GetEnumRandom_SmsTransTelsvcId()
    {
        static std::vector<SmsTransTelsvcId> vect = {
            SmsTransTelsvcId::CMT_91, SmsTransTelsvcId::CPT_95,
            SmsTransTelsvcId::CMT_95, SmsTransTelsvcId::VMN_95,
            SmsTransTelsvcId::WAP, SmsTransTelsvcId::WEMT,
            SmsTransTelsvcId::SCPT, SmsTransTelsvcId::CATPT,
            SmsTransTelsvcId::RESERVED
        };
        return GetEnumRandom(vect);
    }
    TeleserviceMsgType GetEnumRandom_TeleserviceMsgType()
    {
        static std::vector<TeleserviceMsgType> vect = {
            TeleserviceMsgType::DELIVER, TeleserviceMsgType::SUBMIT,
            TeleserviceMsgType::CANCEL, TeleserviceMsgType::DELIVERY_ACK,
            TeleserviceMsgType::USER_ACK, TeleserviceMsgType::READ_ACK,
            TeleserviceMsgType::DELIVER_REPORT, TeleserviceMsgType::SUBMIT_REPORT,
            TeleserviceMsgType::MAX_VALUE, TeleserviceMsgType::RESERVED
        };
        return GetEnumRandom(vect);
    }
protected:
    const uint8_t* g_data;
    const size_t  g_size;
    const int32_t g_slotId = 2;
    const int m_nPortRangeStartDef = 1024;
    const int m_nPortRangeEndDef = 49151;
};

/*
* services/mms/include/mms_apn_info.h
* services/mms/mms_apn_info.cpp
*/
class MmsApnInfoFuzzer final : public Fuzzer {
public:
    using Fuzzer::Fuzzer;
    ~MmsApnInfoFuzzer() final {};
public:
    void StartFuzzerTest() override
    {
        if (!CreateObject()) {
            return;
        }
        TestSetMmscUrl();
        TestGetMmscUrl();
        TestSetMmsProxyAddressAndProxyPort();
        TestGetMmsProxyAddressAndProxyPort();
        TestSplitAndMatchApnTypes();
        TestGetMmsApnValue();
        DestoryObject();
    }
protected:
    bool CreateObject()
    {
        m_pMmsApnInfo = std::make_unique<MmsApnInfo>(g_slotId);
        return m_pMmsApnInfo != nullptr;
    }
    void DestoryObject()
    {
        m_pMmsApnInfo.reset();
    }
    void TestSetMmscUrl()
    {
        std::string strUrl = GetString();
        m_pMmsApnInfo->setMmscUrl(strUrl);
    }
    void TestGetMmscUrl()
    {
        m_pMmsApnInfo->getMmscUrl();
    }
    void TestSetMmsProxyAddressAndProxyPort()
    {
        std::string strUrl = GetString();
        m_pMmsApnInfo->setMmsProxyAddressAndProxyPort(strUrl);
    }
    void TestGetMmsProxyAddressAndProxyPort()
    {
        m_pMmsApnInfo->getMmsProxyAddressAndProxyPort();
    }
    void TestSplitAndMatchApnTypes()
    {
        std::string apn = GetString();
        m_pMmsApnInfo->SplitAndMatchApnTypes(apn);
        apn += ",mms";
        m_pMmsApnInfo->SplitAndMatchApnTypes(apn);
    }
    void TestGetMmsApnValue()
    {
        int count = 0;
        std::string homeUrlVal = GetString();
        std::string mmsIPAddressVal = GetString();
        m_pMmsApnInfo->GetMmsApnValue(nullptr, count, homeUrlVal, mmsIPAddressVal);
    }
private:
    std::unique_ptr<MmsApnInfo> m_pMmsApnInfo;
};

/*
* services/mms/include/mms_network_client.h
* services/mms/mms_network_client.cpp
*/
class MmsNetworkClientFuzzer final : public Fuzzer {
public:
    using Fuzzer::Fuzzer;
    ~MmsNetworkClientFuzzer() final {}
public:
    void StartFuzzerTest() override
    {
        if (!CreateObject()) {
            return;
        }
        TestExecutePost();
        TestExecuteGet();
        TestGetMmsDataBuf();
        TestCheckSendConf();
        TestGetCoverUrl();
        TestUpdateMmsPduToStorage();
        TestGetMmsPduFromFile();
        TestGetMmsPduFromDataBase();
        TestDeleteMmsPdu();
        TestWriteBufferToFile();
        DestoryObject();
    }
protected:
    bool CreateObject()
    {
        m_pMmsNetworkClient = std::make_unique<MmsNetworkClient>(g_slotId);
        return m_pMmsNetworkClient != nullptr;
    }
    void DestoryObject()
    {
        m_pMmsNetworkClient.reset();
    }
    void TestExecutePost()
    {
        std::string  strNormalUrl = "www.baidu.com";
        std::string  strBadUrl = GetString(30);
        std::string  strPostData = GetString(256);
        m_pMmsNetworkClient->Execute("POST", strNormalUrl, strPostData, "ua", "uaprof");
        m_pMmsNetworkClient->Execute("POST", strBadUrl, strPostData, "ua", "uaprof");
    }
    void TestExecuteGet()
    {
        std::string  strNormalUrl = "www.baidu.com";
        std::string  strBadUrl = GetString(30);
        std::string  strPostData = GetString(256);
        m_pMmsNetworkClient->Execute("GET", strNormalUrl, strPostData, "ua", "uaprof");
        m_pMmsNetworkClient->Execute("GET", strBadUrl, strPostData, "ua", "uaprof");
    }
    void TestGetMmsDataBuf()
    {
        std::string  fileName = "www.baidu.com";
        std::string  strBuf = GetString(256);
        m_pMmsNetworkClient->GetMmsDataBuf(strBuf, fileName);
    }
    void TestCheckSendConf()
    {
        m_pMmsNetworkClient->CheckSendConf();
    }
    void TestGetCoverUrl()
    {
        std::string emptyStr;
        m_pMmsNetworkClient->GetCoverUrl(emptyStr);
        std::string str = GetString(20);
        m_pMmsNetworkClient->GetCoverUrl(str);
    }
    void TestUpdateMmsPduToStorage()
    {
        std::string storeDirName = GetString(20);
        m_pMmsNetworkClient->UpdateMmsPduToStorage(storeDirName);
    }
    void TestGetMmsPduFromFile()
    {
        std::string fileName = GetString(20);
        std::string strBuf = GetString(20);
        m_pMmsNetworkClient->GetMmsPduFromFile(fileName, strBuf);
    }
    void TestGetMmsPduFromDataBase()
    {
        std::string dbUrl = GetString(20);
        std::string strBuf = GetString(20);
        m_pMmsNetworkClient->GetMmsPduFromDataBase(dbUrl, strBuf);
    }
    void TestDeleteMmsPdu()
    {
        std::string dbUrl = GetString(20);
        m_pMmsNetworkClient->DeleteMmsPdu(dbUrl);
    }
    void TestWriteBufferToFile()
    {
        std::unique_ptr<char[]> bufp = std::make_unique<char[]>(g_size);
        std::string strPathName = GetString(20);
        m_pMmsNetworkClient->WriteBufferToFile(bufp, g_size, strPathName);
    }
private:
    std::unique_ptr<MmsNetworkClient> m_pMmsNetworkClient;
};

class MmsPersistHelperFuzzer final : public Fuzzer {
public:
    using Fuzzer::Fuzzer;
    ~MmsPersistHelperFuzzer() final {}
public:
    void StartFuzzerTest() override
    {
        if (!CreateObject()) {
            return;
        }
        TestMakeUrl();
        TestInsert();
        TestGet();
        TestDelete();
        TestGet();
        TestSetMmsPdu();
        TestSplitPdu();
        TestSplitUrl();
        DestoryObject();
    }
protected:
    bool CreateObject()
    {
        m_pMmsPersistHelper = std::make_unique<MmsPersistHelper>();
        return m_pMmsPersistHelper != nullptr;
    }
    void DestoryObject()
    {
        m_pMmsPersistHelper.reset();
    }
    void TestMakeUrl()
    {
        std::string strUrl = GetString();
    }
    void TestInsert()
    {
        std::string strMmsPdu = GetString();
        m_pMmsPersistHelper->InsertMmsPdu(strMmsPdu, m_strUrl);
    }
    void TestGet()
    {
        m_pMmsPersistHelper->GetMmsPdu(m_strUrl);
    }
    void TestDelete()
    {
        m_pMmsPersistHelper->DeleteMmsPdu(m_strUrl);
    }
    void TestSetMmsPdu()
    {
        std::string mmsPdu = GetString(100);
        m_pMmsPersistHelper->SetMmsPdu(mmsPdu);
    }
    void TestSplitPdu()
    {
        std::string mmsPdu = StringUtils::StringToHex(GetString(100));
        m_pMmsPersistHelper->SplitPdu(mmsPdu);
    }
    void TestSplitUrl()
    {
        std::string url = GetString(100);
        m_pMmsPersistHelper->SplitUrl(url);
        url += ",fuzzer";
        m_pMmsPersistHelper->SplitUrl(url);
    }
private:
    std::string                       m_strUrl;
    std::unique_ptr<MmsPersistHelper> m_pMmsPersistHelper;
};

class MmsConnCallbackStubFuzzer final : public Fuzzer {
public:
    using Fuzzer::Fuzzer;
    ~MmsConnCallbackStubFuzzer() final {}
public:
    void StartFuzzerTest() override
    {
        if (!CreateObject()) {
            return;
        }
        TestNetAvailable();
        DestoryObject();
    }
protected:
    bool CreateObject()
    {
        m_pMmsConnCallbackStub = std::make_unique<MmsConnCallbackStub>();
        return m_pMmsConnCallbackStub != nullptr;
    }
    void DestoryObject()
    {
        m_pMmsConnCallbackStub.reset();
    }
    void TestNetAvailable()
    {
        sptr<NetManagerStandard::NetHandle> pNetHandle;
        sptr<NetManagerStandard::NetAllCapabilities> pNetAllCap;
        sptr<NetManagerStandard::NetLinkInfo> pNetLinkInfo;

        m_pMmsConnCallbackStub->NetAvailable(pNetHandle);
        m_pMmsConnCallbackStub->NetCapabilitiesChange(pNetHandle, pNetAllCap);
        m_pMmsConnCallbackStub->NetConnectionPropertiesChange(pNetHandle, pNetLinkInfo);
        m_pMmsConnCallbackStub->NetLost(pNetHandle);
        m_pMmsConnCallbackStub->NetUnavailable();
        m_pMmsConnCallbackStub->NetBlockStatusChange(pNetHandle, false);
    }
private:
    std::unique_ptr<MmsConnCallbackStub> m_pMmsConnCallbackStub;
};

class CdmaSmsSenderFuzzer final : public Fuzzer {
public:
    using Fuzzer::Fuzzer;
    ~CdmaSmsSenderFuzzer() final {}
public:
    void StartFuzzerTest() override
    {
        if (!CreateObject()) {
            return;
        }
        TestTextBasedSmsDeliveryViaIms();
        TestSendSmsToRil();
        TestGetOrSimpleCall();
        EncodeMsg();
        DestoryObject();
    }
protected:
    bool CreateObject()
    {
        m_pCdmaSmsSender = std::make_unique<CdmaSmsSender>(g_slotId, &CdmaSmsSenderFuzzer::SendRetryFunc);
        m_pCdmaSmsSender->Init();
        return m_pCdmaSmsSender != nullptr;
    }
    void DestoryObject()
    {
        m_pCdmaSmsSender.reset();
    }
    static void SendRetryFunc(std::shared_ptr<SmsSendIndexer> index) {}
    void TestTextBasedSmsDeliveryViaIms()
    {
        std::string strScAddr = GetString(MAX_ADDRESS_LEN-1);
        std::string strDtAddr = GetString(MAX_ADDRESS_LEN-1);
        std::string strText = GetString();
        size_t      nPort   = GetPort();
        int32_t dataBaseId = 0;
        sptr<ISendShortMessageCallback> sendCallback = nullptr;
        sptr<IDeliveryShortMessageCallback> deliveryCallback = nullptr;
        bool isMmsApp = true;

        m_pCdmaSmsSender->TextBasedSmsDeliveryViaIms(strDtAddr, strScAddr, strText, sendCallback, deliveryCallback,
            dataBaseId, isMmsApp);
        m_pCdmaSmsSender->DataBasedSmsDeliveryViaIms(
            strDtAddr, strScAddr, nPort, g_data, g_size, sendCallback, deliveryCallback);
    }
    void TestSendSmsToRil()
    {
        std::string strScAddr = GetString(MAX_ADDRESS_LEN-1);
        std::string strDtAddr = GetString(MAX_ADDRESS_LEN-1);
        std::string strText = GetString();
        sptr<ISendShortMessageCallback> sendCallback = nullptr;
        sptr<IDeliveryShortMessageCallback> deliveryCallback = nullptr;
        std::shared_ptr<SmsSendIndexer> pSendIndexer
            = std::make_shared<SmsSendIndexer>(strDtAddr, strScAddr, strText,
            sendCallback, deliveryCallback);

        m_pCdmaSmsSender->SendSmsToRil(pSendIndexer);
        m_pCdmaSmsSender->ResendTextDelivery(nullptr);
        m_pCdmaSmsSender->ResendTextDelivery(pSendIndexer);
        m_pCdmaSmsSender->ResendDataDelivery(nullptr);
        m_pCdmaSmsSender->ResendDataDelivery(pSendIndexer);

        std::unique_ptr<CdmaTransportMsg> transMsg = std::make_unique<CdmaTransportMsg>();
        transMsg->type = GetEnumRandom_CdmaTransportMsgType();

        pSendIndexer->SetIsConcat(true);
        m_pCdmaSmsSender->SetConcact(pSendIndexer, transMsg);
    }
    void TestGetOrSimpleCall()
    {
        bool bIsSupported;
        m_pCdmaSmsSender->IsImsSmsSupported(g_slotId, bIsSupported);
        m_pCdmaSmsSender->GetSeqNum();
        m_pCdmaSmsSender->GetSubmitMsgId();
    }
    void EncodeMsg()
    {
        CdmaTransportMsg transMsg;
        transMsg.type = GetEnumRandom_CdmaTransportMsgType();
        m_pCdmaSmsSender->EncodeMsg(transMsg);
    }
private:
    std::unique_ptr<CdmaSmsSender> m_pCdmaSmsSender;
};

class CdmaSmsMessageFuzzer final : public Fuzzer {
public:
    using Fuzzer::Fuzzer;
    ~CdmaSmsMessageFuzzer() final {}
public:
    void StartFuzzerTest() override
    {
        if (!CreateObject()) {
            return;
        }
        TestCovertEncodingType();
        TestCreateMessage();
        TestAnalysisP2pMsg();
        TestAnalsisUserData();
        TestIsOrGet();
        DestoryObject();
    }
protected:
    bool CreateObject()
    {
        m_pCdmaSmsMessage = std::make_unique<CdmaSmsMessage>();
        return m_pCdmaSmsMessage != nullptr;
    }
    void DestoryObject()
    {
        m_pCdmaSmsMessage.reset();
    }
    void TestCovertEncodingType()
    {
        DataCodingScheme data = GetEnumRandom_DataCodingScheme();
        m_pCdmaSmsMessage->CovertEncodingType(data);
    }
    void TestCreateMessage()
    {
        std::vector<uint8_t> vectUint8 = GetPduVect();
        std::string pduHex = StringUtils::StringToHex(vectUint8);
        m_pCdmaSmsMessage->CreateMessage(pduHex);
    }
    void TestAnalysisP2pMsg()
    {
        CdmaP2PMsg p2pMsg;

        p2pMsg.teleserviceId = static_cast<uint16_t>(GetEnumRandom_SmsTransTelsvcId());
        p2pMsg.telesvcMsg.type = GetEnumRandom_TeleserviceMsgType();
        m_pCdmaSmsMessage->AnalysisP2pMsg(p2pMsg);
    }
    void TestAnalsisUserData()
    {
        SmsTeleSvcUserData userData;
        userData.encodeType = GetEnumRandom_SmsEncodingType();
        GetDataRange(userData.userData.data, userData.userData.length, 0, nMaxSmsUDPackageDataLen);
        m_pCdmaSmsMessage->AnalsisUserData(userData);
    }
    void TestIsOrGet()
    {
        m_pCdmaSmsMessage->GetTransTeleService();
        m_pCdmaSmsMessage->GetProtocolId();
        m_pCdmaSmsMessage->IsReplaceMessage();
        m_pCdmaSmsMessage->IsCphsMwi();
        m_pCdmaSmsMessage->IsWapPushMsg();
        m_pCdmaSmsMessage->GetSpecialSmsInd();
        m_pCdmaSmsMessage->GetDestPort();
        m_pCdmaSmsMessage->IsBroadcastMsg();
        m_pCdmaSmsMessage->GetCMASCategory();
        m_pCdmaSmsMessage->GetCMASResponseType();
        m_pCdmaSmsMessage->GetCMASSeverity();
        m_pCdmaSmsMessage->GetCMASUrgency();
        m_pCdmaSmsMessage->GetCMASCertainty();
        m_pCdmaSmsMessage->GetCMASMessageClass();
        m_pCdmaSmsMessage->IsCMAS();
        m_pCdmaSmsMessage->GetMessageId();
        m_pCdmaSmsMessage->GetFormat();
        m_pCdmaSmsMessage->GetLanguage();
        m_pCdmaSmsMessage->GetCbInfo();
        m_pCdmaSmsMessage->GetPriority();
        m_pCdmaSmsMessage->IsEmergencyMsg();
        m_pCdmaSmsMessage->GetServiceCategoty();
        m_pCdmaSmsMessage->GetGeoScope();
        m_pCdmaSmsMessage->GetReceTime();
    }
private:
    const int nMaxSmsUDPackageDataLen = MAX_USER_DATA_LEN;
    std::unique_ptr<CdmaSmsMessage> m_pCdmaSmsMessage;
};
}  // namespace OHOS

template<typename TY>
void RunFuzzer(const uint8_t* data, size_t size)
{
    if (data == nullptr || size == 0) {
        return;
    }
    std::unique_ptr<OHOS::Fuzzer> p = std::make_unique<TY>(data, size);
    p->StartFuzzerTest();
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::AddSmsTokenFuzzer token;

    RunFuzzer<OHOS::MmsApnInfoFuzzer>(data, size);
    RunFuzzer<OHOS::MmsNetworkClientFuzzer>(data, size);
    RunFuzzer<OHOS::MmsPersistHelperFuzzer>(data, size);
    RunFuzzer<OHOS::MmsConnCallbackStubFuzzer>(data, size);
    RunFuzzer<OHOS::CdmaSmsMessageFuzzer>(data, size);
    RunFuzzer<OHOS::CdmaSmsSenderFuzzer>(data, size);
    return 0;
}
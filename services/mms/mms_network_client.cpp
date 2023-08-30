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

#include "mms_network_client.h"

#include "core_manager_inner.h"
#include "mms_apn_info.h"
#include "mms_persist_helper.h"
#include "net_conn_client.h"
#include "net_handle.h"
#include "net_link_info.h"
#include "securec.h"
#include "telephony_errors.h"
#include "telephony_log_wrapper.h"

namespace OHOS {
namespace Telephony {
using namespace NetManagerStandard;
std::string METHOD_POST = "POST";
std::string METHOD_GET = "GET";
static constexpr int32_t URL_SIZE = 1024;
static constexpr uint32_t CODE_BUFFER_MAX_SIZE = 300 * 1024;
static constexpr int64_t CONNECTION_TIMEOUT = 60000;
static constexpr int64_t TRANS_OP_TIMEOUT = 60000;
static constexpr uint8_t LOW_DOWNLOAD_RATE = 5;
constexpr const char *SIMID_IDENT_PREFIX = "simId";
const bool STORE_MMS_PDU_TO_FILE = false;

MmsNetworkClient::MmsNetworkClient(int32_t slotId)
{
    slotId_ = slotId;
    connectionTimeout_ = CONNECTION_TIMEOUT;
    transOpTimeout_ = TRANS_OP_TIMEOUT;
}

MmsNetworkClient::~MmsNetworkClient() {}

int32_t MmsNetworkClient::Execute(const std::string &method, const std::string &mmsc, const std::string &data)
{
    if (METHOD_POST.compare(method) == 0) {
        return PostUrl(mmsc, data);
    } else if (METHOD_GET.compare(method) == 0) {
        return GetUrl(mmsc, data);
    }
    TELEPHONY_LOGI("Execute method error");
    return TELEPHONY_ERR_FAIL;
}

void MmsNetworkClient::InitLibCurl()
{
    CURLcode errCode = curl_global_init(CURL_GLOBAL_ALL);
    if (errCode != CURLE_OK) {
        TELEPHONY_LOGE("curl init failed, errCode:[%{public}x]!", errCode);
    }
}

void MmsNetworkClient::DestoryLibCurl()
{
    curl_global_cleanup();
}

int32_t MmsNetworkClient::PostUrl(const std::string &mmsc, const std::string &fileName)
{
    std::shared_ptr<MmsApnInfo> mmsApnInfo = std::make_shared<MmsApnInfo>(slotId_);
    if (mmsApnInfo == nullptr) {
        TELEPHONY_LOGE("mmsApnInfo is nullptr");
        return TELEPHONY_ERR_MMS_FAIL_APN_INVALID;
    }
    std::string mmscFromDataBase = mmsApnInfo->getMmscUrl();
    if (mmsc != mmscFromDataBase) {
        TELEPHONY_LOGE("mmsc is invalid");
        return TELEPHONY_ERR_ARGUMENT_INVALID;
    }

    std::string strBuf;
    if (STORE_MMS_PDU_TO_FILE) {
        if (!GetMmsPduFromFile(fileName, strBuf)) {
            TELEPHONY_LOGE("Get MmsPdu from file fail");
            return TELEPHONY_ERR_READ_DATA_FAIL;
        }
    } else {
        if (!GetMmsPduFromDataBase(fileName, strBuf)) {
            TELEPHONY_LOGE("Get MmsPdu from data base fail");
            return TELEPHONY_ERR_DATABASE_READ_FAIL;
        }
    }

    SetIfaceName(GetIfaceName());
    TELEPHONY_LOGI("strBuf length:%{public}d", static_cast<uint32_t>(strBuf.size()));
    std::string strResponse = "";
    int32_t ret = HttpPost(mmscFromDataBase, strBuf, strResponse);
    TELEPHONY_LOGI("ret: %{public}d,strResponse len: %{public}d", ret, static_cast<uint32_t>(strResponse.size()));

    CURLcode errCode = static_cast<CURLcode>(ret);
    if (!STORE_MMS_PDU_TO_FILE) {
        DeleteMmsPdu(fileName);
    }
    if (errCode != CURLE_OK) {
        return TELEPHONY_ERR_MMS_FAIL_HTTP_ERROR;
    } else {
        TELEPHONY_LOGI("send mms successed");
        return TELEPHONY_ERR_SUCCESS;
    }
}

bool MmsNetworkClient::GetMmsPduFromFile(const std::string &fileName, std::string &strBuf)
{
    FILE *pFile = nullptr;
    char realPath[PATH_MAX] = { 0 };
    if (fileName.empty() || realpath(fileName.c_str(), realPath) == nullptr) {
        TELEPHONY_LOGE("path or realPath is nullptr");
        return false;
    }

    pFile = fopen(realPath, "rb");
    if (pFile == nullptr) {
        TELEPHONY_LOGE("openFile Error");
        return false;
    }

    (void)fseek(pFile, 0, SEEK_END);
    long fileLen = ftell(pFile);
    if (fileLen <= 0 || fileLen > static_cast<long>(CODE_BUFFER_MAX_SIZE)) {
        (void)fclose(pFile);
        TELEPHONY_LOGE("Mms Over Long Error");
        return false;
    }

    std::unique_ptr<char[]> pduBuffer = std::make_unique<char[]>(fileLen);
    if (!pduBuffer) {
        (void)fclose(pFile);
        TELEPHONY_LOGE("make unique pduBuffer nullptr Error");
        return false;
    }
    (void)fseek(pFile, 0, SEEK_SET);
    int32_t totolLength = static_cast<int32_t>(fread(pduBuffer.get(), 1, CODE_BUFFER_MAX_SIZE, pFile));
    TELEPHONY_LOGI("sendMms fread totolLength%{public}d", totolLength);
    (void)fclose(pFile);

    long i = 0;
    while (i < fileLen) {
        strBuf += pduBuffer[i];
        i++;
    }
    return true;
}

bool MmsNetworkClient::GetMmsPduFromDataBase(const std::string &dbUrl, std::string &strBuf)
{
    if (dbUrl.empty()) {
        TELEPHONY_LOGE("dbUrl is empty");
        return false;
    }
    std::shared_ptr<MmsPersistHelper> mmsPdu = std::make_shared<MmsPersistHelper>();
    if (mmsPdu == nullptr) {
        TELEPHONY_LOGE("mmsPdu nullptr");
        return false;
    }
    strBuf = mmsPdu->GetMmsPdu(dbUrl);
    if (strBuf.empty()) {
        TELEPHONY_LOGE("strBuf is empty");
        return false;
    }
    return true;
}

void MmsNetworkClient::DeleteMmsPdu(const std::string &dbUrl)
{
    std::shared_ptr<MmsPersistHelper> mmsPdu = std::make_shared<MmsPersistHelper>();
    if (mmsPdu == nullptr) {
        TELEPHONY_LOGE("mmsPdu is nullptr");
        return;
    }
    mmsPdu->DeleteMmsPdu(dbUrl);
}

int32_t MmsNetworkClient::GetUrl(const std::string &mmsc, const std::string &storeDirName)
{
    std::string strResponse;
    SetIfaceName(GetIfaceName());
    int32_t getResult = HttpGet(mmsc, strResponse);
    if (getResult != CURLE_OK) {
        TELEPHONY_LOGE("LibCurl HttpGet fail");
        return getResult;
    }

    uint32_t len = strResponse.size();
    if (len > CODE_BUFFER_MAX_SIZE || len == 0) {
        TELEPHONY_LOGE("MMS pdu length invalid");
        return TELEPHONY_ERR_LOCAL_PTR_NULL;
    }

    std::unique_ptr<char[]> resultResponse = std::make_unique<char[]>(len);
    if (STORE_MMS_PDU_TO_FILE) {
        if (memset_s(resultResponse.get(), len, 0x00, len) != EOK) {
            TELEPHONY_LOGE("memset_s err");
            return TELEPHONY_ERR_MEMSET_FAIL;
        }
        if (memcpy_s(resultResponse.get(), len, &strResponse[0], len) != EOK) {
            TELEPHONY_LOGE("memcpy_s error");
            return TELEPHONY_ERR_MEMCPY_FAIL;
        }
        if (!WriteBufferToFile(std::move(resultResponse), len, storeDirName)) {
            TELEPHONY_LOGE("write to file error");
            return TELEPHONY_ERR_WRITE_DATA_FAIL;
        }
        return TELEPHONY_ERR_SUCCESS;
    } else {
        std::shared_ptr<MmsPersistHelper> mmsPduObj = std::make_shared<MmsPersistHelper>();
        if (mmsPduObj == nullptr) {
            TELEPHONY_LOGE("GetUrl mmsPduObj nullptr");
            return TELEPHONY_ERR_LOCAL_PTR_NULL;
        }
        bool ret = mmsPduObj->UpdateMmsPdu(strResponse, storeDirName);
        TELEPHONY_LOGI("ret:%{public}d, length:%{public}d", ret, len);
        return ret ? TELEPHONY_ERR_SUCCESS : TELEPHONY_ERR_FAIL;
    }
}

std::string MmsNetworkClient::GetIfaceName()
{
    int32_t simId = CoreManagerInner::GetInstance().GetSimId(slotId_);
    std::list<int32_t> netIdList;
    int32_t ret =
        NetConnClient::GetInstance().GetNetIdByIdentifier(SIMID_IDENT_PREFIX + std::to_string(simId), netIdList);
    TELEPHONY_LOGI(
        "slot = %{public}d, simId = %{public}d, netIdList size = %{public}zu", slotId_, simId, netIdList.size());
    std::string ifaceName = "";
    if (ret != NETMANAGER_SUCCESS) {
        TELEPHONY_LOGE("get netIdList by identifier fail, ret = %{public}d", ret);
        return ifaceName;
    }
    std::list<sptr<NetHandle>> netList;
    int32_t result = NetConnClient::GetInstance().GetAllNets(netList);
    if (result != NETMANAGER_SUCCESS) {
        TELEPHONY_LOGE("get all nets fail, ret = %{public}d", result);
        return ifaceName;
    }
    for (sptr<NetHandle> netHandle : netList) {
        TELEPHONY_LOGI("netHandle->GetNetId() = %{public}d", netHandle->GetNetId());
        for (auto netId : netIdList) {
            if (netId != netHandle->GetNetId()) {
                continue;
            }
            NetAllCapabilities capabilities;
            NetConnClient::GetInstance().GetNetCapabilities(*netHandle, capabilities);
            auto search = capabilities.netCaps_.find(NetCap::NET_CAPABILITY_MMS);
            if (search == capabilities.netCaps_.end()) {
                continue;
            }
            NetLinkInfo info;
            NetConnClient::GetInstance().GetConnectionProperties(*netHandle, info);
            ifaceName = info.ifaceName_;
            TELEPHONY_LOGI("data is connected ifaceName = %{public}s", ifaceName.c_str());
            return ifaceName;
        }
    }
    TELEPHONY_LOGI("slot = %{public}d data is not connected for this slot", slotId_);
    return ifaceName;
}

void MmsNetworkClient::SetIfaceName(const std::string &name)
{
    const std::string prefix("if!");
    const auto preLen = prefix.length();
    if ((name.length() > preLen && name.substr(0, preLen) == prefix) || name.empty()) {
        ifaceName_ = name;
    } else {
        ifaceName_ = prefix + name;
    }
}

int32_t MmsNetworkClient::HttpGet(const std::string &strUrl, std::string &strResponse)
{
    return HttpRequestExec(HttpReqType::HTTP_REQUEST_TYPE_GET, strUrl, "", strResponse);
}

int32_t MmsNetworkClient::HttpPost(const std::string &strUrl, const std::string &strData, std::string &strResponse)
{
    return HttpRequestExec(HttpReqType::HTTP_REQUEST_TYPE_POST, strUrl, strData, strResponse);
}

void CURLClean::operator()(CURL *p) const
{
    if (p) {
        curl_easy_cleanup(p);
    }
}

int32_t MmsNetworkClient::HttpRequestExec(
    HttpReqType type, const std::string &strUrl, const std::string &strData, std::string &strResponse)
{
    if (strUrl.empty() || strUrl.length() > URL_SIZE) {
        TELEPHONY_LOGE("URL error!");
        return -1;
    }

    std::unique_ptr<CURL, CURLClean> mmsCurl(curl_easy_init(), CURLClean());
    if (mmsCurl.get() == nullptr) {
        TELEPHONY_LOGE("mmsCurl nullptr");
        return -1;
    }

    int32_t result = SetCurlOpt(mmsCurl, type, strUrl, strData, strResponse);
    if (result == 0) {
        CURLcode errCode = CURLE_OK;
        TELEPHONY_LOGI("HttpRequestExec send http request");
        errCode = curl_easy_perform(mmsCurl.get());
        result = static_cast<int32_t>(errCode);
    }
    if (mmsHttpHeaderlist_ != nullptr) {
        curl_slist_free_all(mmsHttpHeaderlist_);
    }
    return ParseExecResult(mmsCurl, result);
}

int32_t MmsNetworkClient::ParseExecResult(const std::unique_ptr<CURL, CURLClean> &mmsCurl, int32_t result)
{
    CURLcode retCode = static_cast<CURLcode>(result);
    if (retCode != CURLE_OK) {
        TELEPHONY_LOGE("HTTP request failed, errStr:[%{public}s], errorBuffer_:[%{public}s]!",
            curl_easy_strerror(retCode), errorBuffer_);
        return result;
    }

    lastTransTime_ = 0;
    curl_off_t totalTimeUs = 0L;
    retCode = curl_easy_getinfo(mmsCurl.get(), CURLINFO_TOTAL_TIME_T, &totalTimeUs);
    if (retCode == CURLE_OK) {
        lastTransTime_ = static_cast<int64_t>(totalTimeUs / 1000L);
        TELEPHONY_LOGI("HTTP request OK,total time in ms:[%{public}" PRId64 "]", lastTransTime_);
    }
    return static_cast<int32_t>(retCode);
}

int32_t MmsNetworkClient::SetCurlOpt(const std::unique_ptr<CURL, CURLClean> &mmsCurl, HttpReqType type,
    const std::string &strUrl, const std::string &strData, std::string &strResponse)
{
    int32_t rlt = SetCurlOptCommon(mmsCurl, strUrl);
    if (rlt != 0) {
        return rlt;
    }

    /* receive the http header */
    curl_easy_setopt(mmsCurl.get(), CURLOPT_HEADERFUNCTION, nullptr);
    curl_easy_setopt(mmsCurl.get(), CURLOPT_HEADERDATA, nullptr);

    /* receive the whole http response */
    strResponse.clear();
    curl_easy_setopt(mmsCurl.get(), CURLOPT_WRITEFUNCTION, DataCallback);
    curl_easy_setopt(mmsCurl.get(), CURLOPT_WRITEDATA, &strResponse);
    curl_easy_setopt(mmsCurl.get(), CURLOPT_FAILONERROR, 1L);
    curl_easy_setopt(mmsCurl.get(), CURLOPT_LOW_SPEED_LIMIT, 1);
    curl_easy_setopt(mmsCurl.get(), CURLOPT_LOW_SPEED_TIME, LOW_DOWNLOAD_RATE);
    if (type == HttpReqType::HTTP_REQUEST_TYPE_POST) {
        /* Specify post content */
        curl_easy_setopt(mmsCurl.get(), CURLOPT_POST, 1L);
        curl_easy_setopt(mmsCurl.get(), CURLOPT_POSTFIELDSIZE, strData.length());
        curl_easy_setopt(mmsCurl.get(), CURLOPT_POSTFIELDS, strData.c_str());

        mmsHttpHeaderlist_ =
            curl_slist_append(mmsHttpHeaderlist_, "Content-Type:application/vnd.wap.mms-message; charset=utf-8");
        mmsHttpHeaderlist_ =
            curl_slist_append(mmsHttpHeaderlist_, "Accept:application/vnd.wap.mms-message, application/vnd.wap.sic");
        curl_easy_setopt(mmsCurl.get(), CURLOPT_HTTPHEADER, mmsHttpHeaderlist_);
    }
    return 0;
}

int32_t MmsNetworkClient::SetCurlOptCommon(const std::unique_ptr<CURL, CURLClean> &mmsCurl, const std::string &strUrl)
{
    /* Print request connection process and return http data on the screen */
    curl_easy_setopt(mmsCurl.get(), CURLOPT_VERBOSE, 0L);

    curl_easy_setopt(mmsCurl.get(), CURLOPT_ERRORBUFFER, errorBuffer_);
    /* not include the headers in the write callback */
    curl_easy_setopt(mmsCurl.get(), CURLOPT_HEADER, 0L);
    /* Specify url content */
    if (memset_s(mmscChar_, MAX_MMSC_SIZE, 0x00, MAX_MMSC_SIZE) != EOK) {
        TELEPHONY_LOGE("set mmsc memset_s err");
        return CURLE_FAILED_INIT;
    }
    if (memcpy_s(mmscChar_, strUrl.length(), &strUrl[0], strUrl.length()) != EOK) {
        TELEPHONY_LOGE("set mmsc memcpy_s err");
        return CURLE_FAILED_INIT;
    }
    curl_easy_setopt(mmsCurl.get(), CURLOPT_URL, mmscChar_);

    /* https support */
    /* the connection succeeds regardless of the peer certificate validation */
    curl_easy_setopt(mmsCurl.get(), CURLOPT_SSL_VERIFYPEER, 0L);
    /* the connection succeeds regardless of the names in the certificate. */
    curl_easy_setopt(mmsCurl.get(), CURLOPT_SSL_VERIFYHOST, 0L);

    curl_easy_setopt(mmsCurl.get(), CURLOPT_NOSIGNAL, 1L);

    /* Allow redirect */
    curl_easy_setopt(mmsCurl.get(), CURLOPT_FOLLOWLOCATION, 1L);
    /* Set the maximum number of subsequent redirects */
    curl_easy_setopt(mmsCurl.get(), CURLOPT_MAXREDIRS, 1L);

    /* connection timeout time */
    curl_easy_setopt(mmsCurl.get(), CURLOPT_CONNECTTIMEOUT_MS, connectionTimeout_);
    /* transfer operation timeout time */
    curl_easy_setopt(mmsCurl.get(), CURLOPT_TIMEOUT_MS, transOpTimeout_);

    std::shared_ptr<MmsApnInfo> mmsApnInfo = std::make_shared<MmsApnInfo>(slotId_);
    if (mmsApnInfo == nullptr) {
        TELEPHONY_LOGE("mmsApnInfo is nullptr");
        return CURLE_FAILED_INIT;
    }

    std::string proxy = mmsApnInfo->getMmsProxyAddressAndProxyPort();
    if (proxy.empty() || static_cast<int32_t>(proxy.length()) > MAX_MMSC_PROXY_SIZE) {
        return static_cast<int32_t>(CURLE_BAD_FUNCTION_ARGUMENT);
    }
    if (memset_s(proxyChar_, MAX_MMSC_PROXY_SIZE, 0x00, MAX_MMSC_PROXY_SIZE) != EOK) {
        TELEPHONY_LOGE("set mmsc proxy memset_s err");
        return CURLE_FAILED_INIT;
    }
    if (memcpy_s(proxyChar_, proxy.length(), &proxy[0], proxy.length()) != EOK) {
        TELEPHONY_LOGE("set mmsc proxy memcpy_s err");
        return CURLE_FAILED_INIT;
    }
    curl_easy_setopt(mmsCurl.get(), CURLOPT_PROXY, proxyChar_);
    curl_easy_setopt(mmsCurl.get(), CURLOPT_PROXYTYPE, CURLPROXY_HTTP);

    CURLcode errCode = CURLE_OK;
    if (!ifaceName_.empty()) {
        errCode = curl_easy_setopt(mmsCurl.get(), CURLOPT_INTERFACE, ifaceName_.c_str());
        if (errCode != CURLE_OK) {
            TELEPHONY_LOGE("CURLOPT_INTERFACE failed errCode:%{public}d", errCode);
        }
    }
    return static_cast<int32_t>(errCode);
}

int32_t MmsNetworkClient::DataCallback(const std::string &data, size_t size, size_t nmemb, std::string *strBuffer)
{
    if (strBuffer == nullptr || size == 0) {
        return 0;
    }

    int32_t writtenLen = static_cast<int32_t>(size * nmemb);
    strBuffer->append(data, writtenLen);
    return writtenLen;
}

bool MmsNetworkClient::WriteBufferToFile(
    const std::unique_ptr<char[]> &buff, uint32_t len, const std::string &strPathName) const
{
    if (buff == nullptr) {
        TELEPHONY_LOGE("buff nullptr");
        return false;
    }
    char realPath[PATH_MAX] = { 0 };
    if (strPathName.empty() || realpath(strPathName.c_str(), realPath) == nullptr) {
        TELEPHONY_LOGE("path or realPath is nullptr");
        return false;
    }
    FILE *pFile = nullptr;
    pFile = fopen(realPath, "wb");
    if (!pFile) {
        TELEPHONY_LOGE("open file fail");
        return false;
    }
    uint32_t fileLen = fwrite(buff.get(), len, 1, pFile);
    if (fileLen == 0) {
        TELEPHONY_LOGI("write mms buffer to file error");
        (void)fclose(pFile);
        return false;
    }
    (void)fclose(pFile);
    return true;
}
} // namespace Telephony
} // namespace OHOS

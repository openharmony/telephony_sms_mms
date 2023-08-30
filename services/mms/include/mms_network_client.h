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

#ifndef MMS_NETWORK_CLIENT_H
#define MMS_NETWORK_CLIENT_H

#include <curl/curl.h>
#include <curl/easy.h>

#include <cinttypes>
#include <cmath>
#include <memory>
#include <string>

namespace OHOS {
namespace Telephony {
enum class HttpReqType {
    HTTP_REQUEST_TYPE_GET,
    HTTP_REQUEST_TYPE_POST,
};
class CURLClean {
public:
    void operator()(CURL *p) const;
};

class MmsNetworkClient {
public:
    explicit MmsNetworkClient(int32_t slotId);
    virtual ~MmsNetworkClient();
    int32_t Execute(const std::string &method, const std::string &mmsc, const std::string &data);
    void InitLibCurl();
    void DestoryLibCurl();

private:
    void SetIfaceName(const std::string &ifaceName);
    std::string GetIfaceName();
    int32_t HttpPost(const std::string &strUrl, const std::string &strData, std::string &strResponse);
    int32_t HttpGet(const std::string &strUrl, std::string &strResponse);
    int32_t HttpRequestExec(
        HttpReqType type, const std::string &strUrl, const std::string &strData, std::string &strResponse);
    int32_t ParseExecResult(const std::unique_ptr<CURL, CURLClean> &mmsCurl, int32_t result);
    int32_t PostUrl(const std::string &mmsc, const std::string &filename);
    int32_t GetUrl(const std::string &mmsc, const std::string &storeDirName);
    int32_t SetCurlOpt(const std::unique_ptr<CURL, CURLClean> &mmsCurl, HttpReqType type, const std::string &strUrl,
        const std::string &strData, std::string &strResponse);
    int32_t SetCurlOptCommon(const std::unique_ptr<CURL, CURLClean> &mmsCurl, const std::string &strUrl);
    static int32_t DataCallback(const std::string &data, size_t size, size_t nmemb, std::string *strBuffer);
    bool WriteBufferToFile(const std::unique_ptr<char[]> &buff, uint32_t len, const std::string &strPathName) const;
    void DeleteMmsPdu(const std::string &dbUrl);
    bool GetMmsPduFromFile(const std::string &fileName, std::string &strBuf);
    bool GetMmsPduFromDataBase(const std::string &dbUrl, std::string &strBuf);

private:
    static constexpr int32_t DEFAULT_ERROR_SIZE = 256;
    static constexpr int32_t MAX_MMSC_SIZE = 50;
    static constexpr int32_t MAX_MMSC_PROXY_SIZE = 50;
    char errorBuffer_[DEFAULT_ERROR_SIZE] = { 0 };
    char mmscChar_[MAX_MMSC_SIZE] = { 0 };
    char proxyChar_[MAX_MMSC_PROXY_SIZE] = { 0 };
    int64_t connectionTimeout_ = 0;
    int64_t transOpTimeout_ = 0;
    int64_t lastTransTime_ = 0;
    std::string ifaceName_;
    int32_t slotId_ = -1;
    struct curl_slist *mmsHttpHeaderlist_ = nullptr;
};
} // namespace Telephony
} // namespace OHOS
#endif
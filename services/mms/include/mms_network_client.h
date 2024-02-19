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

#include <cinttypes>
#include <cmath>
#include <memory>
#include <string>

#include "http_client_task.h"

namespace OHOS {
namespace Telephony {
class MmsNetworkClient {
public:
    explicit MmsNetworkClient(int32_t slotId);
    virtual ~MmsNetworkClient();
    int32_t Execute(const std::string &method, const std::string &mmsc, std::string &data);

private:
    std::string GetIfaceName();
    int32_t PostUrl(const std::string &mmsc, const std::string &filename);
    int32_t GetMmscFromDb(const std::string &mmsc);
    int32_t GetMmsDataBuf(std::string &strBuf, const std::string &fileName);
    int32_t GetUrl(const std::string &mmsc, std::string &storeDirName);
    int32_t HttpRequest(const std::string &method, const std::string &url, const std::string &data);
    int32_t GetMmsApnPorxy(NetStack::HttpClient::HttpProxy &httpProxy);
    bool WriteBufferToFile(const std::unique_ptr<char[]> &buff, uint32_t len, std::string &strPathName) const;
    void DeleteMmsPdu(const std::string &dbUrl);
    bool GetMmsPduFromFile(const std::string &fileName, std::string &strBuf);
    bool GetMmsPduFromDataBase(const std::string &dbUrl, std::string &strBuf);
    void HttpCallBack(std::shared_ptr<NetStack::HttpClient::HttpClientTask> task);
    int32_t UpdateMmsPduToStorage(std::string &storeDirName);

public:
    bool httpFinish_ = false;
    bool httpSuccess_ = false;
    std::mutex clientCts_;
    std::condition_variable clientCv_;
    std::string responseData_;

private:
    int32_t slotId_ = -1;
};
} // namespace Telephony
} // namespace OHOS
#endif
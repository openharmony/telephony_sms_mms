/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "addsmstoken_fuzzer.h"

#include <iostream>

#include "nativetoken_kit.h"
#include "token_setproc.h"

namespace OHOS {
AddSmsTokenFuzzer::AddSmsTokenFuzzer()
{
    const char **perms = new const char *[4];
    perms[0] = "ohos.permission.SEND_MESSAGES";
    perms[1] = "ohos.permission.RECEIVE_SMS";
    perms[2] = "ohos.permission.SET_TELEPHONY_STATE";
    perms[3] = "ohos.permission.GET_TELEPHONY_STATE";

    NativeTokenInfoParams testMmsInfoParams = {
        .dcapsNum = 0,
        .permsNum = 4,
        .aclsNum = 0,
        .dcaps = nullptr,
        .perms = perms,
        .acls = nullptr,
        .processName = "sms_fuzzer",
        .aplStr = "system_basic",
    };
    currentID_ = GetAccessTokenId(&testMmsInfoParams);
    std::cout << "AddSmsTokenFuzzer currentID_ : " << currentID_ << std::endl;
    SetSelfTokenID(currentID_);
    Security::AccessToken::AccessTokenKit::ReloadNativeTokenInfo();
}
AddSmsTokenFuzzer::~AddSmsTokenFuzzer()
{
    std::cout << "AddSmsTokenFuzzer ~AddSmsTokenFuzzer" << std::endl;
}
} // namespace OHOS
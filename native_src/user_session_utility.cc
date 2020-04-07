#include "../headers/user_session_utility.h"

using namespace remoting;

UserSessionUtility::UserSessionUtility()
{
}

std::vector<utmpx> UserSessionUtility::GetUserInfoFromUTMPRegister()
{
    struct utmpx *UtmpxPtr = nullptr;
    setutxent();
    std::vector<utmpx> userSessionInfoList;
    while ((UtmpxPtr = getutxent()) != nullptr)
    {
        if (UtmpxPtr->ut_type != USER_PROCESS)
        {
            continue;
        }
        utmpx userSessionInfo;
        userSessionInfo = *UtmpxPtr;
        userSessionInfoList.push_back(userSessionInfo);
    }

    endutxent();
    return userSessionInfoList;
}

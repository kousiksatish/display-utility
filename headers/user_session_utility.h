#ifndef REMOTING_HOST_USER_SESSION_UTILITY_H_
#define REMOTING_HOST_USER_SESSION_UTILITY_H_

#include<utmpx.h>
#include<vector>

namespace remoting
{
class UserSessionUtility
{
public:
    UserSessionUtility();
    std::vector<utmpx> GetUserInfoFromUTMPRegister();
}; 
} // namespace remoting

#endif // REMOTING_HOST_SCREEN_CAPTURE_UTILITY_H_
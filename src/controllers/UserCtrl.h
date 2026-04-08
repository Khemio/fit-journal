#pragma once

#include <drogon/HttpController.h>

using namespace drogon;

//TODO: Remove users

class Users : public drogon::HttpController<Users>
{
    public:
        METHOD_LIST_BEGIN
        METHOD_ADD(Users::Auth, "/auth", Get);  //? Consider better flow for auth
        METHOD_ADD(Users::SignUp, "/signup", Post);
        METHOD_ADD(Users::SignIn, "/signin", Post);
        METHOD_ADD(Users::SignOut, "/signout", Post);
        // METHOD_ADD(Users::DeleteUser, "/delete", Delete);
        METHOD_LIST_END

        void Auth(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
        void SignOut(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback);
        Task<HttpResponsePtr> SignUp(HttpRequestPtr req);
        Task<HttpResponsePtr> SignIn(HttpRequestPtr req);
        // Task<HttpResponsePtr> DeleteUser(HttpRequestPtr req);
};
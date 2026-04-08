#include "UserCtrl.h"
#include "bcrypt-hash.h"

#include "../models/user.h"

std::string hash(std::string pwd) {
    std::string seed = "jadsjhdsajkh";

    return utils::getMd5(seed + pwd);
}

//? The better way of doing this might be through conditional templates
void Users::Auth(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    bool loggedIn = req->session()->getOptional<bool>("loggedIn").value_or(false);
    HttpResponsePtr resp;
    if (loggedIn)
    {
        resp = HttpResponse::newHttpViewResponse("LogoutPage");
    } else {
        resp = HttpResponse::newHttpViewResponse("LoginPage");
    }
    callback(resp);
}

Task<HttpResponsePtr> Users::SignIn(HttpRequestPtr req) {
// void Users::SignIn(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    HttpResponsePtr resp = HttpResponse::newHttpResponse();
    std::string user = req->getParameter("user");
    std::string passwd = req->getParameter("passwd");

    auto client = app().getDbClient();
    auto result = co_await client->execSqlCoro("SELECT * FROM users WHERE username = ?;", user);

    if (result.size() > 0) { //? Size probably should be 1 for a singular row
        User user{
            .ID = result[0][0].as<unsigned long>(),
            .username = result[0][1].as<std::string>(),
            // .password = result[0][2].as<std::string>()
            // .password = hash(result[0][2].as<std::string>()) // Simulating hashed password from DB
            .password = bcrypt::generateHash(result[0][2].as<std::string>()) // Simulating hashed password from DB
        };

        if (bcrypt::validatePassword(passwd,user.password))
        {
            req->session()->insert("loggedIn", true);
            resp->setBody("<script>window.location.href = \"/\";</script>");
        } 
    }else {
        resp->setStatusCode(k401Unauthorized);
        resp->setBody("<script>window.location.href = \"/\";</script>");
    }
    co_return resp;
}

void Users::SignOut(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    HttpResponsePtr resp = HttpResponse::newHttpResponse();
    req->session()->erase("loggedIn");
    resp->setBody("<script>window.location.href = \"/\";</script>");
    callback(resp);
}
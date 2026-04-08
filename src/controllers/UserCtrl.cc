#include "UserCtrl.h"
#include "bcrypt-hash.h"

#include "../models/user.h"

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

Task<HttpResponsePtr> Users::SignUp(HttpRequestPtr req) {
    HttpResponsePtr resp = HttpResponse::newHttpResponse();

    std::string user = req->getParameter("user");
    std::string passwd = req->getParameter("passwd");

    auto client = app().getDbClient();

    try {
        auto result = co_await client->execSqlCoro(
            "INSERT INTO users (username, password) VALUES ($1, $2);", 
            user, bcrypt::generateHash(passwd));

        resp->setStatusCode(k201Created);
        resp->setBody("<script>window.location.href = \"/\";</script>");

    } catch (const orm::SqlError &e) {
        if (e.extendedErrcode() == 2067) {
            resp->setStatusCode(k409Conflict);
            resp->setBody("<script>window.location.href = \"/\";alert('Username taken');</script>");
            co_return resp;
        }
        resp->setStatusCode(k500InternalServerError);
        resp->setBody("<script>window.location.href = \"/\";alert('Something went wrong');</script>");
    }

    co_return resp;
}

Task<HttpResponsePtr> Users::SignIn(HttpRequestPtr req) {
    HttpResponsePtr resp = HttpResponse::newHttpResponse();
    std::string user = req->getParameter("user");
    std::string passwd = req->getParameter("passwd");

    auto client = app().getDbClient();
    auto result = co_await client->execSqlCoro("SELECT * FROM users WHERE username = ?;", user);

    if (result.size() > 0) { //? Size probably should be 1 for a singular row
        User user{
            .ID = result[0][0].as<unsigned long>(),
            .username = result[0][1].as<std::string>(),
            .password = result[0][2].as<std::string>()
            // .password = bcrypt::generateHash(result[0][2].as<std::string>()) // Simulating hashed password from DB
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
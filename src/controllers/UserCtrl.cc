#include "UserCtrl.h"
#include "bcrypt-hash.h"

#include "../dto/user.h"

 //TODO: Add better error messaging for client because HTMX ignores body for 4XX and 5XX responses

//? The better way of doing this might be through conditional templates
void Users::Auth(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string page) {
    HttpResponsePtr resp;
    HttpViewData data;

    bool loggedIn = req->session()->getOptional<bool>("loggedIn").value_or(false);
    if (loggedIn)
    {
        auto username = req->session()->getOptional<std::string>("username").value_or("");
        data.insert("username", username);
        resp = HttpResponse::newHttpViewResponse("LogoutPage", data);
    } else {
        data.insert("page", page);
        resp = HttpResponse::newHttpViewResponse("LoginPage", data);
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
        resp->addHeader("HX-Location", "/");

    } catch (const orm::SqlError &e) {
        if (e.extendedErrcode() == 2067) {
            resp->setStatusCode(k409Conflict);
            resp->addHeader("HX-Location", "/");

            co_return resp;
        }
        resp->setStatusCode(k500InternalServerError);
        resp->addHeader("HX-Location", "/");
    }

    co_return resp;
}

//TODO: Invalid logins seem to be working, look into it
Task<HttpResponsePtr> Users::SignIn(HttpRequestPtr req) {

    HttpResponsePtr resp = HttpResponse::newHttpResponse();
    std::string user = req->getParameter("user");
    std::string passwd = req->getParameter("passwd");

    std::cout << "user: " << user << std::endl;
    std::cout << "password: " << passwd << std::endl;

    auto client = app().getDbClient();
    auto result = co_await client->execSqlCoro("SELECT * FROM users WHERE username = ?;", user);

    if (result.size() > 0) { //? Size probably should be 1 for a singular row
        User user{result[0]};

        if (bcrypt::validatePassword(passwd,user.password))
        {
            req->session()->insert("loggedIn", true);
            req->session()->insert("ID", user.ID);
            req->session()->insert("username", user.username);
            resp->addHeader("HX-Location", "{\"path\":\"/journals\", \"target\":\"main\"}");

        } 
    }else {
        resp->setStatusCode(k401Unauthorized);
        resp->addHeader("HX-Location", "/");
    }
    co_return resp;
}

void Users::SignOut(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback) {
    HttpResponsePtr resp = HttpResponse::newHttpResponse();
    req->session()->erase("loggedIn");
    req->session()->erase("ID");
    req->session()->erase("username");
    resp->addHeader("HX-Location", "/");

    callback(resp);
}
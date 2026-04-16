#include "UserCtrl.h"
#include "bcrypt-hash.h"

#include "../dto/user.h"

//? The better way of doing this might be through conditional templates
void Users::Auth(const HttpRequestPtr &req, std::function<void(const HttpResponsePtr &)> &&callback, const std::string page) {
    HttpResponsePtr resp;
    HttpViewData data;

    // std::cout << "qwerty: " << bcrypt::generateHash("qwerty") << std::endl;
    // std::cout << "password123: " << bcrypt::generateHash("password123") << std::endl;

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
        // resp->setBody("<script>window.location.href = \"/\";</script>");

    } catch (const orm::SqlError &e) {
        //TODO: Add better error messaging for client because HTMX ignores body for 4XX and 5XX responses
        if (e.extendedErrcode() == 2067) {
            resp->setStatusCode(k409Conflict);
            resp->addHeader("HX-Location", "/");
            // resp->setBody("<script>window.location.href = \"/\";alert('Username taken');</script>");
            co_return resp;
        }
        resp->setStatusCode(k500InternalServerError);
        resp->addHeader("HX-Location", "/");
        // resp->setBody("<script>window.location.href = \"/\";alert('Something went wrong');</script>");
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
            req->session()->insert("ID", user.ID);
            req->session()->insert("username", user.username);
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
    req->session()->erase("ID");
    req->session()->erase("username");
    resp->setBody("<script>window.location.href = \"/\";</script>");
    callback(resp);
}
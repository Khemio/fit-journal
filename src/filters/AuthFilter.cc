#include "AuthFilter.h"

using namespace drogon;

void AuthFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    //Edit your logic here
    bool loggedIn = req->session()->getOptional<bool>("loggedIn").value_or(false);
    if (loggedIn)
    {
        fccb();
        return;
    }
    //Check failed  
    // auto res = drogon::HttpResponse::newHttpResponse();
    // res->setStatusCode(k500InternalServerError);
    // auto resp = HttpResponse::newHttpViewResponse("LoginPage");
    HttpResponsePtr resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k401Unauthorized);
    resp->setBody("<script>window.location.href = \"/\";</script>");
    fcb(resp);
}

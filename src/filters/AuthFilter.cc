#include "AuthFilter.h"

using namespace drogon;

void AuthFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    bool loggedIn = req->session()->getOptional<bool>("loggedIn").value_or(false);
    if (loggedIn)
    {
        fccb();
        return;
    }
    HttpResponsePtr resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k401Unauthorized);
    // resp->setBody("<script>window.location.href = \"/\";</script>");
    resp->addHeader("HX-Location", "/");
    
    fcb(resp);
}

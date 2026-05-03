#include "AuthFilter.h"

using namespace drogon;

class AuthService {
    public:
        virtual void authenticate() {};
        virtual void authorize() {};
};

class SessionAuthService : public AuthService {
    public:
        SessionAuthService(const HttpRequestPtr req) : req(req) {};
        void authenticate() override {
            isAuthenticated =  req->session()->getOptional<bool>("loggedIn").value_or(false);
        }
        virtual void authorize() override {isValid = isAuthenticated;};

        bool isValid;

    private:
        bool isAuthenticated;
        const HttpRequestPtr req;
};

class AuthServiceFactory {
    public:
        static SessionAuthService getSessionAuthService(const HttpRequestPtr req) {
            return SessionAuthService{req};
        };
};

void AuthFilter::doFilter(const HttpRequestPtr &req,
                         FilterCallback &&fcb,
                         FilterChainCallback &&fccb)
{
    auto auth = AuthServiceFactory::getSessionAuthService(req);
    auth.authenticate();
    auth.authorize();
    if (auth.isValid)
    {
        fccb();
        return;
    }
    HttpResponsePtr resp = HttpResponse::newHttpResponse();
    resp->setStatusCode(k401Unauthorized);
    resp->addHeader("HX-Location", "/");
    
    fcb(resp);
}

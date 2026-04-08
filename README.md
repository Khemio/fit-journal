# Fit Journal

This is a health and fitness journaling web application. It supports multiple users and allows tracking of food intake and simple activity.

## Thecnical Info

This is a server driven application, made with C++ framework Drogon. Front End is server rendered with Drogon CSP (C++ Server Pages) and styled with Pico CSS. Interactivity and server communication achieved with HTMX. Database of choice is SQLite.

## Dependencies

- Drogon Framework and it's dependencies
- Bcrypt.cpp (vendored, changed some names to avoid collision with Windows "bcrypt.h")
- HTMX (vendored)
- Pico CSS (vendored)

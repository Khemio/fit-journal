# Fit Journal

A multi-user food journaling web application built with **C++23**, **Drogon**, **SQLite**, **HTMX**, and **Pico CSS**. Fit Journal lets users maintain daily food logs, search a food catalog, and track protein and calorie totals without a client-side JavaScript application framework.

> **Project status:** Functional prototype / learning project. The food-journal workflow is implemented, but the application has known authorization and security issues and is **not ready for public deployment**. See [Known limitations](docs/LIMITATIONS.md).

## Features

- **Accounts and sessions:** Sign up, sign in, and sign out. Passwords are stored as bcrypt hashes; signed-in state is maintained using Drogon sessions.
- **Food journals:** Each newly registered user receives a food journal, along with inactive placeholders for potential life and sport journals. Only active journals appear in the journal list.
- **Daily entries:** Create dated entries in a food journal and view them in reverse chronological order.
- **Food lookup:** Browse the built-in catalog, filter foods by name, and select an item.
- **Nutrition logging:** Add foods to an entry using an amount multiplier; logged items store their own quantity, protein, and calorie values.
- **Automatic totals:** SQLite insert/delete triggers maintain total protein and calories for each daily entry.
- **Server-driven interface:** Drogon CSP templates render HTML; HTMX requests replace selected page fragments as users interact.

**Not implemented in the current repository:** editing existing entries or foods, creating foods through a user-facing form, calculating averages, and functional life/sport journals. These are possible extensions, not existing features.

## Screenshots

Screenshots have not yet been added to the repository. Recommended captures are the sign-in page, journal overview, daily food-entry view, and searchable food catalog. Add them under `docs/screenshots/` before inserting image references here.

## Technology

| Component | Technology | Purpose |
| --- | --- | --- |
| Server | C++23 + [Drogon](https://github.com/drogonframework/drogon) | HTTP routing, coroutines, sessions, templates, ORM/database client |
| Views | Drogon CSP (C++ Server Pages) | Render HTML on the server |
| Interactions | [HTMX](https://htmx.org/) | Request and swap HTML fragments |
| Styling | [Pico CSS](https://picocss.com/) + small custom stylesheet | Lightweight responsive UI |
| Storage | SQLite | Users, journals, food entries, and food catalog |
| Password hashing | Vendored `Bcrypt.cpp` | Hash and verify account passwords |
| Build | CMake | Compile C++ sources and generate CSP views |

## Building locally

The original project was developed using **MSYS2 UCRT on Windows**, with Drogon built from source. The instructions below describe the expected build process, not a verified one-command installation for every platform.

### Prerequisites

- C++ compiler and standard library supporting C++23
- CMake (project declares a minimum of 3.10)
- Drogon installed so `find_package(Drogon CONFIG REQUIRED)` succeeds, with SQLite support and its required libraries
- Git

### Build and run

```bash
git clone https://github.com/Khemio/fit-journal.git
cd fit-journal
cmake -S . -B build
cmake --build build --parallel
cd build
./journal
```

On Windows, the executable may be `journal.exe`. **Run the executable from the `build/` directory**: `src/main.cc` loads `../config.json`, and `config.json` uses paths relative to the current working directory.

Open **http://localhost:5555**. The current configuration listens on `0.0.0.0:5555` over HTTP, uses a SQLite database at `build/journal.db` when run from `build/`, and has a single database connection.

On first startup, the `DBInit` plugin creates tables and triggers and seeds the food catalog. **On every startup it deletes and reinserts all catalog foods**, so food IDs may change and manual catalog modifications are not preserved. Journal-entry items retain their saved nutritional snapshots.

### Docker (experimental)

A `Dockerfile` and a previously published [Docker Hub image](https://hub.docker.com/r/khemio/fitness-journal) are present, but the checked-in Dockerfile has a likely incorrect `cp` destination in its build step and has not been verified for this documentation. After correcting and validating it, the intended commands are:

```bash
docker build -t fit-journal .
docker run --rm -p 5555:5555 fit-journal
```

The image does **not** configure persistent storage for `journal.db`; deleting the container can delete journal data. Do not treat this as a production deployment recipe.

## How it works

1. Register an account and sign in.
2. Open **Journals** and select the active food journal.
3. Create a dated entry.
4. Open the entry, search or select a catalog food, and provide an amount multiplier.
5. Add the food. The item is saved with its calculated quantity, protein, and calories. SQL triggers update entry totals.
6. Delete an item to have the totals adjusted automatically.

The interface returns HTML pages and fragments, not a conventional JSON REST API. Responses also use HTMX headers such as `HX-Location` and `HX-Trigger`.

## Project structure

```text
fit-journal/
├── CMakeLists.txt           # Build configuration, CSP generation
├── Dockerfile               # Experimental container build
├── config.json              # Drogon listener, DB, session and plugin settings
├── sql/
│   ├── schema.sql           # SQL schema reference (not executed directly on startup)
│   └── test*.sql            # Sample SQL / seed data
├── src/
│   ├── main.cc              # Loads config, enables sessions, starts Drogon
│   ├── controllers/         # User, journal and food HTTP handlers
│   ├── filters/             # Session-based authentication filter
│   ├── database/            # Food-catalog query helper
│   ├── dto/                 # Lightweight row-to-C++ structures
│   ├── plugins/             # Database initialization and seeding
│   ├── views/               # CSP templates
│   └── models/              # Unused/example model-generator configuration
├── static/                  # HTML shell, CSS and client-side assets
├── vendor/                  # bcrypt, HTMX and Pico CSS sources/assets
└── test/                    # Drogon test scaffold, disabled in main CMake build
```

## Further documentation

- [Architecture and data model](docs/ARCHITECTURE.md) — request flow, modules, SQLite schema, triggers, design choices
- [HTTP routes](docs/HTTP_ROUTES.md) — actual controller routes, request parameters, responses and HTMX behavior
- [Development notes](docs/DEVELOPMENT.md) — build configuration, startup behavior, testing and troubleshooting
- [Known limitations](docs/LIMITATIONS.md) — verified gaps, security issues and operational caveats
- [Development roadmap](docs/ROADMAP.md) — prioritized fixes, improvements, acceptance criteria and release gates

## Notes

This repository does not currently contain a top-level license for the application itself. The vendored `Bcrypt.cpp` code has its own license. Clarify the application's license before inviting reuse or contributions.

Documentation is based on the `master` branch reviewed **13 November 2026**.
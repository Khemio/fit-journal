# Development guide

This document describes the current build/startup flow and a practical route for continuing the project. It is not a claim that the repository has been built and tested on every operating system.

## Toolchain and dependencies

The repository uses CMake, requires C++23, and finds Drogon via `find_package(Drogon CONFIG REQUIRED)`. The bcrypt implementation is built from its vendored `vendor/Bcrypt.cpp` subdirectory. CSP views are compiled by `drogon_create_views` into generated C++ sources. HTMX and Pico CSS are checked in as static assets.

The author originally built the project on Windows with MSYS2 UCRT and Drogon built from source. On other platforms, provide a working Drogon install with SQLite support, set `CMAKE_PREFIX_PATH` if necessary, then run:

```bash
cmake -S . -B build
cmake --build build --parallel
cd build
./journal
```

If Drogon is installed in a non-standard location, configure with a path such as:

```bash
cmake -S . -B build -DCMAKE_PREFIX_PATH=/path/to/drogon/install
```

The executable expects to be launched from `build/`, because `src/main.cc` calls `app().loadConfigFile("../config.json")`. The CMake build copies `static/` to `build/static/` after compiling.

## Configuration

`config.json` currently specifies:

| Setting | Checked-in value | Notes |
| --- | --- | --- |
| Listener | `0.0.0.0:5555` | Public interface binding, **HTTP only** (`https: false`) |
| Database | SQLite `journal.db` | Relative to the working directory, normally `build/` |
| DB connections | `1` | Important for the current per-connection PRAGMA initialization |
| App threads | `1` | Configured for simple prototype use |
| Sessions | Enabled by `main.cc` | `config.json` also contains `enable_session: false`; the explicit `enableSession(12000)` call is relevant |
| Static root | `./static` | Relative to process working directory |
| Plugins | `DBInit`, Prometheus exporter, access logger | `DBInit` creates schema and reseeds catalog |

Never expose the development HTTP configuration or test account credentials to a public network. Review logging and session-cookie settings before deploying.

## Database bootstrap

On every launch, `DBInit::initAndStart()` calls these functions in order:

1. `init_db_settings()` — runs `PRAGMA foreign_keys = ON` on the configured database client.
2. `init_tables()` — issues `CREATE TABLE/TRIGGER IF NOT EXISTS` statements.
3. `init_foods()` — **executes `DELETE FROM foods` and inserts sample catalog rows**.

Two schema definitions exist: `sql/schema.sql` and the embedded statements in `DBInit.cc`. The application runs the embedded version; editing only `sql/schema.sql` will **not** change runtime initialization. A migration mechanism or single schema source would remove this duplication.

Because food rows are deleted and reinserted at startup, catalog IDs are unstable. Existing `food_entry_items` rows are snapshots and do not hold foreign keys back to `foods`, so they retain their values.

## Current tests

There is a `test/` directory with a Drogon test-main scaffold and one empty `BasicTest` test. The top-level `CMakeLists.txt` has `# add_subdirectory(test)` commented out, so these tests are **not enabled as part of the normal build**. The repository does not currently provide meaningful automated coverage for authentication, ownership, journal CRUD, or trigger behavior.

Suggested initial test coverage:

- Creating a user adds exactly the expected journal rows; duplicate usernames fail.
- Sign-in succeeds only with a valid bcrypt password and returns a consistent error for invalid credentials.
- Users cannot read or mutate another user's journal/entry/item IDs.
- Inserting or deleting an item changes totals by exactly the item's stored nutritional values.
- Updating an item (once supported) recomputes totals correctly.
- Adding a second entry on the same date is rejected **after** a date-uniqueness rule is introduced.
- Invalid amounts and dates return controlled validation errors.
- Restarting the app preserves catalog records **after** destructive seeding has been removed.

## Docker notes

The checked-in `Dockerfile` uses `drogonframework/drogon:latest`, compiles the application and exposes 5555, but does not define a runtime data volume. Its last build-step command is `cp -r ../src/views build/` while the working directory is `/app/build`; that destination is probably wrong and should be checked before relying on `docker build`.

For a robust container build, remove the unnecessary/misplaced view-copy operation if CSP files are already compiled, verify config and static asset paths, pin the base-image version, and mount a directory for SQLite outside the disposable container layer. The original README notes that its Docker image was unoptimized and roughly 1.5 GB.

## Git tracking of documentation

The existing `.gitignore` contains a line excluding **`docs/`**. Remove that line before copying this package's `docs/` directory into the project, or Git will silently ignore these files. Run `git status --short` to confirm they are staged/tracked.

The project currently has no top-level application license; the vendored bcrypt library includes a separate license file.
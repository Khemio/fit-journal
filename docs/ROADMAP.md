# Fit Journal: development roadmap

This roadmap is based on the C++/Drogon implementation on the \`master\` branch reviewed **13 November 2026**. It prioritizes protecting user data and keeping the existing food-tracking workflow correct before adding new features.

**Scope:** The tasks below are proposed work, **not implemented features**. None of the checkboxes should be considered complete until the corresponding code and tests are merged. See [LIMITATIONS.md](LIMITATIONS.md) for the evidence behind the highest-risk items and [ARCHITECTURE.md](ARCHITECTURE.md) for the current design.

## Overall approach

- **P0 — Security and access control:** fix first. The app must remain local/private until these are addressed.
- **P1 — Data integrity and predictable behavior:** avoid silent data loss, inconsistent totals and uncaught input errors.
- **P2 — Complete the food-journal workflow:** updates, deletion, catalog management and interaction polish.
- **P3 — Quality and deployment:** repeatable tests, portable builds, persistent data and secure hosting.
- **P4 — Optional product extensions:** analytics, additional journals or a separate API, only after the core is reliable.

The roadmap is ordered by dependency and risk, **not by estimated duration**.

## P0 — Blockers for public use

### P0.1 Stop logging credentials

**Current code:** \`src/controllers/UserCtrl.cc\` prints the submitted username **and plaintext password** during sign-in.

- [ ] Remove all plaintext-password and credential-value debug output.
- [ ] Review Drogon request/access logging so credentials cannot appear in URLs, log bodies or error traces.
- [ ] If real credentials were ever logged, review local logs and rotate affected credentials where appropriate.

**Done when:** No sign-in path prints or persists plaintext passwords; an automated regression test and manual log inspection confirm it.

### P0.2 Enforce ownership for every journal query and mutation

**Current code:** \`AuthFilter\` verifies only a signed-in session. Several journal handlers query or mutate records by caller-supplied IDs without checking ownership.

- [ ] Derive the acting user ID exclusively from the authenticated session, never a form parameter.
- [ ] Scope \`GET /journals/{journal_id}\` to journals owned by the session user.
- [ ] For \`GET /journals/{journal_id}/{entry_id}\`, verify both that the journal is owned by the user **and** that the entry belongs to that journal.
- [ ] Apply the same ownership chain to creating dated entries and adding/deleting food-entry items, including item ID and \`entry-id\` submitted via HTMX.
- [ ] Use constrained \`SELECT\`, \`INSERT ... SELECT\`, \`DELETE ... WHERE ... EXISTS\` statements or transactions so validation and mutation cannot drift apart.
- [ ] Return consistent \`404\` or \`403\` responses for unauthorized records, without leaking their content.
- [ ] Add two-user integration tests for legitimate and cross-user read/create/add/delete attempts, plus deliberately mismatched journal/entry IDs.

**Done when:** Altering any route or form ID cannot read or change another user's journal, entry or item.

### P0.3 Correct sign-in and validate account inputs

**Current code:** An unknown username is rejected, but an existing username with the wrong password has no explicit error response; form values are otherwise minimally validated.

- [ ] Return \`401 Unauthorized\` for every invalid username/password combination with a visible HTMX-friendly error.
- [ ] Validate required usernames/passwords and reasonable lengths; normalize or define username rules explicitly.
- [ ] Handle duplicate usernames without a confusing redirect; do not reveal unnecessary account details.
- [ ] Review login/logout session lifecycle (including session renewal on authentication), cookie policy and session expiration.
- [ ] Add tests for successful login, incorrect password, unknown user, duplicate sign-up and logout.

**Done when:** Invalid logins are consistently rejected, session state is predictable, and errors do not disclose secrets.

### P0.4 Protect browser-based state changes

- [ ] Define and test a CSRF mitigation for authenticated HTMX/form mutations (do not assume sessions or HTMX alone provide protection).
- [ ] Configure appropriate session cookie attributes for the chosen HTTP/HTTPS deployment.
- [ ] Serve the application via HTTPS before exposing it on a public network; do not publish the current \`https: false\` development configuration as a secure deployment.
- [ ] Disable or restrict debug-only surfaces, including the configured \`/metrics\` exporter, in public deployments.

**Done when:** Mutating requests require a valid authenticated session and CSRF protection, and the deployed site uses encrypted transport.

## P1 — Data correctness and reliability

### P1.1 Make food-catalog initialization non-destructive

**Current code:** \`DBInit::init_foods()\` runs \`DELETE FROM foods\` and reinserts seed rows on every application start.

- [ ] Seed default foods only when the catalog is initially empty, or via a separate, explicit seed command.
- [ ] Preserve existing food records and IDs across restarts.
- [ ] Decide whether catalog foods are immutable defaults, global editable foods or user-specific foods before adding management endpoints.
- [ ] Test first start, repeated start, and start after a catalog record has been modified.

**Done when:** Restarting the server causes no unintended catalog data loss or renumbering.

### P1.2 Establish one schema/migration source

**Current code:** SQL definitions are duplicated in \`sql/schema.sql\` and \`src/plugins/DBInit.cc\`; \`journals.is_active\` uses the declared type \`INTEGR\`.

- [ ] Choose versioned SQLite migrations instead of two independently maintained schema definitions.
- [ ] Migrate existing databases safely, including preservation/backups of journal and user data.
- [ ] Correct the \`is_active\` type declaration in the migration.
- [ ] Add indexes for frequently joined/filtered foreign keys (\`journals.owner_id\`, \`food_entries.journal_id\`, \`food_entry_items.entry_id\`).
- [ ] Verify \`PRAGMA foreign_keys=ON\` for **every** SQLite connection; currently the config uses one database connection.
- [ ] Document how to upgrade an existing \`journal.db\` and how to roll back a failed upgrade.

**Done when:** A fresh database and an upgraded database reach the same tested schema without destructive reset.

### P1.3 Define and enforce daily-entry and nutrition invariants

- [ ] Decide whether one food-journal entry per date is required. If yes, first resolve existing duplicates and add a unique index on \`(journal_id, entry_date)\`.
- [ ] Validate dates as real calendar dates in an agreed format; reject empty or malformed strings.
- [ ] Reject non-finite, zero/negative (unless intentionally supported) and unreasonably large food multipliers.
- [ ] Specify nutrition-unit semantics and rounding/precision; currently \`Food\`/\`Item\` DTOs use \`float\` and SQLite columns use \`NUMERIC\`.
- [ ] Add database \`NOT NULL\`, \`CHECK\` and FK constraints where appropriate, without breaking existing data.
- [ ] Verify that add/delete operations keep entry totals equal to the sum of item snapshots. Add UPDATE triggers or use an explicit recalculation path before supporting edits.

**Done when:** Invalid inputs cannot corrupt totals, and calculated totals match persisted food items under all supported mutations.

### P1.4 Make errors explicit and HTMX-friendly

- [ ] Centralize parsing and validation for IDs, quantities and dates.
- [ ] Handle \`std::invalid_argument\`, \`std::out_of_range\`, missing rows and database constraint failures.
- [ ] Return meaningful 400/401/403/404/409/500 responses and render a clear message in the interface.
- [ ] Replace embedded \`<script>alert(...)</script>\` error bodies and misleading \`302\` status codes.
- [ ] Check the relative \`hx-delete\`/\`hx-post\` URLs in templates (e.g. \`FoodJournalEntry.csp\`, \`FoodItemForm.csp\`); prefer unambiguous root-relative routes.
- [ ] Rename the overloaded \`itemAdded\` refresh event into clear events or document its deliberately generic meaning.

**Done when:** Common failures show a useful message without silent success, server crashes or stale UI state.

## P2 — Finish the core food-journal workflow

### P2.1 Edit and delete dated entries

- [ ] Add an endpoint and UI to change an entry's date, following the date-uniqueness rule selected in P1.
- [ ] Add an endpoint and confirmation flow to delete an entire dated entry and its items.
- [ ] Apply P0 ownership checks and P1 input rules to all new mutations.
- [ ] Confirm FK cascades work for deletions and that HTMX updates the journal overview.

**Done when:** A user can create, inspect, edit and delete their **own** dated entries safely.

### P2.2 Edit food-entry items

- [ ] Allow changing an item's amount, or deleting and re-adding it with a deliberate user experience.
- [ ] Keep a nutritional snapshot in the entry so historical logs are not retroactively changed when a catalog food changes.
- [ ] Recalculate totals correctly on updates; cover insert, update and delete with regression tests.
- [ ] Validate any new entry/item fields and enforce record ownership.

**Done when:** A logged food can be corrected without leaving inaccurate daily totals.

### P2.3 Food catalog management

- [ ] Choose **shared catalog vs per-user catalog** and define visibility/edit permissions.
- [ ] Implement add/edit/remove food operations, with validated quantities, units, protein and calories.
- [ ] Keep seeded defaults and user-entered foods stable across restarts.
- [ ] Decide how to archive or delete foods that have historical journal snapshots.

**Done when:** Users can maintain the intended catalog without destroying historical journal data.

### P2.4 Improve interaction and accessibility

- [ ] Improve validation feedback, empty states, confirmation dialogs and navigation after HTMX swaps.
- [ ] Verify keyboard navigation, labels and error announcements for form controls and dialogs.
- [ ] Add consistent loading states and confirm no broken relative HTMX paths remain.
- [ ] Capture actual screenshots for the README after the interface is stable.

**Done when:** The main food-logging path is understandable and functional with mouse and keyboard.

## P3 — Tests, builds and deployment

### P3.1 Automated coverage and CI

**Current code:** The \`test/\` directory contains only a starter Drogon test, and \`add_subdirectory(test)\` is commented out in the root CMake file.

- [ ] Enable a runnable test target and document the command to run it.
- [ ] Use an isolated temporary SQLite database for integration tests.
- [ ] Test authentication, ownership, nutrition totals/triggers, invalid input, duplicates and HTMX response headers.
- [ ] Add GitHub Actions or equivalent CI to compile and run tests on at least one supported toolchain.
- [ ] Add sanitizers/static analysis where the platform and dependency builds support them.

**Done when:** A fresh checkout can build and run meaningful tests automatically, and regressions fail CI.

### P3.2 Reproducible local builds and Docker

- [ ] Pin compatible versions of Drogon and other build dependencies rather than relying on a floating \`latest\` image.
- [ ] Fix and test the Dockerfile's build/copy paths; use multi-stage builds with a lean runtime image if practical.
- [ ] Verify start-up working directory, configuration paths, CSP view generation, static assets and write permissions.
- [ ] Expose a persistent SQLite volume and a documented backup/restore procedure.
- [ ] Add a simple smoke test that starts the built app and requests a known route.
- [ ] Move deployment-specific settings to environment/configuration profiles, keeping development defaults separate.

**Done when:** Local and container builds are repeatable from a clean checkout and preserve data across restarts.

### P3.3 Production-readiness gate (only if public hosting is intended)

- [ ] Revisit concurrency settings, SQLite locking and transaction boundaries under multiple requests.
- [ ] Put TLS termination, session cookies, CSRF, access logging and metrics exposure behind explicit deployment policies.
- [ ] Include safe error pages, request limits, backup/restore checks and basic health monitoring.
- [ ] Review vendored dependency licenses and choose a top-level project license.
- [ ] Do a final cross-user authorization and data-restoration test on the release candidate.

**Done when:** P0 is complete, relevant P1 issues are closed, and the deployment-specific security/data checks pass. Until then, describe the app as a learning prototype.

## P4 — Optional future directions

These are choices, **not required fixes**:

- **Nutrition analytics:** rolling averages, daily/weekly protein and calorie charts, trends and target tracking.
- **Expanded journals:** build actual life/sport workflows, or remove their unused placeholders if food logging remains the focus.
- **Separate HTTP/JSON API:** useful for mobile/desktop clients or a future Python backend, but not necessary for the existing server-rendered HTMX architecture.
- **Portability and packaging:** provide scripted MSYS2 UCRT setup or Linux instructions with verified dependency versions.

## Suggested implementation sequence

1. **Security patch:** P0.1 → P0.2 → P0.3 → P0.4. Add focused regression tests alongside each fix.
2. **Stable storage:** P1.1 → P1.2 → P1.3 → P1.4. Back up and migrate existing databases safely.
3. **Feature completion:** P2.1 → P2.2 → P2.3 → P2.4. Extend tests for every new mutation.
4. **Engineering hardening:** P3.1 → P3.2 → P3.3. Automate checks and test the deployment path.
5. **Optional expansion:** choose from P4 based on whether the project is for learning, a portfolio, or real users.

A good **first pull request** removes plaintext-password logging, corrects the wrong-password response, and adds authentication tests. The next should close all cross-user record-access paths **before** adding more CRUD endpoints.

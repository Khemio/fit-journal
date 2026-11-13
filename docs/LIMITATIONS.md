# Known limitations and recommended next steps

The findings below are based on the source in the repository's `master` branch as reviewed on **13 November 2026**. They distinguish features present in the code from planned enhancements. They are not the results of a runtime penetration test.

## Highest-priority issues before public deployment

### 1. Enforce per-record authorization (critical)

`AuthFilter` only checks the `loggedIn` session flag. For example, `Journals::get_journal` selects food entries by supplied `journal_id`; `Journals::get_entry` selects items by `entry_id`; and add/delete endpoints act on caller-supplied IDs without checking their ownership. A signed-in user could potentially access or modify another user's data by changing IDs in requests.

**Recommended fix:** Scope every read and mutation through the authenticated session user and the complete ownership chain (`users → journals → food_entries → food_entry_items`). Use SQL joins or `EXISTS` guards, enforce authorization in each handler, and add cross-user regression tests. Do this before hosting the app publicly.

### 2. Stop logging passwords (critical)

`Users::SignIn` currently prints the submitted username **and plaintext password** to standard output. Remove password logging entirely and ensure request logging does not capture secrets. Check prior local logs for credential exposure if any real accounts were used.

### 3. Use HTTPS and review session/CSRF protections (high)

`config.json` listens over HTTP (`https: false`), and HTTPS was previously planned. Use TLS behind a trusted reverse proxy or with Drogon, configure secure session cookies, review session lifecycle and CSRF protections for the form-based mutation routes, and remove unnecessary exposure of debug/metrics endpoints.

### 4. Validate inputs and handle errors consistently (high)

Handlers accept user-provided IDs, dates and numeric multipliers. `std::stof`/`std::stoul` can throw; nonnegative/finite amount validation is incomplete; wrong-password sign-in lacks an explicit failure branch; and some error handlers respond with a redirect and embedded alert script. Add server-side validation and consistent HTTP/HTMX error views.

## Data correctness and maintainability

### 5. Prevent duplicate daily entries

There is no unique constraint on `(journal_id, entry_date)`, even though a date is intended to represent one journal day. Add an explicit constraint, decide how duplicates should be reported, and handle the resulting SQLite constraint errors.

### 6. Stop destructive catalog reseeding

`DBInit::init_foods` deletes all `foods` rows at each startup before inserting defaults. Make seeding an explicit, idempotent first-run operation or migration, and keep catalog IDs stable. The current code does not include a user-facing method for maintaining the catalog.

### 7. Centralize schema changes

`sql/schema.sql` and `src/plugins/DBInit.cc` independently define tables and triggers. Use migrations or a single migration source of truth rather than duplicating schema text. Enable foreign-key enforcement reliably on every SQLite connection if connection count is later increased.

### 8. Support updates and deletion of complete entries

Currently, users can create dated entries and add/delete items, but cannot edit existing items, edit dates, delete an entire daily entry, or edit catalog foods. Implement explicit update endpoints, their corresponding views, and a trigger/recalculation strategy for any changed nutritional values.

### 9. Add domain and database constraints

Potential improvements include validating item quantities, handling nutrition precision and rounding, adding appropriate foreign-key indexes, and enforcing meaningful journal types. Consider making `entry_date` a validated ISO date and adding `CHECK` constraints for nonnegative quantities/nutrition values. The current `journals.is_active` declaration has a `INTEGR` typo (SQLite accepts flexible declared type names, but this should be corrected via a migration if needed).

## Functional roadmap

The following items have been discussed as extensions, not as existing capabilities:

- Better user-facing error messages and validations throughout the HTMX interface.
- Edit, delete, and date-uniqueness rules for food-journal entries.
- Manage the shared/personal food catalog through the UI.
- Summary views, trends, and averages across days.
- Expand the placeholder `life` and `sport` journal types if they remain in scope.
- Harden authentication, authorization, HTTPS and deployment settings.
- Integration tests for cross-user access, SQLite triggers and common workflows.

## Suggested development order

1. **Security pass:** Remove password logging, enforce ownership at every record-level route, handle invalid credentials correctly, and add negative tests.
2. **Persistence/data pass:** Remove startup catalog deletion, unify schema/migrations, add date uniqueness and validation rules.
3. **Edit operations:** Implement update/delete of food-journal entries and items, update UI, and keep totals correct.
4. **Product polish:** Add error UI, screenshots, reliable container build and optional nutrition summaries.
5. **Deployment:** Add HTTPS, session/CSRF hardening, backups, runtime monitoring and an appropriately scoped licensing decision.

## Deliberately not claimed

No assertion is made here that HTTPS, production-ready multi-user isolation, automated integration tests, average-statistics dashboards, life/sport tracking, or an editable food catalog are implemented. The project is better presented as a focused C++/Drogon learning project with meaningful working features and clear next steps.

For a milestone-by-milestone implementation plan and acceptance criteria, see [ROADMAP.md](ROADMAP.md).
